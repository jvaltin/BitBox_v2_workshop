/*
 Copyright (c) 2018 Douglas J. Bakkum
 The MIT License (MIT)
*/


#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "driver_init.h"
#include "screen.h"
#include "random.h"
#include "util.h"
#include "sd.h"
#include "demo_list.h"


void demo_sd(void)
{
    char status[64];
    char data[8 + 1];
    uint8_t number[4];
    char filename[16];
    int replace = 1; // replace file if it exists

    screen_print_debug("SD card demo", NULL, 3000);


    while (!sd_card_inserted()) {
        screen_print_debug("Please insert\nan SD card", NULL, 0);
        delay_ms(800);
    }


    random_bytes(number, sizeof(number));
    snprintf(data, sizeof(data), "%s", utils_uint8_to_hex(number, sizeof(number)));
    snprintf(filename, sizeof(filename), "%s.txt", data);
    
    screen_print_debug("Listing SD card\nfiles...", NULL, 3000);
    sd_list();
    
    snprintf(status, sizeof(status), "Generating random\ndata:\n  %s", data);
    screen_print_debug(status, NULL, 3000);
    
    screen_print_debug("Saving the data to\nthe SD card...", NULL, 3000);

    sd_write(filename, data, replace);

    screen_print_debug("Listing SD card\nfiles...", NULL, 3000);
    sd_list();

    screen_print_debug("Saved file contains...", NULL, 3000);
    screen_print_debug(sd_load(filename), NULL, 3000);

    snprintf(status, sizeof(status), "Erasing filename:\n  %s", filename);
    screen_print_debug(status, NULL, 3000);
    
    sd_erase(filename);
    
    screen_print_debug("Listing SD card\nfiles...", NULL, 3000);
    sd_list();
    
    screen_print_debug("SD card demo\nfinished.", NULL, 3000);
    
    while (1) {}
}
