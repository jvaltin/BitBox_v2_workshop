/*
 Copyright (c) 2018 Jonas Schnelli
 The MIT License (MIT)
*/


#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "driver_init.h"
#include "qtouch.h"
#include "screen.h"
#include "ugui.h"
#include "demo_list.h"

#include "fonts/font_ab_10X10.h"
#include "fonts/font_ab_13X13.h"
#include "fonts/font_ab_15X17.h"
#include "fonts/font_a_17X18.h"


#define g_null(s) (s >= 0 ? s : 0)


enum DBB_TOPBAR_STYLE {
    DBB_TOPBAR_STYLE_NONE,
    DBB_TOPBAR_STYLE_ABORT,
    DBB_TOPBAR_STYLE_ABORT_RIGHT,
    DBB_TOPBAR_STYLE_ABORT_LEFT,
    DBB_TOPBAR_STYLE_ABORT_RIGHT_LEFT
};

extern volatile uint8_t measurement_done_touch;
uint16_t slider_position[DEF_NUM_SCROLLERS];
uint16_t active_timer[DEF_NUM_SCROLLERS];
uint8_t touch_down[DEF_NUM_SCROLLERS];
int sd_delta[DEF_NUM_SENSORS]; // raw data of sliders
static float topbar_y = 0;


static void touch_slider_update(uint16_t nr)
{
    if (0u != qtouch_get_scroller_state(nr)) {
        slider_position[nr] = qtouch_get_scroller_position(nr);
        active_timer[nr]++;
    } else {
        if (active_timer[nr] >= 1) {
            touch_down[nr] = 1;
        }
        active_timer[nr] = 0;
    }
}


/* events */
static bool under_right_button(void)
{
    if (active_timer[1] >= 1 && slider_position[1] > 200) {
        return true;
    }
    return false;
}


static bool right_top_click(void)
{
    if (touch_down[1] == 1 && slider_position[1] > 200) {
        touch_down[1] = 0;
        return true;
    }
    return false;
}


static bool under_left_button(void)
{
    if (active_timer[1] >= 1 && slider_position[1] < 85) {
        return true;
    }
    return false;
}


static bool left_top_click(void)
{
    if (touch_down[1] == 1 && slider_position[1] < 85) {
        touch_down[1] = 0;
        return true;
    }
    return false;
}


static bool middle_click(void)
{
    if (touch_down[1] == 1 && slider_position[1] > 85 && slider_position[1] < 200) {
        touch_down[1] = 0;
        return true;
    }
    return false;
}


static void ui_draw_arrow_left(uint8_t top_left_x, uint8_t top_left_y)
{
    UG_FontSelect(&font_font_ab_10X10);
    UG_SetForecolor( C_BLACK );
    UG_SetBackcolor( C_WHITE );
    UG_PutStringNoBreak(top_left_x + 1, top_left_y - 11 - 3, "back");
    UG_SetForecolor( C_WHITE);
    UG_SetBackcolor( C_BLACK );
    UG_DrawPixel(top_left_x + 2, top_left_y, C_WHITE);
    UG_DrawPixel(top_left_x + 1, top_left_y + 1, C_WHITE);
    UG_DrawPixel(top_left_x, top_left_y + 2, C_WHITE);
    UG_DrawPixel(top_left_x + 1, top_left_y + 3, C_WHITE);
    UG_DrawPixel(top_left_x + 2, top_left_y + 4, C_WHITE);
}


static void ui_draw_arrow_right(uint8_t top_left_x, uint8_t top_left_y)
{
    UG_FontSelect(&font_font_ab_10X10);
    UG_SetForecolor( C_BLACK );
    UG_SetBackcolor( C_WHITE );
    UG_PutStringNoBreak(top_left_x - 20, top_left_y - 11 - 3, "next");
    UG_SetForecolor( C_WHITE);
    UG_SetBackcolor( C_BLACK );
    UG_DrawPixel(top_left_x, top_left_y, C_WHITE);
    UG_DrawPixel(top_left_x + 1, top_left_y + 1, C_WHITE);
    UG_DrawPixel(top_left_x + 2, top_left_y + 2, C_WHITE);
    UG_DrawPixel(top_left_x + 1, top_left_y + 3, C_WHITE);
    UG_DrawPixel(top_left_x, top_left_y + 4, C_WHITE);
}


static void ui_draw_slider(int16_t top_left_x, int16_t top_left_y, int16_t width, float progress)
{
    if (progress > 1.0) {
        progress = 1.0;
    }
    float pwidth = progress * (width - 1.0);
    UG_DrawLine(top_left_x + 2, top_left_y, top_left_x + width - 2, top_left_y, C_WHITE);
    UG_DrawLine(top_left_x + 2, top_left_y + 8, top_left_x + width - 2, top_left_y + 8,
                C_WHITE);
    UG_DrawLine(top_left_x, top_left_y + 2, top_left_x, top_left_y + 5, C_WHITE);
    UG_DrawLine(top_left_x + width, top_left_y + 2, top_left_x + width, top_left_y + 5,
                C_WHITE);
    UG_DrawPixel(top_left_x + 1, top_left_y + 1, C_WHITE);
    UG_DrawPixel(top_left_x + 1, top_left_y + 7, C_WHITE);
    UG_DrawPixel(top_left_x + width - 1, top_left_y + 1, C_WHITE);
    UG_DrawPixel(top_left_x + width - 1, top_left_y + 7, C_WHITE);

    UG_FillFrame(top_left_x + 1, top_left_y + 1, top_left_x + pwidth, top_left_y + 7,
                 C_WHITE);
}


static void ui_draw_arrow_down(uint8_t top_left_x, uint8_t top_left_y)
{
    UG_DrawPixel(top_left_x + 2, top_left_y, C_WHITE);
    UG_DrawPixel(top_left_x + 2, top_left_y + 1, C_WHITE);
    UG_DrawPixel(top_left_x + 2, top_left_y + 2, C_WHITE);
    UG_DrawPixel(top_left_x + 2, top_left_y + 3, C_WHITE);
    UG_DrawPixel(top_left_x + 2, top_left_y + 4, C_WHITE);
    UG_DrawPixel(top_left_x + 2, top_left_y + 5, C_WHITE);

    UG_DrawPixel(top_left_x, top_left_y + 3, C_WHITE);
    UG_DrawPixel(top_left_x + 1, top_left_y + 4, C_WHITE);

    UG_DrawPixel(top_left_x + 4, top_left_y + 3, C_WHITE);
    UG_DrawPixel(top_left_x + 3, top_left_y + 4, C_WHITE);
}


static void ui_yes_no_button(uint8_t top_left_x, uint8_t top_left_y, int drawhl)
{
    uint8_t width = 90;
    uint8_t height = 24;
    UG_FillFrame(top_left_x, top_left_y, top_left_x + width, top_left_y + 1, C_WHITE);
    UG_FillFrame(top_left_x, top_left_y, top_left_x + 1, top_left_y + height, C_WHITE);
    UG_FillFrame(top_left_x + width - 1, top_left_y, top_left_x + width, top_left_y + height,
                 C_WHITE);
    UG_FillFrame(top_left_x, top_left_y + height - 1, top_left_x + width, top_left_y + height,
                 C_WHITE);

    UG_FillFrame(top_left_x + (width / 2), top_left_y, top_left_x + (width / 2) + 1,
                 top_left_y + height, C_WHITE);

    UG_DrawPixel(top_left_x, top_left_y, C_BLACK);
    UG_DrawPixel(top_left_x, top_left_y + 1, C_BLACK);
    UG_DrawPixel(top_left_x + 1, top_left_y, C_BLACK);

    UG_DrawPixel(top_left_x + width, top_left_y, C_BLACK);
    UG_DrawPixel(top_left_x + width - 1, top_left_y, C_BLACK);
    UG_DrawPixel(top_left_x + width, top_left_y + 1, C_BLACK);

    UG_DrawPixel(top_left_x + width, top_left_y + height, C_BLACK);
    UG_DrawPixel(top_left_x + width - 1, top_left_y + height, C_BLACK);
    UG_DrawPixel(top_left_x + width, top_left_y + height - 1, C_BLACK);

    UG_DrawPixel(top_left_x, top_left_y + height, C_BLACK);
    UG_DrawPixel(top_left_x, top_left_y + height - 1, C_BLACK);
    UG_DrawPixel(top_left_x + 1, top_left_y + height, C_BLACK);

    if (drawhl == 1) {
        UG_SetForecolor( C_BLACK );
        UG_SetBackcolor( C_WHITE );
        UG_FillFrame(top_left_x + 2, top_left_y + 2, top_left_x + (width / 2),
                     top_left_y + height - 2, C_WHITE);
    }
    UG_PutStringNoBreak(30, 38, "YES");
    UG_SetForecolor( C_WHITE);
    UG_SetBackcolor( C_BLACK );
    if (drawhl == 2) {
        UG_SetForecolor( C_BLACK );
        UG_SetBackcolor( C_WHITE );
        UG_FillFrame(top_left_x + (width / 2), top_left_y + 2, top_left_x + width,
                     top_left_y + height - 2, C_WHITE);
    }
    UG_PutStringNoBreak(80, 38, "NO");
    UG_SetForecolor( C_WHITE);
    UG_SetBackcolor( C_BLACK );
}


static void ui_draw_center_button(int y)
{
    if (y > 0) {
        UG_FillFrame(0, 0, SCREEN_WIDTH - 1, y - 1, C_WHITE);
        UG_DrawLine(2, y - 1, SCREEN_WIDTH - 3, y - 1, C_WHITE);
        UG_DrawLine(0, 0, 0, g_null(y - 3), C_WHITE);
        UG_DrawPixel(0, y - 1, C_BLACK);
        UG_DrawPixel(SCREEN_WIDTH - 1, y - 1, C_BLACK);
    }
    UG_DrawLine(51, y + 0, 51 + 27, y + 0, C_WHITE);
    UG_DrawLine(52, y + 1, 52 + 25, y + 1, C_WHITE);
    UG_DrawLine(52, y + 2, 52 + 25, y + 2, C_WHITE);
    UG_DrawLine(53, y + 3, 53 + 23, y + 3, C_WHITE);
    UG_DrawLine(53, y + 4, 53 + 23, y + 4, C_WHITE);
    UG_DrawLine(54, y + 5, 54 + 21, y + 5, C_WHITE);
    UG_DrawLine(54, y + 6, 54 + 21, y + 6, C_WHITE);
    UG_DrawLine(55, y + 7, 55 + 19, y + 7, C_WHITE);
    UG_DrawLine(55, y + 8, 55 + 19, y + 8, C_WHITE);
    UG_DrawLine(56, y + 9, 56 + 17, y + 9, C_WHITE);

    UG_FontSelect(&font_font_ab_10X10);
    UG_SetForecolor( C_BLACK );
    UG_SetBackcolor( C_WHITE );
    UG_PutStringNoBreak(53, y - 10, "abort");

    UG_PutStringNoBreak(63, y + 0, "x");
    UG_SetForecolor( C_WHITE );
    UG_SetBackcolor( C_BLACK);
}


static void build_up_topbar_(int y, enum DBB_TOPBAR_STYLE style)
{
    if (style == DBB_TOPBAR_STYLE_NONE) {
        return;
    }
    ui_draw_center_button(y);
    if (style == DBB_TOPBAR_STYLE_ABORT_LEFT || style == DBB_TOPBAR_STYLE_ABORT_RIGHT_LEFT) {
        ui_draw_arrow_left(3, 3 + y);
    }
    if (style == DBB_TOPBAR_STYLE_ABORT_RIGHT || style == DBB_TOPBAR_STYLE_ABORT_RIGHT_LEFT) {
        ui_draw_arrow_right(122, 3 + y);
    }
    UG_FontSelect(&font_font_ab_13X13);
}


// shows slider1 pos and time top right
static void debug_show_timer(void)
{
    static char msg[128] = {0};
    UG_FontSelect(&FONT_5X8);
    snprintf(msg, sizeof(msg), "%3u %3u", sd_delta[0], sd_delta[1]);
    UG_FillFrame(80, 1, 110, 9, C_BLACK);
    UG_PutStringNoBreak(80, 1, msg);
}


void demo_sign(void)
{
    UG_FontSetHSpace(0); // set h space to 0

    topbar_y = 12.0;
    //bool long_delay = true;
    int page = 0;
    float arrow_y = 0.0;
    float arrow_speed = 0.1;
    float sliderprogress = 0.0;
    int barstyle = DBB_TOPBAR_STYLE_ABORT_RIGHT;
    bool _debug = false;


    int alert = 0;
    while (1) {
        qtouch_process();
        if (measurement_done_touch == 1) {
            touch_slider_update(0);
            touch_slider_update(1);
            measurement_done_touch = 0;
        }

        for (unsigned int i = 0; i < DEF_NUM_SENSORS; i++) {
            sd_delta[i] = qtouch_get_sensor_node_signal(i) - qtouch_get_sensor_node_reference(i);
        }

        UG_ClearBuffer();

        bool left_click = left_top_click();
        bool right_click = right_top_click();

        if (middle_click() && !alert) {
            alert = 1;
            //_reset_mcu();
        }
        if (page == 0) {
            barstyle = DBB_TOPBAR_STYLE_ABORT_RIGHT;
            if (right_click && !alert) {
                page = 1;
                continue;
            }
            UG_FontSelect(&font_font_ab_15X17);
            UG_PutStringNoBreakCenter(0, 21, 0, "Confirm");
            UG_PutStringNoBreakCenter(0, 40, 0, "Transaction");
        } else if (page == 1) {
            barstyle = DBB_TOPBAR_STYLE_ABORT_RIGHT_LEFT;
            if (left_click && !alert) {
                page = 0;
                continue;
            } else if (right_click && !alert) {
                page = 2;
                continue;
            } else {
                if (arrow_y <= 27.0) {
                    arrow_y = 27.0;
                    arrow_speed = 0.2;
                }
                ui_draw_arrow_down(62, (int)arrow_y);
                if (arrow_y >= 32.0) {
                    arrow_y = 32.0;
                    arrow_speed = -0.2;
                }
                arrow_y += arrow_speed;
                UG_FontSelect(&font_font_ab_13X13);
                UG_PutStringNoBreakCenter(0, 16, 0, "1230.1234567 BTC");
                UG_FontSelect(&font_font_a_17X18);
                float p1x_f = 255.0 - slider_position[0];
                UG_PutStringNoBreak(-p1x_f, 38, "    2MyAsbKVY2pqTV528bv4jSdtr7C3JybwLct");
            }
        } else if (page == 2) {
            barstyle = DBB_TOPBAR_STYLE_ABORT_RIGHT_LEFT;
            if (left_click && !alert) {
                page = 1;
                continue;
            } else if (right_click && !alert) {
                page = 3;
                continue;
            }
            UG_FontSelect(&font_font_ab_13X13);
            UG_PutStringNoBreakCenter(0, 16, 0, "Fee 0.00012345 BTC");
        } else if (page == 3) {
            barstyle = DBB_TOPBAR_STYLE_ABORT_LEFT;
            if (left_click && !alert) {
                page = 2;
                continue;
            }
            topbar_y = 0;
            if (sliderprogress >= 1.0 && !alert) {
                barstyle = DBB_TOPBAR_STYLE_NONE;
                // confirmed
                UG_FontSelect(&font_font_ab_13X13);
                UG_PutStringNoBreakCenter(0, 20, 0, "Signing Transaction");
                ui_draw_slider(14, 44, 100, sliderprogress);
                UG_SendBuffer();
                delay_ms(2000);
                _reset_mcu();
            } else {
                if (under_right_button()) {
                    sliderprogress = 1.0 / 100.0 * active_timer[1];
                }
                UG_FontSelect(&font_font_ab_13X13);
                UG_PutStringNoBreakCenter(0, 13, 0, "Accept transaction?");
                UG_PutStringNoBreakCenter(0, 27, 0, "Touch right for 3s");
                ui_draw_slider(14, 44, 100, sliderprogress);
            }

        }

        build_up_topbar_((int)topbar_y, barstyle);

        if (alert == 1) {
            if (left_click) {
                _reset_mcu();
                continue;
            }
            if (right_click) {
                alert = 0;
                continue;
            }

            UG_ClearBuffer();
            UG_FontSelect(&font_font_ab_13X13);
            UG_PutStringNoBreakCenter(0, 5, 0, "Abort?");

            int button = 0;
            if (under_left_button()) {
                button = 1;
            }
            if (under_right_button()) {
                button = 2;
            }

            ui_yes_no_button(19, 31, button);
        }

        if (_debug) {
            debug_show_timer();
        }
        UG_SendBuffer();

        //delay_ms( long_delay ? 1000 : 1);
        //long_delay = false;
        if (active_timer[1] > 5) {
            topbar_y += 1;
            if (topbar_y >= 12.0) {
                topbar_y = 11.9;
            }
        } else {
            topbar_y *= 0.95;
        }
    }

    UG_PutStringNoBreakCenter(0, 12, 0, "Confirm with swipe");
    UG_PutStringNoBreakCenter(0, 26, 0, "Touch briefly to abort");


    UG_ClearBuffer();
    UG_FontSelect(&font_font_ab_15X17);
    UG_FontSetHSpace(0);
    UG_PutStringNoBreakCenter(0, 12, 0, "Confirm");
    UG_PutStringNoBreakCenter(0, 26, 0, "Transaction");
    UG_SendBuffer();
    delay_ms(5000);
}
