/*
 Copyright (c) 2018 Douglas J. Bakkum
 The MIT License (MIT)
*/


#include <string.h>
#include "util.h"
#include "screen.h"
#include "random.h"
#include "commander.h"

    
static char report[0x1000];


char *commander(const char *command)
{
    // do something
    screen_print_debug(command, NULL, 0);

    // return a report
    snprintf(report, sizeof(report), "{\"status\":\"%s\"}", "success");
    return report;
}
