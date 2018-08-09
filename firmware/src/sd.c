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


#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include "driver_init.h"
#include "sd.h"
#include "sd_mmc.h"
#include "commander.h"
#include "screen.h"
#include "util.h"
#include "ff.h"

#define f_printf3(a, b) f_printf(a, b)
#define FO(a)           &a

static sd_mmc_detect_t SDMMC_ACCESS_0_cd[CONF_SD_MMC_MEM_CNT] = {
    {-1, CONF_SD_MMC_0_CD_DETECT_VALUE},
};
static sd_mmc_detect_t SDMMC_ACCESS_0_wp[CONF_SD_MMC_MEM_CNT] = {
    {-1, CONF_SD_MMC_0_WP_DETECT_VALUE},
};

uint32_t sd_update = 0;
uint32_t sd_fs_found = 0;
uint32_t sd_listing_pos = 0;
uint32_t sd_num_files = 0;
static char ROOTDIR[] = "0:/bitbox";
FATFS fs;


uint8_t sd_write(const char *fn, const char *text, uint8_t replace)
{
    char file[256];

    if (!strlens(text)) {
        screen_print_debug("ERR_SD_TEXT", NULL, 1000);
        return 1;
    }

    memset(file, 0, sizeof(file));
    snprintf(file, sizeof(file), "%s/%s", ROOTDIR, fn);

    sd_mmc_init(&MCI_0, SDMMC_ACCESS_0_cd, SDMMC_ACCESS_0_wp);
    sd_listing_pos = 0;

    sd_mmc_err_t err = sd_mmc_check(0);
    if (err != SD_MMC_OK && err != SD_MMC_INIT_ONGOING) {
        screen_print_debug("ERR_SD_CARD", NULL, 1000);
        goto err;
    }

    FRESULT res;
    FIL file_object;

    memset(&fs, 0, sizeof(FATFS));
    res = f_mount(&fs, "SD", 1);
    if (FR_INVALID_DRIVE == res) {
        screen_print_debug("ERR_SD_MOUNT", NULL, 1000);
        goto err;
    }

    f_mkdir(ROOTDIR);

    res = f_open(FO(file_object), (char const *)file,
                 (replace == 1 ? FA_CREATE_ALWAYS : FA_CREATE_NEW) | FA_WRITE);
    if (res != FR_OK) {
        screen_print_debug("ERR_SD_OPEN_FILE", NULL, 1000);
        f_mount(NULL, "SD", 1);
        goto err;
    }

    f_printf(FO(file_object), text);
    
    f_close(FO(file_object));
    f_mount(NULL, "SD", 1);
    return 0;
err:
    return 1;
}


char *sd_load(const char *fn)
{
    char file[256];
    static char text[512];

    memset(file, 0, sizeof(file));
    memset(text, 0, sizeof(text));

    snprintf(file, sizeof(file), "%s/%s", ROOTDIR, fn);

    sd_mmc_init(&MCI_0, SDMMC_ACCESS_0_cd, SDMMC_ACCESS_0_wp);
    sd_listing_pos = 0;

    sd_mmc_err_t err = sd_mmc_check(0);
    if (err != SD_MMC_OK && err != SD_MMC_INIT_ONGOING) {
        screen_print_debug("ERR_SD_CARD", NULL, 1000);
        goto err;
    }

    FRESULT res;
    FIL file_object;

    memset(&fs, 0, sizeof(FATFS));
    res = f_mount(&fs, "SD", 1);
    if (FR_INVALID_DRIVE == res) {
        screen_print_debug("ERR_SD_MOUNT", NULL, 1000);
        goto err;
    }

    res = f_open(FO(file_object), (char const *)file, FA_OPEN_EXISTING | FA_READ);
    if (res != FR_OK) {
        screen_print_debug("ERR_SD_OPEN_FILE", NULL, 1000);
        f_mount(NULL, "SD", 1);
        goto err;
    }
        
    char line[0x1000];
    char *text_p = text;
    unsigned content_found = 0, text_p_index = 0;
    while (f_gets(line, sizeof(line), FO(file_object))) {
        snprintf(text_p + text_p_index, sizeof(text) - text_p_index, "%s", line);
        text_p_index += strlens(line);
        text[text_p_index] = '\0';
    }

    f_close(FO(file_object));
    f_mount(NULL, "SD", 1);
    return text;
err:
    return NULL;
}


uint8_t sd_list(void)
{
    char files[0x1000] = {0};
    size_t f_len = 0;
    uint32_t pos = 1;

    FILINFO fno;
    DIR dir;
#ifdef _USE_LFN
    char c_lfn[_MAX_LFN + 1];
    fno.lfname = c_lfn;
    fno.lfsize = sizeof(c_lfn);
#endif

    sd_mmc_init(&MCI_0, SDMMC_ACCESS_0_cd, SDMMC_ACCESS_0_wp);
    sd_listing_pos = 0;

    sd_mmc_err_t err = sd_mmc_check(0);
    if (err != SD_MMC_OK && err != SD_MMC_INIT_ONGOING) {
        screen_print_debug("ERR_SD_CARD", NULL, 1000);
        return 1;
    }

    FRESULT res;
    memset(&fs, 0, sizeof(FATFS));
    res = f_mount(&fs, "SD", 1);
    if (FR_INVALID_DRIVE == res) {
        screen_print_debug("ERR_SD_MOUNT", NULL, 1000);
        return 1;
    }

    res = f_opendir(&dir, ROOTDIR);
    if (res == FR_OK) {
        strcat(files, "[");
        f_len++;
        for (;;) {
            char *pc_fn;
            res = f_readdir(&dir, &fno);
            if (res != FR_OK || fno.fname[0] == 0) {
                break;
            }
#ifdef _USE_LFN
            pc_fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            pc_fn = fno.fname;
#endif
            if (*pc_fn == '.' && *(pc_fn + 1) == '\0') {
                continue;
            }
            if (*pc_fn == '.' && *(pc_fn + 1) == '.' && *(pc_fn + 2) == '\0') {
                continue;
            }

            f_len += strlen(pc_fn) + strlens(",\"\"");
            if (f_len + 1 >= sizeof(files)) {
                f_mount(NULL, "SD", 1);
                screen_print_debug("WARNING_SD_NUM_FILES", NULL, 1000);
                strcat(files, "\"");
                break;
            }

            if (pos >= sd_listing_pos) {
                if (strlens(files) > 1) {
                    strcat(files, ",");
                }
                snprintf(files + strlens(files), sizeof(files) - f_len, "\"%s\"", pc_fn);
            }
            pos += 1;
        }
        strcat(files, "]");
    } else {
        screen_print_debug("ERR_SD_OPEN_DIR", NULL, 1000);
    }

    screen_print_debug(files, NULL, 1000);
    f_mount(NULL, "SD", 1);
    return 0;
}


uint8_t sd_card_inserted(void)
{
    sd_mmc_init(&MCI_0, SDMMC_ACCESS_0_cd, SDMMC_ACCESS_0_wp);
    sd_listing_pos = 0;

    sd_mmc_err_t err = sd_mmc_check(0);
    if (err != SD_MMC_OK && err != SD_MMC_INIT_ONGOING) {
        return 0;
    }
    return 1;
}


uint8_t sd_file_exists(const char *fn)
{
    char file[256];
    memset(file, 0, sizeof(file));
    snprintf(file, sizeof(file), "%s/%s", ROOTDIR, fn);

    FIL file_object;
    FRESULT res;

    memset(file, 0, sizeof(file));
    snprintf(file, sizeof(file), "%s/%s", ROOTDIR, fn);

    sd_mmc_init(&MCI_0, SDMMC_ACCESS_0_cd, SDMMC_ACCESS_0_wp);
    sd_listing_pos = 0;

    sd_mmc_err_t err = sd_mmc_check(0);
    if (err != SD_MMC_OK && err != SD_MMC_INIT_ONGOING) {
        return 1;
    }

    memset(&fs, 0, sizeof(FATFS));
    res = f_mount(&fs, "SD", 1);
    if (FR_INVALID_DRIVE == res) {
        return 1;
    }

    res = f_open(FO(file_object), (char const *)file, FA_OPEN_EXISTING | FA_READ);
    if (res == FR_OK) {
        f_close(FO(file_object));
        f_mount(NULL, "SD", 1);
        return 0;
    }
    f_mount(NULL, "SD", 1);
    return 1;
}


static uint8_t sd_delete_files(char *path)
{
    int failed = 0;
    FRESULT res;
    FILINFO fno;
    DIR dir;
#ifdef _USE_LFN
    static char lfn[_MAX_LFN + 1];
    fno.lfname = lfn;
    fno.lfsize = sizeof lfn;
#endif

    res = f_opendir(&dir, path);
    if (res == FR_OK) {
        for (;;) {

            char *pc_fn;
            res = f_readdir(&dir, &fno);
            if (res != FR_OK) {
                failed++;
                break;
            }

            if (fno.fname[0] == 0) { // no more files or directories
                break;
            }

#ifdef _USE_LFN
            pc_fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            pc_fn = fno.fname;
            if (*pc_fn == '.' && *(pc_fn + 1) == '\0') {
                continue;
            }
            if (*pc_fn == '.' && *(pc_fn + 1) == '.' && *(pc_fn + 2) == '\0') {
                continue;
            }

            char file[1024];
            snprintf(file, sizeof(file), "%s/%s", path, pc_fn);

            if (fno.fattrib & AM_DIR) { // is a directory
                failed += sd_delete_files(file);
            } else { // is a file
                FIL file_object;
                res = f_open(FO(file_object), (char const *)file, FA_OPEN_EXISTING | FA_WRITE);
                if (res != FR_OK) {
                    failed++;
                } else {
                    DWORD f_ps, fsize;
                    fsize = file_object.obj.objsize < ULONG_MAX ? file_object.obj.objsize : ULONG_MAX;
                    for (f_ps = 0; f_ps < fsize; f_ps++) {
                        f_putc(0xAC, FO(file_object)); // overwrite data
                    }
                    if (f_close(FO(file_object)) != FR_OK) {
                        failed++;
                    }
                }
            }
            if (f_unlink(file + 2) != FR_OK) {
                failed++;
            }
        }
    }
    return failed;
#endif
}

static uint8_t sd_delete_file(const char *fn)
{
    char file[256];
    memset(file, 0, sizeof(file));
    snprintf(file, sizeof(file), "%s/%s", ROOTDIR, fn);
    int failed = 0;
    FRESULT res;
    FIL file_object;

    res = f_open(FO(file_object), (char const *)file, FA_OPEN_EXISTING | FA_WRITE);
    if (res != FR_OK) {
        failed++;
    } else {
        DWORD f_ps, fsize;
        fsize = file_object.obj.objsize < ULONG_MAX ? file_object.obj.objsize : ULONG_MAX;
        for (f_ps = 0; f_ps < fsize; f_ps++) {
            f_putc(0xAC, FO(file_object)); // overwrite data
        }
        if (f_close(FO(file_object)) != FR_OK) {
            failed++;
        }
    }

    if (f_unlink(file + 2) != FR_OK) {
        failed++;
    }

    return failed;
}


uint8_t sd_erase(const char *fn)
{
    int failed = 0;
    char *path = ROOTDIR;

    sd_mmc_init(&MCI_0, SDMMC_ACCESS_0_cd, SDMMC_ACCESS_0_wp);
    sd_listing_pos = 0;

    sd_mmc_err_t err = sd_mmc_check(0);
    if (err != SD_MMC_OK && err != SD_MMC_INIT_ONGOING) {
        screen_print_debug("ERR_SD_CARD", NULL, 1000);
        return 1;
    }

    FRESULT res;
    memset(&fs, 0, sizeof(FATFS));
    res = f_mount(&fs, "SD", 1);
    if (FR_INVALID_DRIVE == res) {
        screen_print_debug("ERR_SD_MOUNT", NULL, 1000);
        return 1;
    }
    if (strlens(fn)) {
        failed = sd_delete_file(fn);
    } else {
        failed = sd_delete_files(path);
    }

    f_mount(NULL, "SD", 1);

    if (failed) {
        screen_print_debug("ERR_SD_ERASE", NULL, 1000);
        return 1;
    } else {
        screen_print_debug("SUCCESS", NULL, 1000);
        return 0;
    }
}
