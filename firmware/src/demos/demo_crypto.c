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
    
    screen_print_debug("Hardware crypto demo", NULL, 3000);

    //
    // Hardware random number
    //
    uint8_t number[16];
	
    for (i = 0; i < 8; i++) {
        rand_sync_read_buf8(&RAND_0, number, sizeof(number));
        snprintf(status, sizeof(status),
                "Generating random\nnumber:\n  %s\n  %s",
                utils_uint8_to_hex(number, sizeof(number)),
                utils_uint8_to_hex(number, sizeof(number)) + 8
                );
        screen_print_debug(status, NULL, 1000);
    }


    //
    // Hardware AES encryption
    // See hal_aes_sync.h for the API
    //
    static uint8_t aes_plain_text[16]  = {
        0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 
        0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a
    };
    static uint8_t aes_key[32] = {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
        0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60,
        0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97
    };
    static uint8_t aes_cipher_text[16] = {
        0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60,
        0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97
    };
    uint8_t aes_output[16] = {0x00};
    char aes_output_hex[16 * 2 + 1];
    char aes_cipher_hex[16 * 2 + 1];
    
    uint8_t iv[16];
    memset(iv, 0, sizeof(iv));

	aes_sync_enable(&CRYPTOGRAPHY_0);
	aes_sync_set_encrypt_key(&CRYPTOGRAPHY_0, aes_key, AES_KEY_256);
    aes_sync_cbc_crypt(&CRYPTOGRAPHY_0, AES_ENCRYPT, aes_plain_text, aes_output, sizeof(aes_plain_text), iv);
    
    snprintf(aes_cipher_hex, sizeof(aes_cipher_hex), "%s", utils_uint8_to_hex(aes_cipher_text, sizeof(aes_cipher_text)));
    snprintf(aes_output_hex, sizeof(aes_output_hex), "%s", utils_uint8_to_hex(aes_output, sizeof(aes_output)));

    // Compare the output with the expected cipher text
    snprintf(status, sizeof(status), "AES256CBC encryption\n cipher  %s\n output %s", aes_cipher_hex, aes_output_hex);
    screen_print_debug(status, NULL, 3000);


    //
    // Hardware SHA hash
    // See hal_sha_sync.h for the API
    //
	const uint8_t sha_digest[] = {
        0xA9, 0x99, 0x3E, 0x36, 0x47, 0x06, 0x81, 0x6A,
        0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60,
        0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97,
        0x25, 0x71, 0x78, 0x50, 0xC2, 0x6C, 0x9C, 0xD0
    };
	
    uint8_t sha_output[32] = {0x00};
    char sha_output_hex[32 * 2 + 1];
    char sha_digest_hex[32 * 2 + 1];

	sha_sync_enable(&HASH_ALGORITHM_0);
    sha_sync_sha256_compute(&HASH_ALGORITHM_0, &context, false, "abc", 3, sha_output);
    
    snprintf(sha_digest_hex, sizeof(sha_digest_hex), "%s", utils_uint8_to_hex(sha_digest, sizeof(sha_digest)));
    snprintf(sha_output_hex, sizeof(sha_output_hex), "%s", utils_uint8_to_hex(sha_output, sizeof(sha_output)));
        
    // Compare the output with the expected digest
    snprintf(status, sizeof(status), "SHA256 hash\n digest %s\n output %s", sha_digest_hex, sha_output_hex);
    screen_print_debug(status, NULL, 3000);
}
