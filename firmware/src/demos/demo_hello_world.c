/*
 Copyright (c) 2018 Jonas Schnelli, Douglas J. Bakkum
 The MIT License (MIT)
*/


#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "driver_init.h"
#include "screen.h"
#include "ugui.h"

#include "demo_list.h"


void demo_hello_world(void)
{
    int flip = 0;

    UG_FontSelect(&FONT_6X10);
    
    while (1) {
        UG_ClearBuffer();
        if (flip) {
            UG_FillFrame(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, C_WHITE); 
            UG_SetForecolor(C_BLACK);
            UG_SetBackcolor(C_WHITE);
        } else {
            UG_FillFrame(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, C_BLACK); 
            UG_SetForecolor(C_WHITE);
            UG_SetBackcolor(C_BLACK);
        }
        UG_PutString(26, 30, "Hello world!");
        UG_SendBuffer();
        delay_ms(5000);
        flip = !flip;
    }
}
