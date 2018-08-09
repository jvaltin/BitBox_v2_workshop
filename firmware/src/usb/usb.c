/*

 The MIT License (MIT)

 Copyright (c) 2018 Douglas J. Bakkum

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
#include "driver_init.h"
#include "hiddf_generic.h"
#include "hiddf_generic_desc.h"
#include "usb_apdu.h"
#include "u2f/u2f_hid.h"
#include "util.h"
#include "usb.h"


#define USB_QUEUE_NUM_PACKETS 128


static bool usb_hww_enabled = false;
static uint8_t usb_reply_queue_packets[USB_QUEUE_NUM_PACKETS][USB_REPORT_SIZE];
static uint32_t usb_reply_queue_index_start = 0;
static uint32_t usb_reply_queue_index_end = 0;
static uint8_t usb_hid_generic_out_report[UDI_HID_REPORT_OUT_SIZE];
static uint8_t usb_ctrl_endpoint_buffer[USB_REPORT_SIZE];
static const uint8_t usb_hid_report[] = {CONF_USB_HID_GENERIC_REPORT};
static uint8_t usb_desc_bytes[] = {HID_GENERIC_DESCES_LS_FS};// Device descriptors and Configuration descriptors list.
static struct usbd_descriptors usb_desc[] = {{usb_desc_bytes, usb_desc_bytes + sizeof(usb_desc_bytes)}
#if CONF_USBD_HS_SP
                                             , {NULL, NULL}
#endif
};


static bool usb_device_cb_generic_out(const uint8_t ep, const enum usb_xfer_code rc, const uint32_t count)
{
	(void) ep;
	(void) rc;
	(void) count;

    usb_hww_report((const unsigned char *)usb_hid_generic_out_report);
	hiddf_generic_read(usb_hid_generic_out_report, UDI_HID_REPORT_OUT_SIZE);

	// No error. 
	return false;
}


void usb_init(void)
{
	/* usb stack init */
	usbdc_init(usb_ctrl_endpoint_buffer);

	/* usbdc_register_funcion inside */
	hiddf_generic_init(usb_hid_report, CONF_USB_HID_GENERIC_REPORT_LEN);

	usbdc_start(usb_desc);
	usbdc_attach();

	while (!hiddf_generic_is_enabled()) {
		// wait hid generic to be installed
	};
	
    hiddf_generic_register_callback(HIDDF_GENERIC_CB_READ, (FUNC_PTR)usb_device_cb_generic_out);
    hiddf_generic_register_callback(HIDDF_GENERIC_CB_WRITE, (FUNC_PTR)usb_report_sent);
	hiddf_generic_read(usb_hid_generic_out_report, UDI_HID_REPORT_OUT_SIZE); // Wait for data
}


void usb_hww_report(const unsigned char *command)
{
    usb_reply_queue_clear();
    usb_apdu_device_run((const USB_FRAME *)command);
}


void usb_report_sent(void)
{
    usb_reply_queue_send();
}


void usb_reply(uint8_t *report)
{
    if (report) {
        hiddf_generic_write(report, UDI_HID_REPORT_IN_SIZE);
    }
}


uint8_t *usb_reply_queue_read(void)
{
    uint32_t p = usb_reply_queue_index_start;
    if (p == usb_reply_queue_index_end) {
        // queue is empty
        return NULL;
    }
    usb_reply_queue_index_start = (p + 1) % USB_QUEUE_NUM_PACKETS;
    return usb_reply_queue_packets[p];
}


void usb_reply_queue_clear(void)
{
    usb_reply_queue_index_start = usb_reply_queue_index_end;
}


void usb_reply_queue_add(const USB_FRAME *frame)
{
    uint32_t next = (usb_reply_queue_index_end + 1) % USB_QUEUE_NUM_PACKETS;
    if (usb_reply_queue_index_start == next) {
        return; // Buffer full
    }
    memcpy(usb_reply_queue_packets[usb_reply_queue_index_end], frame, USB_REPORT_SIZE);
    usb_reply_queue_index_end = next;
}


void usb_reply_queue_load_msg(const uint8_t cmd, const uint8_t *data, const uint32_t len,
                              const uint32_t cid)
{
    USB_FRAME f;
    uint32_t cnt = 0;
    uint32_t l = len;
    uint32_t psz;
    uint8_t seq = 0;

    memset(&f, 0, sizeof(f));
    f.cid = cid;
    f.init.cmd = cmd;
    f.init.bcnth = len >> 8;
    f.init.bcntl = len & 0xff;

    // Init packet
    psz = MIN(sizeof(f.init.data), l);
    memcpy(f.init.data, data, psz);
    usb_reply_queue_add(&f);
    l -= psz;
    cnt += psz;

    // Cont packet(s)
    for (; l > 0; l -= psz, cnt += psz) {
        memset(&f.cont.data, 0, sizeof(f.cont.data));
        f.cont.seq = seq++;
        psz = MIN(sizeof(f.cont.data), l);
        memcpy(f.cont.data, data + cnt, psz);
        usb_reply_queue_add(&f);
    }
}


void usb_reply_queue_send(void)
{
    static uint8_t *data;
    data = usb_reply_queue_read();
    usb_reply(data);
}


void usb_set_feature(uint8_t *report)
{
    (void) report;
}


void usb_suspend_action(void) {}


void usb_resume_action(void) {}


void usb_remotewakeup_enable(void) {}


void usb_remotewakeup_disable(void) {}


bool usb_hww_enable(void)
{
    usb_hww_enabled = true;
    return true;
}


void usb_hww_disable(void)
{
    usb_hww_enabled = false;
}
