/*
 Copyright (c) 2018 Douglas J. Bakkum
 The MIT License (MIT)
*/


#include <string.h>
#include "driver_init.h"
#include "screen.h"
#include "qtouch.h"
#include "usb.h"

#include "demo_list.h"


void HardFault_Handler(void)
{
    screen_print_debug("Hard Fault", NULL, 0);
    while (1) {}
}


int main(void)
{
    system_init();
    screen_init();
    screen_splash("defcon", 300);
    qtouch_init();
    usb_init();

    demo_sd();
    demo_sign();
    demo_detect_touch();
    demo_mini_pong();
    demo_animation();
    demo_hello_world();

    screen_splash("go", 300);

    while (1) {}
}
