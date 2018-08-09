
#include <stdint.h>
#include <stdbool.h>
#include <hal_delay.h>
#include "ugui.h"
#include "ssd1306.h"
#include "screen.h"


static UG_GUI guioled; // Global GUI structure for OLED screen
static bool screen_led_is_on = false;


void screen_init(void)
{
    UG_Init(&guioled, (void(*)(UG_S16,UG_S16,UG_COLOR))OLED_PSET, SCREEN_WIDTH, SCREEN_HEIGHT);
    UG_SelectGUI(&guioled);
}


void screen_print_debug(const char *message, const UG_FONT *font, int duration)
{
	UG_ClearBuffer();
    font ? UG_FontSelect(font) : UG_FontSelect(&FONT_6X10);
    UG_PutString(0, 0, message);
    UG_Update();
    UG_SendBuffer();
	delay_ms(duration);
}


void screen_led_on(void)
{
    UG_U16 l = 10;
    screen_led_is_on = true;
    UG_FillFrame(SCREEN_WIDTH - l, 0, SCREEN_WIDTH, l, C_WHITE); 
    UG_Update();
    UG_SendBuffer();
}


void screen_led_off(void)
{
    UG_U16 l = 10;
    screen_led_is_on = false;
    UG_FillFrame(SCREEN_WIDTH - l, 0, SCREEN_WIDTH, l, C_BLACK); 
    UG_Update();
    UG_SendBuffer();
}


void screen_led_toggle(void)
{
    screen_led_is_on ? screen_led_off() : screen_led_on();
}


void screen_led_blink(void)
{
    screen_led_on();
    delay_ms(300);
    screen_led_off();
}


void screen_led_abort(void)
{
    screen_led_off();
    delay_ms(300);
    screen_led_on();
    delay_ms(100);
    screen_led_off();
    delay_ms(100);
    screen_led_on();
    delay_ms(100);
    screen_led_off();
}


void screen_splash(const char *version, int duration)
{
    uint8_t b, x0, x1, x, y0, y1, y, trim;
    
	UG_ClearBuffer();
	UG_FontSelect(&FONT_16X26);
	UG_ConsoleSetBackcolor(C_BLACK);
	UG_ConsoleSetForecolor(C_WHITE);
  
    // Swiss flag dimensions
    b = 6; // size; must be multiples of 6 in order to avoid misaligned pixels
    x = 3; // x offset
    y = 64 / 2 - b * 5 / 2; // y offset
    trim = 2; // trim outer square below official specified dimensions
    
    // outer square
    x0 = x + trim;
    x1 = x + b * 5 - trim;
    y0 = y + trim;
    y1 = y + b * 5 - trim;
    UG_FillFrame(x0, y0, x1, y1, C_WHITE); 
    
    // horizontal line
    x0 = x + b * 5 / 6;
    x1 = x0 + b * 10 / 3;
    y0 = y + b * 2;
    y1 = y0 + b;
    UG_FillFrame(x0, y0, x1, y1, C_BLACK); 

    // vertical line
    x0 = x + b * 2;
    x1 = x0 + b;
    y0 = y + b * 5 / 6;
    y1 = y0 + b * 10 / 3;
    UG_FillFrame(x0, y0, x1, y1, C_BLACK); 
   
    // BitBox
    x0 = x + b * 6;
    y0 = y + 2;
    UG_SetBackcolor(C_BURLY_WOOD);
    UG_PutCharTransparent('B', x0, y0, C_WHITE);
    x0 += 12;
    UG_PutCharTransparent('i', x0, y0, C_WHITE);
    x0 += 10;
    UG_PutCharTransparent('t', x0, y0, C_WHITE);
    x0 += 16;
    UG_PutCharTransparent('B', x0, y0, C_WHITE);
    x0 += 15;
    UG_PutCharTransparent('o', x0, y0, C_WHITE);
    x0 += 13;
    UG_PutCharTransparent('x', x0, y0, C_WHITE);
    UG_SetBackcolor(C_BLACK);

    // version
	UG_FontSelect(&FONT_5X8);
	UG_PutString(x + 80, y + 40, version);
    
    UG_Update();
    UG_SendBuffer();
	delay_ms(duration);
    UG_ClearBuffer();
}
