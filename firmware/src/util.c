/*

 The MIT License (MIT)

 Copyright (c) 2015-2018 Douglas J. Bakkum

 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the "Software"),
 to deal in the Software without restriction, including without limitation
 the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
 OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*/


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include "util.h"


static uint8_t utils_buffer[UTILS_BUFFER_LEN];


static void utils_clear_buffers(void)
{
    memset(utils_buffer, 0, UTILS_BUFFER_LEN);
}


uint8_t *utils_hex_to_uint8(const char *str)
{
    if (strlens(str) > UTILS_BUFFER_LEN) {
        return NULL;
    }
    utils_clear_buffers();
    uint8_t c;
    size_t i;
    for (i = 0; i < strlens(str) / 2; i++) {
        c = 0;
        if (str[i * 2] >= '0' && str[i * 2] <= '9') {
            c += (str[i * 2] - '0') << 4;
        }
        if (str[i * 2] >= 'a' && str[i * 2] <= 'f') {
            c += (10 + str[i  * 2] - 'a') << 4;
        }
        if (str[i * 2] >= 'A' && str[i * 2] <= 'F') {
            c += (10 + str[i * 2] - 'A') << 4;
        }
        if (str[i * 2 + 1] >= '0' && str[i * 2 + 1] <= '9') {
            c += (str[i * 2 + 1] - '0');
        }
        if (str[i * 2 + 1] >= 'a' && str[i * 2 + 1] <= 'f') {
            c += (10 + str[i * 2 + 1] - 'a');
        }
        if (str[i * 2 + 1] >= 'A' && str[i * 2 + 1] <= 'F') {
            c += (10 + str[i * 2 + 1] - 'A');
        }
        utils_buffer[i] = c;
    }
    return utils_buffer;
}


char *utils_uint8_to_hex(const uint8_t *bin, size_t l)
{
    if (l > (UTILS_BUFFER_LEN / 2 - 1)) {
        return NULL;
    }
    static char digits[] = "0123456789abcdef";
    utils_clear_buffers();
    size_t i;
    for (i = 0; i < l; i++) {
        utils_buffer[i * 2] = digits[(bin[i] >> 4) & 0xF];
        utils_buffer[i * 2 + 1] = digits[bin[i] & 0xF];
    }
    utils_buffer[l * 2] = '\0';
    return (char *)utils_buffer;
}
