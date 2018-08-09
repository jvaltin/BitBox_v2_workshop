#ifndef _FLASH_H_
#define _FLASH_H_


#include <stdint.h>
#include "ugui.h"


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64


void screen_init(void);
void screen_print_debug(const char *message, const UG_FONT *font, int duration);
void screen_led_on(void);
void screen_led_off(void);
void screen_led_toggle(void);
void screen_led_blink(void);
void screen_led_abort(void);
void screen_splash(const char *version, int duration);


#endif
