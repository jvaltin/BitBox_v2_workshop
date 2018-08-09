/*============================================================================
Filename : datastreamer.h
Project : QTouch Modular Library
Purpose : Prototypes for functions in datastreamer_UART_xxx.c file

This file is part of QTouch Modular Library example application.

Important Note: Do not edit this file manually.
                Use QTouch Configurator within Atmel Start to apply any
                modifications to this file.

Usage License: Refer license.h file for license information
Support: Visit http://www.microchip.com/support/hottopics.aspx
               to create MySupport case.

------------------------------------------------------------------------------
Copyright (c) 2017 Microchip. All rights reserved.
------------------------------------------------------------------------------
============================================================================*/

#ifndef __DATASTREAMER_H_
#define __DATASTREAMER_H_

/*----------------------------------------------------------------------------
 *     include files
 *----------------------------------------------------------------------------*/
#include "qtouch.h"

#if (DEF_TOUCH_DATA_STREAMER_ENABLE == 1u)

/*----------------------------------------------------------------------------
 *     defines
 *----------------------------------------------------------------------------*/
#define USER_BOARD 0x0000
#define TINY_XPLAINED_MINI 0xF012
#define TINY_XPLAINED_PRO 0xF013
#define TINY_MOISTURE_DEMO 0xF014
#define MEGA_328PB_XPLAINED_MINI 0xF015
#define MEGA_324PB_XPLAINED_PRO 0xF016

/*----------------------------------------------------------------------------
 *   prototypes
 *----------------------------------------------------------------------------*/
void datastreamer_init(void);
void datastreamer_output(void);

#endif

#endif
