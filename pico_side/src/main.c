#define PICOUTIL_TEST

#include <picoutil.h>

#include "../aes_test/aes.h"

__restore_macro(__always_inline)

#include <pico.h>

#include <pico/stdio.h>
#include <pico/stdlib.h>
#include <pico/sync.h>
#include <pico/bootrom.h>
#include <pico/double.h>
#include <hardware/clocks.h>
#include <hardware/xosc.h>
#include <hardware/pll.h>
#include <hardware/structs/mpu.h>

#include <stdint.h>
#include <stddef.h>
#include <stdalign.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h> // memcpy, memset
#include <math.h>
#include <stdio.h>
#include <inttypes.h>

#include <picoutil_fix_macros.h>
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

void set_sysclock_to_xosc(uint32_t freq)
{
    xosc_init();
    clocks_init();
    uint32_t current_xosc_freq = XOSC_MHZ * MHZ;
    if (clock_configure(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC, 0, current_xosc_freq, current_xosc_freq) != true)
        while (true)
            printf("Failed to configure clk_ref\n");
    if (clock_configure(clk_sys, CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLK_REF, 0, current_xosc_freq, current_xosc_freq) != true)
        while (true)
            printf("Failed to configure clk_sys\n");
    uint params[3] = { 0 };
    uint32_t wanted = freq;
    while (freq != 0 && !check_sys_clock_khz(freq, &(params[0]), &(params[1]), &(params[2])))
        freq -= 100;
    if (freq == 0)
    {
        while (true)
            printf("Failed to find a valid clock frequency\n");
        return;
    }
    if (freq != wanted)
        printf("Warning: Could not set system clock to %" PRIu32 "KHz, setting to %" PRIu32 "KHz instead\n", wanted, freq);
    else
        printf("Successfully set system clock to %" PRIu32 "KHz\n", freq);
    set_sys_clock_pll(params[0], params[1], params[2]);
    /* clock_configure(clk_sys, freq, xosc_get_rate_hz()); */
}

void print_clocks(void)
{
    uint32_t clk_gpout0_freq = clock_get_hz(clk_gpout0);
    uint32_t clk_gpout1_freq = clock_get_hz(clk_gpout1);
    uint32_t clk_gpout2_freq = clock_get_hz(clk_gpout2);
    uint32_t clk_gpout3_freq = clock_get_hz(clk_gpout3);
    uint32_t clk_ref_freq = clock_get_hz(clk_ref);
    uint32_t clk_sys_freq = clock_get_hz(clk_sys);
    uint32_t clk_peri_freq = clock_get_hz(clk_peri);
    uint32_t clk_usb_freq = clock_get_hz(clk_usb);
    uint32_t clk_adc_freq = clock_get_hz(clk_adc);
    uint32_t clk_rtc_freq = clock_get_hz(clk_rtc);
    io_rw_32 xosc_status = xosc_hw->status;
    // Crystal Oscillator Status
    // 0x80000000 [31]    : STABLE (0): Oscillator is running and stable
    // 0x01000000 [24]    : BADWRITE (0): An invalid value has been written to CTRL_ENABLE or CTRL_FREQ_RANGE or DORMANT
    // 0x00001000 [12]    : ENABLED (0): Oscillator is enabled but not necessarily running and stable, resets to 0
    // 0x00000003 [1:0]   : FREQ_RANGE (0): The current frequency range setting, always reads 0
    bool is_xosc_active = (xosc_status & XOSC_STATUS_ENABLED_BITS);
    bool is_xosc_stable = (xosc_status & XOSC_STATUS_STABLE_BITS);
    
    printf("clk_gpout0: %" PRIu32 "Hz\n", clk_gpout0_freq);
    printf("clk_gpout1: %" PRIu32 "Hz\n", clk_gpout1_freq);
    printf("clk_gpout2: %" PRIu32 "Hz\n", clk_gpout2_freq);
    printf("clk_gpout3: %" PRIu32 "Hz\n", clk_gpout3_freq);
    printf("clk_ref: %" PRIu32 "Hz\n", clk_ref_freq);
    printf("clk_sys: %" PRIu32 "Hz\n", clk_sys_freq);
    printf("clk_peri: %" PRIu32 "Hz\n", clk_peri_freq);
    printf("clk_usb: %" PRIu32 "Hz\n", clk_usb_freq);
    printf("clk_adc: %" PRIu32 "Hz\n", clk_adc_freq);
    printf("clk_rtc: %" PRIu32 "Hz\n", clk_rtc_freq);
    printf("is_xosc_active: %s\n", is_xosc_active ? "true" : "false");
    printf("is_xosc_stable: %s\n", is_xosc_stable ? "true" : "false");
}


/* clk_gpout0: 0Hz */
/* clk_gpout1: 0Hz */
/* clk_gpout2: 0Hz */
/* clk_gpout3: 0Hz */
/* clk_ref: 12000000Hz */
/* clk_sys: 125000000Hz */
/* clk_peri: 125000000Hz */
/* clk_usb: 48000000Hz */
/* clk_adc: 48000000Hz */
/* clk_rtc: 46875Hz */

/* clk_gpout0: 0Hz */
/* clk_gpout1: 0Hz */
/* clk_gpout2: 0Hz */
/* clk_gpout3: 0Hz */
/* clk_ref: 12000000Hz */
/* clk_sys: 125000000Hz */
/* clk_peri: 125000000Hz */
/* clk_usb: 48000000Hz */
/* clk_adc: 48000000Hz */
/* clk_rtc: 46875Hz */

static void print_mpu_state(void)
{
    printf("mpu_hw->ctrl->enable: %s \n", (mpu_hw->ctrl & M0PLUS_MPU_CTRL_ENABLE_BITS) ? "true" : "false");
}

void test_func_core1(void* arg)
{
    (void)arg;
    printf("Hello from core %u\n", get_core_num());
    picoutil_return_to_core1(NULL, 0);
}

int main(void)
{
    /* clocks_init(); */
    stdio_init_all();
    picoutil_install_exception_handlers();
    picoutil_set_log_threshold(LOG_SUCCESS);
    picoutil_launch_core1();
    /* set_sysclock_to_xosc(48 * MHZ); */
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
    uint64_t count = 0;
    while (true)
    {
        char c = getchar();
        print_clocks();
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
        uint32_t arr[] = { 1, 2, 3, 4, 5 };
        for (size_t i = 0; i < count_of(arr); ++i)
            printf("%" PRIu32 " ", arr[i]);
        picoutil_memset_explicit(arr, 1, sizeof(arr));
        printf("\n");
        for (size_t i = 0; i < count_of(arr); ++i)
            printf("%" PRIu32 " ", arr[i]);
        printf("\n");
        picoutil_memset_explicit(arr, 0, sizeof(arr));
        for (size_t i = 0; i < count_of(arr); ++i)
            printf("%" PRIu32 " ", arr[i]);
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
        // picoutil_test_mix_columns();
        //picoutil_test_encryption_ecb_mode(13);
        // test_aes_encrypt_decrypt_ecb();
        //test_aes_encrypt_decrypt_cbc();
        //__zero_used_regs
        //void test_aes_encrypt_decrypt_ecb_256_blocksize_192_keysize_no_reference(void);
        //test_aes_encrypt_decrypt_ecb_256_blocksize_192_keysize_no_reference();
        //__zero_used_regs
        //void test_aes_encrypt_decrypt_cbc_192_blocksize_256_keysize_no_reference(void);
        //test_aes_encrypt_decrypt_cbc_192_blocksize_256_keysize_no_reference();

        uint32_t a1 = UINT32_MAX / 2 - ((UINT32_MAX / 2) % 2 == 0 ? 0 : 1); 
        uint32_t b1 = 2;

        int32_t a2 = INT32_MIN / 4;
        int32_t b2 = 2;

        printf("%" PRIu32 " == %" PRIu32 "\n", __mul_instruction_unsigned(a1, b1), a1 * b1);
        printf("%" PRIi32 " == %" PRIi32 "\n", __mul_instruction_signed(a2, b2), a2 * b2);

        print_mpu_state();

        __unused
        void lambda(void* arg)
        {
            (void)arg;
            picoutil_log(LOG_SUCCESS, "Hello from core %u in loop iteration number %" PRIu64 "\n", get_core_num(), count);
            picoutil_return_to_core1(NULL, 0);
        }

        picoutil_async_exec(&lambda, NULL, NULL, 0);
        picoutil_wait_result(NULL, 0);

        ++count;

#if 0
        picoutil_static_allocator_set_safe(true);
        uint32_t* test1 = picoutil_static_calloc_aligned(4, 4, 4);
        if (test1 == NULL)
        {
            printf("Failed to allocate memory\n");
            goto reset;
        }
        uint32_t* test2 = picoutil_static_calloc_aligned(4, 100, 1);
        if (test2 == NULL)
        {
            printf("Failed to allocate memory\n");
            picoutil_static_free(test1);
            goto reset;
        }
        uint32_t* test3 = picoutil_static_calloc_aligned(4, 100, 4);
        if (test3 == NULL)
        {
            printf("Failed to allocate memory\n");
            picoutil_static_free(test1);
            picoutil_static_free(test2);
            goto reset;
        }

        memcpy(test1, "Hello", 5);
        memcpy(test2 + 50, "World", 5);
        memcpy(test3 + 50, "!", 1);

        printf("%s\n", test1);
        printf("%s\n", test2 + 50);
        printf("%s\n", test3 + 50);

        picoutil_memset_explicit(test1, 143, 16);
        picoutil_memset_explicit(test2, 143, 400);

        puts("");
        picoutil_static_allocator_dump_hdrs();

        picoutil_static_free(test2);

        byte_t* test4 = picoutil_static_alloc(100);
        if (test4 == NULL)
        {
            printf("Failed to allocate memory\n");
            picoutil_static_free(test1);
            picoutil_static_free(test3);
            goto reset;
        }
        memcpy(test4 + 50, "!", 2);
        printf("%s\n", test4 + 50);

        puts("");
        picoutil_static_allocator_dump_hdrs();

        picoutil_static_free(test1);
        
        puts("");
        picoutil_static_allocator_dump_hdrs();

        picoutil_static_free(test3);
        picoutil_static_free(test4);

        puts("");
        picoutil_static_allocator_dump_hdrs();
#endif
#endif
    }
reset:
    stdio_flush();
    reset_usb_boot(0, 0);
    return 0;
}
