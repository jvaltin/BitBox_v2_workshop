/*
 Copyright (c) 2018 Douglas J. Bakkum
 The MIT License (MIT)
*/


#ifndef _DRIVER_INIT_H_
#define _DRIVER_INIT_H_


#include <utils.h>
#include <hal_atomic.h>
#include <hal_delay.h>
#include <hal_gpio.h>
#include <hal_init.h>
#include <hal_io.h>
#include <hal_sleep.h>
#include <hal_timer.h>
#include <spi_lite.h>
#include <hal_i2c_m_sync.h>
#include <hal_usart_async.h>
#include <hal_mci_sync.h>
#include <hal_usb_device.h>
#include <hal_aes_sync.h>
#include <hal_sha_sync.h>
#include <hal_rand_sync.h>
#include <hal_flash.h>


// SAMD51 has 14 pin functions
#define GPIO_PIN_FUNCTION_A 0
#define GPIO_PIN_FUNCTION_B 1
#define GPIO_PIN_FUNCTION_C 2
#define GPIO_PIN_FUNCTION_D 3
#define GPIO_PIN_FUNCTION_E 4
#define GPIO_PIN_FUNCTION_F 5
#define GPIO_PIN_FUNCTION_G 6
#define GPIO_PIN_FUNCTION_H 7
#define GPIO_PIN_FUNCTION_I 8
#define GPIO_PIN_FUNCTION_J 9
#define GPIO_PIN_FUNCTION_K 10
#define GPIO_PIN_FUNCTION_L 11
#define GPIO_PIN_FUNCTION_M 12
#define GPIO_PIN_FUNCTION_N 13

#define PIN_OLED_ON GPIO(GPIO_PORTA, 0)
#define PIN_SD_CD GPIO(GPIO_PORTA, 1)
#define PIN_SD_PWON GPIO(GPIO_PORTA, 2)
#define PA04 GPIO(GPIO_PORTA, 4)
#define PA05 GPIO(GPIO_PORTA, 5)
#define PA06 GPIO(GPIO_PORTA, 6)
#define PIN_ATAES_CS GPIO(GPIO_PORTA, 7)
#define PA08 GPIO(GPIO_PORTA, 8)
#define PA09 GPIO(GPIO_PORTA, 9)
#define PA10 GPIO(GPIO_PORTA, 10)
#define PA11 GPIO(GPIO_PORTA, 11)
#define PA12 GPIO(GPIO_PORTA, 12)
#define PA13 GPIO(GPIO_PORTA, 13)
#define PA16 GPIO(GPIO_PORTA, 16)
#define PA17 GPIO(GPIO_PORTA, 17)
#define PA18 GPIO(GPIO_PORTA, 18)
#define PIN_OLED_CS GPIO(GPIO_PORTA, 19)
#define PIN_I2C_SCL GPIO(GPIO_PORTA, 22)
#define PIN_I2C_SDA GPIO(GPIO_PORTA, 23)
#define PA24 GPIO(GPIO_PORTA, 24)
#define PA25 GPIO(GPIO_PORTA, 25)
#define PIN_NU1 GPIO(GPIO_PORTA, 27)
#define PB00 GPIO(GPIO_PORTB, 0)
#define PB01 GPIO(GPIO_PORTB, 1)
#define PB02 GPIO(GPIO_PORTB, 2)
#define PB03 GPIO(GPIO_PORTB, 3)
#define PB10 GPIO(GPIO_PORTB, 10)
#define PB11 GPIO(GPIO_PORTB, 11)
#define PB12 GPIO(GPIO_PORTB, 12)
#define PB13 GPIO(GPIO_PORTB, 13)
#define PB14 GPIO(GPIO_PORTB, 14)
#define PB15 GPIO(GPIO_PORTB, 15)
#define PIN_OLED_RES GPIO(GPIO_PORTB, 16)
#define PIN_OLED_CMD GPIO(GPIO_PORTB, 17)
#define PIN_NU2 GPIO(GPIO_PORTB, 22)


extern struct timer_descriptor Timer;
extern struct usart_async_descriptor USART;
extern struct i2c_m_sync_desc I2C_0;
extern struct mci_sync_desc MCI_0;
extern struct aes_sync_descriptor CRYPTOGRAPHY_0;
extern struct sha_sync_descriptor HASH_ALGORITHM_0;
extern struct flash_descriptor FLASH_0;
extern struct rand_sync_desc RAND_0;


/**
 * \brief Perform system initialization, initialize pins and clocks for
 * peripherals
 */
void system_init(void);


#endif
