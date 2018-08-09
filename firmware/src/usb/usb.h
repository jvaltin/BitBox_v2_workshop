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


#ifndef _USB_H_
#define _USB_H_


#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


#define  USB_REPORT_SIZE 64
#define  USB_DEVICE_VENDOR_ID             0x03EB
#define  USB_DEVICE_PRODUCT_ID            0x2402
#define  USB_DEVICE_MAJOR_VERSION         DIGITAL_BITBOX_VERSION_MAJOR
#define  USB_DEVICE_MINOR_VERSION         DIGITAL_BITBOX_VERSION_MINOR
#define  USB_DEVICE_POWER                 100// Consumption (mA)
#define  USB_DEVICE_ATTR                  (USB_CONFIG_ATTR_BUS_POWERED)
#define  USB_DEVICE_MANUFACTURE_NAME      "shiftcrypto.ch"
#define  USB_DEVICE_PRODUCT_NAME          "Bitbox Defcon Demo"
#define  USB_DEVICE_SERIAL_NAME_TYPE      "defcon:v0.0.0"

#define  UDI_HID_REPORT_IN_SIZE      USB_REPORT_SIZE
#define  UDI_HID_REPORT_OUT_SIZE     USB_REPORT_SIZE
#define  UDI_HID_REPORT_FEATURE_SIZE 8
#define  UDI_HID_EP_SIZE             64


__extension__ typedef struct {
    uint32_t cid;               // Channel identifier
    union {
        uint8_t type;           // Frame type - bit 7 defines type
        struct {
            uint8_t cmd;        // Command - bit 7 set
            uint8_t bcnth;      // Message byte count - high
            uint8_t bcntl;      // Message byte count - low
            uint8_t data[USB_REPORT_SIZE - 7]; // Data payload
        } init;
        struct {
            uint8_t seq;        // Sequence number - bit 7 cleared
            uint8_t data[USB_REPORT_SIZE - 5]; // Data payload
        } cont;
    };
} USB_FRAME;


typedef struct {
    uint8_t cla, ins, p1, p2;
    uint8_t lc1, lc2, lc3;
    uint8_t data[];
} USB_APDU;


void usb_init(void);

void usb_reply_queue_clear(void);
void usb_reply_queue_add(const USB_FRAME *frame);
void usb_reply_queue_load_msg(const uint8_t cmd, const uint8_t *data, const uint32_t len,
                              const uint32_t cid);
void usb_reply_queue_send(void);
uint8_t *usb_reply_queue_read(void);
void usb_reply(uint8_t *report);

void usb_suspend_action(void);
void usb_resume_action(void);
void usb_remotewakeup_enable(void);
void usb_remotewakeup_disable(void);

void usb_set_feature(uint8_t *report);
void usb_report_sent(void);

bool usb_hww_enable(void);
void usb_hww_disable(void);
void usb_hww_report(const unsigned char *command);


#endif
