/*
 Copyright (c) 2018 Douglas Bakkum
 The MIT License (MIT)
*/


#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "driver_init.h"
#include "screen.h"
#include "ugui.h"
#include "util.h"
#include "demo_list.h"

    
//struct rand_sync_desc RAND_0;

COMPILER_ALIGNED(128)
struct sha_context context;
COMPILER_PACK_RESET();


void demo_crypto(void)
{
	int32_t i;
    char status[256];
    
    screen_print_debug("Hardware crypto\ndemo", NULL, 3000);

    //
    // Hardware random number
    //
    uint8_t number[16];
	
    for (i = 0; i < 8; i++) {
        rand_sync_read_buf8(&RAND_0, number, sizeof(number));
        snprintf(status, sizeof(status),
                "Generating random\nnumbers:\n%s",
                utils_uint8_to_hex(number, sizeof(number))
                );
        screen_print_debug(status, NULL, 1000);
    }


    //
    // Hardware AES encryption
    // See drivers/hal/include/hal_aes_sync.h for the API
    // Input lengths must be a multiple of 16 bytes
    //
    static uint8_t aes_plain_text[32]  = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
    };
    static uint8_t aes_key[32] = {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
        0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60,
        0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97
    };
    static uint8_t aes_cipher_text[32];
    uint8_t aes_decrypt_text[32] = {0x00};
    char aes_decrypt_hex[32 * 2 + 1];
    char aes_cipher_hex[32 * 2 + 1];
    
    // In practice, the initial vector (IV) should be random
    uint8_t iv[16];
    memset(iv, 0, sizeof(iv));

    aes_sync_enable(&CRYPTOGRAPHY_0);
    aes_sync_set_encrypt_key(&CRYPTOGRAPHY_0, aes_key, AES_KEY_256);
    aes_sync_cbc_crypt(&CRYPTOGRAPHY_0, AES_ENCRYPT, aes_plain_text, aes_cipher_text, sizeof(aes_plain_text), iv);
    
    // The IV gets updated, so must reset to initial value
    memset(iv, 0, sizeof(iv));
    aes_sync_cbc_crypt(&CRYPTOGRAPHY_0, AES_DECRYPT, aes_cipher_text, aes_decrypt_text, sizeof(aes_cipher_text), iv);
    
    snprintf(aes_cipher_hex, sizeof(aes_cipher_hex), "%s", utils_uint8_to_hex(aes_cipher_text, sizeof(aes_cipher_text)));
    snprintf(aes_decrypt_hex, sizeof(aes_decrypt_hex), "%s", utils_uint8_to_hex(aes_decrypt_text, sizeof(aes_decrypt_text)));
    snprintf(status, sizeof(status), "AES256CBC\ncipher  %.*s..\ndecrypt %.*s..", 22, aes_cipher_hex, 22, aes_decrypt_hex);
    screen_print_debug(status, NULL, 6000);
    

    //
    // Hardware SHA hash
    // See drivers/hal/include/hal_sha_sync.h for the API
    //
    uint8_t sha_digest[32];

    sha_sync_enable(&HASH_ALGORITHM_0);
    sha_sync_sha256_compute(&HASH_ALGORITHM_0, &context, 0, "abc", 3, sha_digest);
    
    snprintf(status, sizeof(status), "SHA256 hash:\n%s", utils_uint8_to_hex(sha_digest, sizeof(sha_digest)));
    screen_print_debug(status, NULL, 6000);
   

    screen_print_debug("Hardware crypto\ndemo finished.", NULL, 3000);
    
    while (1) {}
}
