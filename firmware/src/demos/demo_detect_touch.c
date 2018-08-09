/*
 Copyright (c) 2018 Jonas Schnelli, Douglas J. Bakkum
 The MIT License (MIT)
*/


#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "driver_init.h"
#include "qtouch.h"
#include "screen.h"
#include "ugui.h"
#include "util.h"
#include "demo_list.h"


#define LOOP_DELAY_MS     1
#define RESET_HOLD_CNT  300


extern volatile uint8_t measurement_done_touch;
uint16_t slider_position[DEF_NUM_SCROLLERS];
uint16_t active_timer[DEF_NUM_SCROLLERS];


static void touch_slider_update(uint16_t nr)
{
    if (0u != qtouch_get_scroller_state(nr)) {
        slider_position[nr] = (255 - qtouch_get_scroller_position(nr));
        active_timer[nr]++;
    } else {
        active_timer[nr] = 0;
    }
}


void demo_detect_touch(void)
{
    uint16_t i;
    uint16_t touch_delta;
    char value_slider_0[32];
    char value_slider_1[32];
    char value_button[8][16];
    
    UG_FontSelect(&FONT_6X10);
    
    while (1) {
        
        qtouch_process();

        if (measurement_done_touch == 1) {
            touch_slider_update(0);
            touch_slider_update(1);
            measurement_done_touch = 0;

            for (i = 0; i < 8; i++) {
                touch_delta = MAX(0, qtouch_get_sensor_node_signal(i) - qtouch_get_sensor_node_reference(i));
                snprintf(value_button[i], sizeof(value_button) / 8, "%02x", (touch_delta / 4) * 4);// round to resolution of 4
            }

            if (active_timer[0] > RESET_HOLD_CNT && active_timer[1] > RESET_HOLD_CNT) {
                screen_print_debug("Restarting MCU", NULL, 0);
                delay_ms(900);
                _reset_mcu();
            }

            if (active_timer[0])
                snprintf(value_slider_0, sizeof(value_slider_0), "SLIDER0 : %3u", slider_position[0]);
            else
                snprintf(value_slider_0, sizeof(value_slider_0), "slider0 : %3u", slider_position[0]);

            if (active_timer[1])
                snprintf(value_slider_1, sizeof(value_slider_1), "SLIDER1 : %3u", slider_position[1]);
            else
                snprintf(value_slider_1, sizeof(value_slider_1), "slider1 : %3u", slider_position[1]);
            
            UG_ClearBuffer();
            UG_PutString(10, 34, value_slider_0);
            UG_PutString(10, 18, value_slider_1);
            
            // FIXME - Correct button positions
            for (i = 0; i < 8; i++) {
                int x = (i % 4) * (SCREEN_WIDTH / 4 + 4);
                int y = (i / 4) * (SCREEN_HEIGHT - 10);
                UG_PutString(x, y, value_button[i]);
            }
            UG_Update();
            UG_SendBuffer();
        }
        
        delay_ms(LOOP_DELAY_MS);
    }
}
