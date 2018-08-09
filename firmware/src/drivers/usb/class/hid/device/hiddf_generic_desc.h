/**
 * \file
 *
 * \brief USB Device Stack HID Generic Function Descriptor Setting.
 *
 * Copyright (C) 2015 - 2017 Atmel Corporation. All rights reserved.
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 * Atmel AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#ifndef USBDF_HID_GENERIC_DESC_H_
#define USBDF_HID_GENERIC_DESC_H_

#include "usb_protocol.h"
#include "usb_protocol_hid.h"
#include "usbd_config.h"

#define HID_GENERIC_DEV_DESC                                                                                           \
	USB_DEV_DESC_BYTES(CONF_USB_HID_GENERIC_BCDUSB,                                                                    \
	                   0x00,                                                                                           \
	                   0x00,                                                                                           \
	                   0x00,                                                                                           \
	                   CONF_USB_HID_GENERIC_BMAXPKSZ0,                                                                 \
	                   CONF_USB_HID_GENERIC_IDVENDER,                                                                  \
	                   CONF_USB_HID_GENERIC_IDPRODUCT,                                                                 \
	                   CONF_USB_HID_GENERIC_BCDDEVICE,                                                                 \
	                   CONF_USB_HID_GENERIC_IMANUFACT,                                                                 \
	                   CONF_USB_HID_GENERIC_IPRODUCT,                                                                  \
	                   CONF_USB_HID_GENERIC_ISERIALNUM,                                                                \
	                   0x01)

#define HID_GENERIC_CFG_DESC                                                                                           \
	USB_CONFIG_DESC_BYTES(                                                                                             \
	    41, 1, 0x01, CONF_USB_HID_GENERIC_ICONFIG, CONF_USB_HID_GENERIC_BMATTRI, CONF_USB_HID_GENERIC_BMAXPOWER)

#define HID_GENERIC_IFC_DESC                                                                                           \
	USB_IFACE_DESC_BYTES(CONF_USB_HID_GENERIC_BIFCNUM, 0x00, 0x02, 0x03, 0x00, 0x00, CONF_USB_HID_GENERIC_IIFC)        \
	, USB_HID_DESC_BYTES(0x09, 0x21, 0x01, 0x22, CONF_USB_HID_GENERIC_REPORT_LEN),                                     \
	    USB_ENDP_DESC_BYTES(CONF_USB_HID_GENERIC_INTIN_EPADDR, 0x03, CONF_USB_HID_GENERIC_INTIN_MAXPKSZ, 10),          \
	    USB_ENDP_DESC_BYTES(CONF_USB_HID_GENERIC_INTOUT_EPADDR, 0x03, CONF_USB_HID_GENERIC_INTOUT_MAXPKSZ, 10)

#define HID_GENERIC_STR_DESCES                                                                                         \
	CONF_USB_HID_GENERIC_LANGID_DESC                                                                                   \
	CONF_USB_HID_GENERIC_IMANUFACT_STR_DESC                                                                            \
	CONF_USB_HID_GENERIC_IPRODUCT_STR_DESC                                                                             \
	CONF_USB_HID_GENERIC_ISERIALNUM_STR_DESC                                                                           \
	CONF_USB_HID_GENERIC_ICONFIG_STR_DESC

/** USB Device descriptors and configuration descriptors */
#define HID_GENERIC_DESCES_LS_FS                                                                                       \
	HID_GENERIC_DEV_DESC, HID_GENERIC_CFG_DESC, HID_GENERIC_IFC_DESC, HID_GENERIC_STR_DESCES

#endif /* USBDF_HID_GENERIC_DESC_H_ */
