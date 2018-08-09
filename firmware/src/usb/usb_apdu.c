/*

 The MIT License (MIT)

 Copyright (c) 2016-2017 Douglas J. Bakkum, Shift Devices AG

 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the "Software"),
 to deal in the Software without restriction, including without limitation
 the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
 OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*/


#include <string.h>

#include "usb.h"
#include "screen.h"
#include "random.h"
#include "commander.h"
#include "util.h"

#include "u2f/u2f.h"
#include "u2f/u2f_hid.h"
#include "usb_apdu.h"


#define U2F_READBUF_MAX_LEN  7609// Max allowed by U2F specification = (57 + 128 * 59) = 7609. 


static uint32_t cid = 0;
volatile bool u2f_state_continue = false;


typedef struct {
    uint8_t appId[U2F_APPID_SIZE];
    uint8_t flags;
    uint8_t ctr[4];
    uint8_t challenge[U2F_NONCE_LENGTH];
} U2F_AUTHENTICATE_SIG_STR;


typedef struct {
    uint8_t buf[U2F_READBUF_MAX_LEN];
    uint8_t *buf_ptr;
    uint32_t len;
    uint8_t seq;
    uint8_t cmd;
} U2F_ReadBuffer;


static U2F_ReadBuffer reader;


static uint32_t next_cid(void)
{
    do {
        cid = random_uint32();
    } while (cid == 0 || cid == U2FHID_CID_BROADCAST);
    return cid;
}


void usb_apdu_send_message(const uint8_t *data, const uint32_t len)
{
    usb_reply_queue_load_msg(U2FHID_MSG, data, len, cid);
}


void usb_apdu_send_err_hid(uint32_t fcid, uint8_t err)
{
    USB_FRAME f;

    memset(&f, 0, sizeof(f));
    f.cid = fcid;
    f.init.cmd = U2FHID_ERROR;
    f.init.bcntl = 1;
    f.init.data[0] = err;
    usb_reply_queue_add(&f);
}


static void usb_apdu_device_reset_state(void)
{
    memset(&reader, 0, sizeof(reader));
    u2f_state_continue = false;
}


static void usb_apdu_device_init(const USB_FRAME *in)
{
    const U2FHID_INIT_REQ *init_req = (const U2FHID_INIT_REQ *)&in->init.data;
    USB_FRAME f;
    U2FHID_INIT_RESP resp;

    if (in->cid == 0) {
        usb_apdu_send_err_hid(in->cid, U2FHID_ERR_INVALID_CID);
        return;
    }

    memset(&f, 0, sizeof(f));
    f.cid = in->cid;
    f.init.cmd = U2FHID_INIT;
    f.init.bcnth = 0;
    f.init.bcntl = U2FHID_INIT_RESP_SIZE;

    memcpy(resp.nonce, init_req->nonce, sizeof(init_req->nonce));
    resp.cid = in->cid == U2FHID_CID_BROADCAST ? next_cid() : in->cid;
    resp.versionInterface = U2FHID_IF_VERSION;
    resp.versionMajor = 0;
    resp.versionMinor = 0;
    resp.versionBuild = 0;
    resp.capFlags = U2FHID_CAPFLAG_WINK;
    memcpy(&f.init.data, &resp, sizeof(resp));
    usb_reply_queue_add(&f);
}


static void usb_apdu_device_cmd_cont(const USB_FRAME *f)
{
    (void) f;

    if ((reader.buf_ptr - reader.buf) < (signed)reader.len) {
        // Need more data
        return;
    }

    u2f_state_continue = false;

    // Received all data
    switch (reader.cmd) {
        case U2FHID_HWW: {
            char *report;
            reader.buf[MIN(reader.len, sizeof(reader.buf) - 1)] = '\0';// NULL terminate
            report = commander((const char *)reader.buf);
            usb_reply_queue_load_msg(U2FHID_HWW, (const uint8_t *)report, strlens(report), cid);
            break;
        }
        default:
            usb_apdu_send_err_hid(cid, U2FHID_ERR_INVALID_CMD);
            break;
    }

    // Finished
    usb_apdu_device_reset_state();
    cid = 0;
}


static void usb_apdu_device_cmd_init(const USB_FRAME *f)
{
    if (f->cid == U2FHID_CID_BROADCAST || f->cid == 0) {
        usb_apdu_send_err_hid(f->cid, U2FHID_ERR_INVALID_CID);
        return;
    }

    if ((unsigned)U2FHID_MSG_LEN(*f) > sizeof(reader.buf)) {
        usb_apdu_send_err_hid(f->cid, U2FHID_ERR_INVALID_LEN);
        return;
    }

    memset(&reader, 0, sizeof(reader));
    reader.seq = 0;
    reader.buf_ptr = reader.buf;
    reader.len = U2FHID_MSG_LEN(*f);
    reader.cmd = f->type;
    memcpy(reader.buf_ptr, f->init.data, sizeof(f->init.data));
    reader.buf_ptr += sizeof(f->init.data);
    cid = f->cid;

    u2f_state_continue = true;
    usb_apdu_device_cmd_cont(f);
}


void usb_apdu_device_run(const USB_FRAME *f)
{
    if ((f->type & U2FHID_TYPE_MASK) == U2FHID_TYPE_INIT) {

        if (f->init.cmd == U2FHID_INIT) {
            usb_apdu_device_init(f);
            if (f->cid == cid) {
                usb_apdu_device_reset_state();
            }
        } else if (u2f_state_continue) {
            if (f->cid == cid) {
                usb_reply_queue_clear();
                usb_apdu_device_reset_state();
                usb_apdu_send_err_hid(f->cid, U2FHID_ERR_INVALID_SEQ);
            } else {
                usb_apdu_send_err_hid(f->cid, U2FHID_ERR_CHANNEL_BUSY);
            }
        } else {
            usb_apdu_device_cmd_init(f);
        }
        goto exit;
    }

    if ((f->type & U2FHID_TYPE_MASK) == U2FHID_TYPE_CONT) {

        if (!u2f_state_continue) {
            goto exit;
        }

        if (cid != f->cid) {
            usb_apdu_send_err_hid(f->cid, U2FHID_ERR_CHANNEL_BUSY);
            goto exit;
        }

        if (reader.seq != f->cont.seq) {
            usb_reply_queue_clear();
            usb_apdu_device_reset_state();
            usb_apdu_send_err_hid(f->cid, U2FHID_ERR_INVALID_SEQ);
            goto exit;
        }

        // Check bounds
        if ((reader.buf_ptr - reader.buf) >= (signed) reader.len
                || (reader.buf_ptr + sizeof(f->cont.data) - reader.buf) > (signed) sizeof(
                    reader.buf)) {
            goto exit;
        }

        reader.seq++;
        memcpy(reader.buf_ptr, f->cont.data, sizeof(f->cont.data));
        reader.buf_ptr += sizeof(f->cont.data);
        usb_apdu_device_cmd_cont(f);
    }

exit:
    usb_reply_queue_send();
}
