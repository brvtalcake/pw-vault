#include <picoutil.h>

#include "../aes_test/aes.h"

#include <pico.h>

#include <pico/stdio.h>
#include <pico/stdlib.h>
#include <pico/sync.h>
#include <pico/bootrom.h>
#include <pico/double.h>


#include <stdint.h>
#include <stddef.h>
#include <stdalign.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h> // memcpy, memset
#include <math.h>
#include <stdio.h>
#include <inttypes.h>

#if 0
void demo_allocator(void)
{
    char* str1 = picoutil_static_alloc(10);
    if (str1 == NULL)
    {
        printf("Failed to allocate memory\n");
        return;
    }
    strcpy(str1, "Hello");
    printf("%s\n", str1);
    picoutil_static_allocator_dump_hdrs();

    char* str2 = picoutil_static_alloc(10);
    if (str2 == NULL)
    {
        printf("Failed to allocate memory\n");
        picoutil_static_free(str1);
        return;
    }
    strcpy(str2, "World");
    printf("%s\n", str2);
    char* str3 = picoutil_static_alloc(10);
    if (str3 == NULL)
    {
        printf("Failed to allocate memory\n");
        picoutil_static_free(str1);
        picoutil_static_free(str2);
        return;
    }
    strcpy(str3, "!");
    printf("%s\n", str3);
    picoutil_static_allocator_dump_hdrs();

    picoutil_static_free(str1);
    picoutil_static_free(str2);
    picoutil_static_free(str3);
    picoutil_static_allocator_dump_hdrs();
    printf("\n");
}

static void aes_mul_column_by_poly(byte_t bytes[sizeof(aes_word_t)] /* r */)
{
    __unused static const byte_t poly[sizeof(aes_word_t)] = { 0x02, 0x01, 0x01, 0x03 };
    byte_t bytes_cpy[sizeof(aes_word_t)] = { 0 }; // a
    byte_t b[sizeof(aes_word_t)] = { 0 }; // b
    uint8_t i = 0; // c
    byte_t highest_bit = 0; // h
    for (; i < sizeof(aes_word_t); ++i)
    {
        bytes_cpy[i] = bytes[i];
        highest_bit = (bytes[i] >> 7) & 1;
        b[i] = bytes[i] << 1;
        b[i] ^= (highest_bit * 0x1b);
    }
    bytes[0] = b[0] ^ bytes_cpy[3] ^ bytes_cpy[2] ^ b[1] ^ bytes_cpy[1];
    bytes[1] = b[1] ^ bytes_cpy[0] ^ bytes_cpy[3] ^ b[2] ^ bytes_cpy[2];
    bytes[2] = b[2] ^ bytes_cpy[1] ^ bytes_cpy[0] ^ b[3] ^ bytes_cpy[3];
    bytes[3] = b[3] ^ bytes_cpy[2] ^ bytes_cpy[1] ^ b[0] ^ bytes_cpy[0];
}
#endif

int main(void)
{
    stdio_init_all();
    picoutil_static_allocator_init(true);
    uint32_t i = 1;
    uint32_t j = UINT32_C(123456789);
    uint32_t counter = 0;
#if 0
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);
#endif
    while (true)
    {
        char c = getchar();
        /* picoutil_static_allocator_memdump((uintptr_t[]){ 0, 24 }); */
        /* picoutil_static_allocator_dump_hdrs(); */
        if (c == 'q')
            goto reset;
        /* demo_allocator(); */
        i = 1 << MODu32(counter, 32);
        printf("%" PRIu32 " == %" PRIu32 "\n", picoutil_rotate_left_32(i, 1), ROTATE_RAW_LEFT(i, 1));
        printf("%" PRIu32 " == %" PRIu32 "\n\n", picoutil_rotate_right_32(i, 1), ROTATE_RAW_RIGHT(i, 1));

        printf("%" PRIu32 " with digits rotated to the left by %" PRIu32 " == %" PRIu32 "\n", j, MODu32(counter, DIGIT_COUNT(j)), ROL_DIGITS(j, MODu32(counter, DIGIT_COUNT(j))));
        printf("%" PRIu32 " with digits rotated to the right by %" PRIu32 " == %" PRIu32 "\n\n", j, MODu32(counter, DIGIT_COUNT(j)), ROR_DIGITS(j, MODu32(counter, DIGIT_COUNT(j))));
        ++counter;
        int arr[] = { 1, 2, 3, 4, 5 };
        for (size_t i = 0; i < count_of(arr); ++i)
            printf("%d ", arr[i]);
        picoutil_memset_explicit(arr, 1, sizeof(arr));
        printf("\n");
        for (size_t i = 0; i < count_of(arr); ++i)
            printf("%d ", arr[i]);
        printf("\n");
        picoutil_memset_explicit(arr, 0, sizeof(arr));
        for (size_t i = 0; i < count_of(arr); ++i)
            printf("%d ", arr[i]);
        printf("\n");

#if 0
        byte_t test_vec1[] = { 219, 19, 83, 69 };
        byte_t test_vec2[] = { 242, 10, 34, 92 };
        byte_t test_vec3[] = { 1, 1, 1, 1 };
        byte_t test_vec4[] = { 198, 198, 198, 198 };

        aes_mul_column_by_poly(test_vec1);
        aes_mul_column_by_poly(test_vec2);
        aes_mul_column_by_poly(test_vec3);
        aes_mul_column_by_poly(test_vec4);

        printf("Result for vec { 219, 19, 83, 69 } is { %" PRIu8 ", %" PRIu8 ", %" PRIu8 ", %" PRIu8 " }\n", test_vec1[0], test_vec1[1], test_vec1[2], test_vec1[3]);
        printf("Result for vec { 242, 10, 34, 92 } is { %" PRIu8 ", %" PRIu8 ", %" PRIu8 ", %" PRIu8 " }\n", test_vec2[0], test_vec2[1], test_vec2[2], test_vec2[3]);
        printf("Result for vec { 1, 1, 1, 1 } is { %" PRIu8 ", %" PRIu8 ", %" PRIu8 ", %" PRIu8 " }\n", test_vec3[0], test_vec3[1], test_vec3[2], test_vec3[3]);
        printf("Result for vec { 198, 198, 198, 198 } is { %" PRIu8 ", %" PRIu8 ", %" PRIu8 ", %" PRIu8 " }\n", test_vec4[0], test_vec4[1], test_vec4[2], test_vec4[3]);
        printf("\n");
#endif

#if 0
        unsigned char txt[] = "abababababababab";
      
        aes_block_t block = { 0 };
        uint8_t buffer[16] = { 0 };
      
        block.block_size = AES_BLOCK_SIZE_128;
        block.block = picoutil_static_calloc(4, 4);
        if (block.block == NULL)
        {
            printf("Failed to allocate memory\n");
            goto reset;
        }
        memcpy(block.block, txt, sizeof(txt) - 1);
      
        printf("Original block: ");
        for (size_t i = 0; i < 4; ++i)
            for (size_t j = 0; j < 4; ++j)
                printf("%" PRIx8 " ", (block.block[i] >> (j * 8)) & 0xff);
        printf("\n");
        memcpy(buffer, txt, sizeof(txt) - 1);
        printf("Original buffer: ");
        for (size_t i = 0; i < 16; ++i)
            printf("%" PRIx8 " ", buffer[i]);
        printf("\n");

        unsigned char raw_key[] = "31415Ap2002#4444";

        aes_key_t key = { 0 };
        key.key_size = AES_KEY_SIZE_128;
        key.key = picoutil_static_calloc(4, 4);
        if (key.key == NULL)
        {
            printf("Failed to allocate memory\n");
            picoutil_static_free(block.block);
            goto reset;
        }
        memcpy(key.key, raw_key, sizeof(raw_key) - 1);
        struct AES_ctx ctx = { 0 };
        uint8_t key_[16] = { 0 };
        memcpy(key_, key.key, 16);
        AES_init_ctx(&ctx, key_);
        
        printf("Key: ");
        for (size_t i = 0; i < 4; ++i)
            for (size_t j = 0; j < 4; ++j)
                printf("%" PRIx8 " ", (key.key[i] >> (j * 8)) & 0xff);
        printf("\n");
        printf("Key_: ");
        for (size_t i = 0; i < 16; ++i)
            printf("%" PRIx8 " ", key_[i]);
        printf("\n");
        
        AES_ECB_encrypt(&ctx, buffer);
        block = picoutil_aes_encrypt_block(block, key);
        
        printf("Encrypted buffer: ");
        for (size_t i = 0; i < 16; ++i)
            printf("%" PRIx8 " ", buffer[i]);
        printf("\n");
        printf("Encrypted block: ");
        for (size_t i = 0; i < 4; ++i)
            for (size_t j = 0; j < 4; ++j)
                printf("%" PRIx8 " ", (block.block[i] >> (j * 8)) & 0xff);
        printf("\n");
        
        picoutil_static_free(block.block);
        picoutil_static_free(key.key);
        printf("\n");
#else
        // picoutil_test_key_schedule();
        // picoutil_test_sub_bytes();
        // picoutil_test_shift_rows();
        //picoutil_test_mix_columns();
        picoutil_test_encryption_ecb_mode(0);
#endif
    }
reset:
    stdio_flush();
    reset_usb_boot(0, 0);
    return 0;
}
