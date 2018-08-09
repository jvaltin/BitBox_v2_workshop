/*
 Copyright (c) 2018 Jonas Schnelli, Douglas J. Bakkum
 The MIT License (MIT)
*/


#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "driver_init.h"
#include "random.h"
#include "qtouch.h"
#include "screen.h"
#include "ugui.h"
#include "demo_list.h"


extern volatile uint8_t measurement_done_touch;
uint16_t slider_position[DEF_NUM_SCROLLERS];


static void touch_slider_update(uint16_t nr)
{
    if (qtouch_get_scroller_state(nr))
        slider_position[nr] = (255 - qtouch_get_scroller_position(nr));
}


void demo_mini_pong(void)
{
    float ballx = 10;
    float bally = 10;
    float forcex = 1;
    float forcey = 0.2;
    int force_delay = 450;
    uint8_t ball_sticky = 0;
    static const uint8_t HANDLE_WIDTH = 18;

    uint8_t p1x = 0;
    uint8_t p2x = 0;

    int pointsp1 = 0;
    int pointsp2 = 0;
    char msg[64];
    float pointsxf = 5;
    float pointsxforce = 0.2;

    /* select into font */
    UG_FontSelect(&FONT_8X8);

    uint8_t ix = 10;
    const char *intro = "Bitbox\nMini Pong";
    UG_ClearBuffer();
    UG_PutString(ix, 24, intro);
    UG_SendBuffer();
    delay_ms(700);
    for (unsigned int i = ix; i < 200; i++) {
        UG_ClearBuffer();
        UG_PutStringNoBreak((uint8_t)i, 24, intro);
        UG_SendBuffer();
    }

    UG_ClearBuffer();
    UG_SendBuffer();
    delay_ms(200);

    /* select points font */
    UG_FontSelect(&FONT_12X16);

    while (1) {
        qtouch_process();
        if (measurement_done_touch == 1) {
            touch_slider_update(0);
            touch_slider_update(1);
            measurement_done_touch = 0;
        }

        /*dcalculate slider A movment */
        float p1x_f = slider_position[1] / (float)0xFF * SCREEN_WIDTH;
        p1x = SCREEN_WIDTH - (uint8_t)p1x_f;

        float p2x_f = slider_position[0] / (float)0xFF * SCREEN_WIDTH;
        p2x = (uint8_t)p2x_f;

        /* calculate ball movement */
        if (force_delay == 0) {
            ballx += forcex;
            bally += forcey;
        } else {
            snprintf(msg, sizeof(msg), "%d : %d", pointsp1, pointsp2);
            force_delay--;
            if (ball_sticky == 1) {
                ballx = p1x;
            } else if (ball_sticky == 2) {
                ballx = p2x;
            }
        }

        if (ballx > SCREEN_WIDTH - 3) {
            ballx = SCREEN_WIDTH - 3;
            forcex = -forcex;
        }
        if (ballx <= 1) {
            ballx = 1;
            forcex = -forcex;
        }
        if (bally <= 4 && ballx >= p1x - HANDLE_WIDTH / 2 && ballx <= p1x + HANDLE_WIDTH / 2) {
            bally = 4;
            forcey = -forcey;
        }
        if (bally >= SCREEN_HEIGHT - 4 && ballx >= p2x - HANDLE_WIDTH / 2 &&
                ballx <= p2x + HANDLE_WIDTH / 2) {
            bally = SCREEN_HEIGHT - 4;
            forcey = -forcey;
        }
        if (bally <= 0 || bally >= SCREEN_HEIGHT) {
            forcex = (random_uint32() % 10) / 10.0 / 2 + 0.2;
            if (random_uint32() % 2 == 1) {
                forcex = -forcex;
            }
            forcey = (random_uint32() % 10) / 10.0 / 2 + 0.2;
            if (bally <= 4) {
                /* p1 lost */
                pointsp2++;
                bally = 2;
                ball_sticky = 1;
                ballx = p1x;
            } else {
                pointsp1++;
                forcey = -forcey;
                bally = SCREEN_HEIGHT - 7;
                ball_sticky = 2;
                ballx = p2x;
            }
            force_delay = 150;
            pointsxf = 5;
            pointsxforce = 0.2;
        }

        UG_ClearBuffer();

        if (force_delay > 0) {
            UG_PutStringNoBreak((int)pointsxf, 24, msg);
            if (force_delay < 120) {
                pointsxf += pointsxforce;
            }
            pointsxforce *= 1.02;
            if (pointsp1 >= 10 || pointsp1 > 10) {
                /* game finished */
                UG_ClearBuffer();
                UG_PutString(10, 14, "Game\n   over!");
                UG_SendBuffer();
                delay_ms(3000);
                _reset_mcu();
                break;
            }
        }
        /* draw ball */
        UG_DrawPixel(ballx + 1, bally - 1, C_WHITE);
        UG_DrawPixel(ballx - 1, bally + 1, C_WHITE);
        UG_DrawPixel(ballx + 1, bally + 3, C_WHITE);
        UG_DrawPixel(ballx + 3, bally + 1, C_WHITE);
        UG_FillFrame(ballx, bally, ballx + 2, bally + 2, C_WHITE);

        /* draw p1 handle */
        UG_FillFrame(p1x - HANDLE_WIDTH / 2, 1, p1x + HANDLE_WIDTH / 2, 2, C_WHITE);

        /* draw p2 handle */
        UG_FillFrame(p2x - HANDLE_WIDTH / 2, SCREEN_HEIGHT - 3, p2x + HANDLE_WIDTH / 2,
                     SCREEN_HEIGHT - 2, C_WHITE);

        UG_SendBuffer();
    }
}
