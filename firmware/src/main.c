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

    demo_crypto();
    demo_sd();
    demo_mini_pong();
    demo_detect_touch();
    demo_hello_world();
    demo_animation();
    demo_sign();


    screen_splash("go", 300);

    while (1) {}
}
