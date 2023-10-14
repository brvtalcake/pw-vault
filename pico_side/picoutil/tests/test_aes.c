#include <picoutil/tests/tests.h>
__restore_macro(__always_inline)

#include <inttypes.h>

#include <picoutil_fix_macros.h>

__deprecated_msg("Just here in case I need to make tests, but do not use this function")
aes_block_t __time_critical_func(picoutil_aes_encrypt_block_until)(aes_block_t block, aes_key_t key, size_t num_round)
{
    aes_key_expanded_t expanded_key = aes_key_schedule(key, block.block_size);
    if (expanded_key.round_keys == NULL)
    {
        return block;
    }
    void print_block(aes_block_t block);
    int	printf (const char *__restrict, ...)
               _ATTRIBUTE ((__format__ (__printf__, 1, 2)));
    aes_block_t key_block = { 0 };
    key_block.block_size = block.block_size;
    key_block.block = expanded_key.round_keys[0];
    printf("RoundKey (0):\n");
    print_block(key_block);
    block = aes_add_round_key(block, expanded_key.round_keys[0]);
    printf("AddRoundKey (0):\n");
    print_block(block);
    if (num_round == 0)
        goto ret;
    for (size_t i = 1; i <= num_round && i < expanded_key.round_count; ++i)
    {
        block = aes_sub_bytes(block);
        printf("SubBytes (%zu):\n", i);
        print_block(block);
        block = aes_shift_rows(block);
        printf("ShiftRows (%zu):\n", i);
        print_block(block);
        block = aes_mix_columns(block);
        printf("MixColumns (%zu):\n", i);
        print_block(block);
        key_block.block = expanded_key.round_keys[i];
        printf("RoundKey (%zu):\n", i);
        print_block(key_block);
        block = aes_add_round_key(block, expanded_key.round_keys[i]);
        printf("AddRoundKey (%zu):\n", i);
        print_block(block);
    }
    if (num_round < expanded_key.round_count)
        goto ret;
    block = aes_sub_bytes(block);
    block = aes_shift_rows(block);
    block = aes_add_round_key(block, expanded_key.round_keys[expanded_key.round_count]);
ret:
    aes_key_expanded_free(expanded_key);
    return block;
}

__deprecated_msg("Just here in case I need to make tests, but do not use this function")
aes_block_t __time_critical_func(picoutil_aes_encrypt_block)(aes_block_t block, aes_key_t key)
{
    aes_key_expanded_t expanded_key = aes_key_schedule(key, block.block_size);
    if (expanded_key.round_keys == NULL)
    {
        return block;
    }
    block = aes_add_round_key(block, expanded_key.round_keys[0]);
    for (size_t i = 1; i < expanded_key.round_count; ++i)
    {
        block = aes_sub_bytes(block);
        block = aes_shift_rows(block);
        block = aes_mix_columns(block);
        block = aes_add_round_key(block, expanded_key.round_keys[i]);
    }
    block = aes_sub_bytes(block);
    block = aes_shift_rows(block);
    block = aes_add_round_key(block, expanded_key.round_keys[expanded_key.round_count]);
    aes_key_expanded_free(expanded_key);
    return block;
}

void print_block(aes_block_t block)
{
    int wc = picoutil_aes_block_word_count(block.block_size);
    if (wc < 0)
        printf("Invalid block size\n");
    byte_t row[wc];
    bzero(row, wc);
    for (size_t i = 0; i < sizeof(aes_word_t); ++i)
    {
        GET_ROW(block, i, row);
        for (int j = 0; j < picoutil_aes_block_word_count(block.block_size); ++j)
            printf("%" PRIx8 " ", row[j]);
        puts("");
    }
    puts("");
}

void print_key(aes_key_t key)
{
    for (int i = 0; i < picoutil_aes_key_word_count(key.key_size); ++i)
    {
        for (size_t j = 0; j < sizeof(aes_word_t); ++j)
            printf("%" PRIx8 " ", ((byte_t*)key.key)[i * sizeof(aes_word_t) + j]);
        puts("");
    }
    puts("");
}

void print_buf(uint8_t* buf, size_t len)
{
    for (size_t i = 0; i < len / sizeof(aes_word_t); ++i)
    {
        for (size_t j = 0; j < sizeof(aes_word_t); ++j)
            printf("%" PRIx8 " ", buf[j * sizeof(aes_word_t) + i]);
        puts("");
    }
    puts("");
}

__restore_macro(__always_inline)

#include "../../aes_test/aes.h"

#include <picoutil_fix_macros.h>

__zero_used_regs
void picoutil_test_encryption_ecb_mode(size_t num_rounds)
{
    puts("TESTING ENCRYPTION (ECB MODE)...");
    unsigned char raw_key[] = "Thats my Kung Fu";
    unsigned char raw_txt[] = "Two One Nine Two";
    aes_key_t key1 = { 0 };
    aes_block_t block1 = { 0 };
    struct AES_ctx ctx2 = { 0 };
    uint8_t key2[16] = { 0 };
    uint8_t buffer2[16] = { 0 };

    key1.key_size = AES_KEY_SIZE_128;
    key1.key = picoutil_static_calloc(4, 4);
    if (key1.key == NULL)
    {
        printf("Failed to allocate memory\n");
        return;
    }
    memcpy(key1.key, raw_key, sizeof(raw_key) - 1);
    block1.block_size = AES_BLOCK_SIZE_128;
    block1.block = picoutil_static_calloc(4, 4);
    if (block1.block == NULL)
    {
        printf("Failed to allocate memory\n");
        picoutil_static_free(key1.key);
        return;
    }
    aes_prepare_entry(raw_txt, block1);
    memcpy(key2, raw_key, sizeof(raw_key) - 1);
    memcpy(buffer2, raw_txt, sizeof(raw_txt) - 1);
    AES_init_ctx(&ctx2, key2);

    /* printf("Block1: (before) \n"); */
    /* print_block(block1); */
    /* printf("\n"); */
    /* printf("Block2: (before) \n"); */
    /* print_buf(buffer2, count_of(buffer2)); */
    /* printf("Key1: \n"); */
    /* print_key(key1); */
    /* printf("Key2: \n"); */
    /* print_buf(key2, count_of(key2)); */

    printf("\n\n");
    __diag_push
    __suppress_warning(deprecated-declarations)
    block1 = picoutil_aes_encrypt_block_until(block1, key1, num_rounds);
    __diag_pop
    AES_ECB_encrypt(&ctx2, buffer2);
    
    printf("Block1: (encrypted) \n");
    print_block(block1);
    printf("\n");
    printf("Block2: (encrypted) \n");
    print_buf(buffer2, count_of(buffer2));
    printf("\n\n");

    picoutil_static_free(key1.key);
    picoutil_static_free(block1.block);
}

__zero_used_regs
void test_aes_encrypt_decrypt_ecb(void)
{
    __buffer_var unsigned char key_buf[16] = "blablablatruc666";
    __buffer_var unsigned char txt[32] = "This is a test! This is a test ";
    __buffer_var unsigned char txt_cpy[32] = { 0 };
    memcpy(txt_cpy, txt, 32);
    // ORIGINAL TEXT
    puts("ORIGINAL TEXT:\n");
    print_buf(txt, 32);

    puts("TESTING ENCRYPTION (ECB MODE)...\n");

    // MY IMPLEMENTATION
    aes_context_t ctx = { 0 };
    aes_key_t key = { 0 };
    if (!picoutil_aes_key_init(&key, AES_KEY_SIZE_128, key_buf, 16))
    {
        printf("Failed to initialize key\n");
        return;
    }
    if (!picoutil_aes_context_init(&ctx, AES_MODE_ECB, AES_DIR_ENCRYPT, AES_KEY_SIZE_128, AES_BLOCK_SIZE_128, &key))
    {
        printf("Failed to initialize context\n");
        return;
    }
    picoutil_aes_key_destroy(&key); // Since its content (and not the pointer) is copied into the context, we can destroy it safely

    aes_result_t res = picoutil_aes_process(&ctx, txt, 32);
    if (res.error)
    {
        printf("Failed to encrypt\n");
        return;
    }
    // REFERENCE IMPLEMENTATION
    struct AES_ctx ctx_ref = { 0 };
    AES_init_ctx(&ctx_ref, key_buf);
    AES_ECB_encrypt(&ctx_ref, txt_cpy);
    AES_ECB_encrypt(&ctx_ref, txt_cpy + 16);

    printf("MY IMPLEMENTATION:\n");
    print_buf(res.data, res.data_size);
    printf("REFERENCE IMPLEMENTATION:\n");
    print_buf(txt_cpy, 32);

    puts("TESTING DECRYPTION (ECB MODE)...\n");
    ctx.dir = AES_DIR_DECRYPT;
    aes_result_t old_res = (aes_result_t) { .data = res.data, .data_size = res.data_size };
    res = picoutil_aes_process(&ctx, res.data, res.data_size);
    if (res.error)
    {
        printf("Failed to decrypt\n");
        return;
    }
    AES_ECB_decrypt(&ctx_ref, txt_cpy);
    AES_ECB_decrypt(&ctx_ref, txt_cpy + 16);

    printf("MY IMPLEMENTATION:\n");
    print_buf(res.data, res.data_size);
    printf("REFERENCE IMPLEMENTATION:\n");
    print_buf(txt_cpy, 32);

    picoutil_aes_result_destroy(&old_res);
    picoutil_aes_result_destroy(&res);
    picoutil_aes_context_deinit(&ctx);

    picoutil_static_allocator_dump_hdrs();
}

__zero_used_regs
void test_aes_encrypt_decrypt_cbc(void)
{
    __buffer_var unsigned char key_buf[16] = "blablablatruc666";
    __buffer_var unsigned char txt[32] = "This is a test! This is a test!";
#if 0
    __buffer_var unsigned char iv_buf[16] = { 0 };
    if (!picoutil_rand_fill(iv_buf, 16))
    {
        printf("Failed to fill IV\n");
        return;
    }
#else
    __buffer_var unsigned char iv_buf[16] = "1234567890123456";
#endif
    __buffer_var unsigned char key_cpy[16] = "blablablatruc666";
    __buffer_var unsigned char txt_cpy[32] = "This is a test! This is a test!";
    __buffer_var unsigned char iv_cpy[16] = "1234567890123456";

    // ORIGINAL TEXT
    puts("ORIGINAL TEXT:\n");
    print_buf(txt, 32);

    puts("TESTING ENCRYPTION (CBC MODE)...\n");

    // MY IMPLEMENTATION
    aes_context_t ctx = { 0 };
    aes_key_t key = { 0 };
    aes_block_t iv = { 0 };
    if (!picoutil_aes_key_init(&key, AES_KEY_SIZE_128, key_buf, 16))
    {
        printf("Failed to initialize key\n");
        return;
    }
    if (!picoutil_aes_iv_init(&iv, AES_BLOCK_SIZE_128, iv_buf))
    {
        printf("Failed to initialize IV\n");
        return;
    }
    if (!picoutil_aes_context_init(&ctx, AES_MODE_CBC, AES_DIR_ENCRYPT, AES_KEY_SIZE_128, AES_BLOCK_SIZE_128, &key, &iv))
    {
        printf("Failed to initialize context\n");
        return;
    }
    picoutil_aes_key_destroy(&key); // Since its content (and not the pointer) is copied into the context, we can destroy it safely
    picoutil_aes_iv_destroy(&iv);

    aes_result_t res = picoutil_aes_process(&ctx, txt, 32);
    if (res.error)
    {
        printf("Failed to encrypt\n");
        return;
    }
    // REFERENCE IMPLEMENTATION
    struct AES_ctx ctx_ref = { 0 };
    AES_init_ctx_iv(&ctx_ref, key_cpy, iv_cpy);
    AES_CBC_encrypt_buffer(&ctx_ref, txt_cpy, 32);

    printf("MY IMPLEMENTATION:\n");
    print_buf(res.data, res.data_size);
    printf("REFERENCE IMPLEMENTATION:\n");
    print_buf(txt_cpy, 32);

    puts("TESTING DECRYPTION (CBC MODE)...\n");
    ctx.dir = AES_DIR_DECRYPT;
    aes_result_t old_res = (aes_result_t) { .data = res.data, .data_size = res.data_size };
    res = picoutil_aes_process(&ctx, res.data, res.data_size);
    if (res.error)
    {
        printf("Failed to decrypt\n");
        return;
    }
    AES_ctx_set_iv(&ctx_ref, iv_buf);
    AES_CBC_decrypt_buffer(&ctx_ref, txt_cpy, 32);

    printf("MY IMPLEMENTATION:\n");
    print_buf(res.data, res.data_size);
    printf("REFERENCE IMPLEMENTATION:\n");
    print_buf(txt_cpy, 32);

    picoutil_aes_result_destroy(&old_res);
    picoutil_aes_result_destroy(&res);
    picoutil_aes_context_deinit(&ctx);

    picoutil_static_allocator_dump_hdrs();
}

__zero_used_regs
// Do not test reference implementation
void test_aes_encrypt_decrypt_ecb_256_blocksize_192_keysize_no_reference(void)
{
    __buffer_var unsigned char key_buf[24] = "blablablatruc666bla1234";
    __buffer_var unsigned char txt[64] = "This is a test! This is a test! This is a test! This is a test! ";
    __buffer_var unsigned char txt_cpy[64] = { 0 };
    memcpy(txt_cpy, txt, 64);

    puts("ORIGINAL TEXT:\n");
    print_buf(txt, 64);

    puts("TESTING ENCRYPTION (ECB MODE)...\n");

    aes_context_t ctx = { 0 };
    aes_key_t key = { 0 };
    if (!picoutil_aes_key_init(&key, AES_KEY_SIZE_192, key_buf, 24))
    {
        printf("Failed to initialize key\n");
        return;
    }
    if (!picoutil_aes_context_init(&ctx, AES_MODE_ECB, AES_DIR_ENCRYPT, AES_KEY_SIZE_192, AES_BLOCK_SIZE_256, &key))
    {
        printf("Failed to initialize context\n");
        return;
    }
    picoutil_aes_key_destroy(&key);

    aes_result_t res = picoutil_aes_process(&ctx, txt, 64);
    if (res.error)
    {
        printf("Failed to encrypt\n");
        return;
    }

    printf("MY IMPLEMENTATION:\n");
    print_buf(res.data, res.data_size);

    puts("TESTING DECRYPTION (ECB MODE)...\n");
    ctx.dir = AES_DIR_DECRYPT;
    aes_result_t old_res = (aes_result_t) { .data = res.data, .data_size = res.data_size };
    res = picoutil_aes_process(&ctx, res.data, res.data_size);
    if (res.error)
    {
        printf("Failed to decrypt\n");
        return;
    }

    printf("MY IMPLEMENTATION:\n");
    print_buf(res.data, res.data_size);

    if (memcmp(txt, res.data, 64) != 0)
        printf("FAILURE: %s != %s\n", txt, (const char*) strndupa((const char*) res.data, 64));
    else
        printf("SUCCESS: %s == %s\n", txt, (const char*) strndupa((const char*) res.data, 64));

    picoutil_aes_result_destroy(&old_res);
    picoutil_aes_result_destroy(&res);
    picoutil_aes_context_deinit(&ctx);

    picoutil_static_allocator_dump_hdrs();
}

__zero_used_regs
void test_aes_encrypt_decrypt_cbc_192_blocksize_256_keysize_no_reference(void)
{
    __buffer_var unsigned char key_buf[32] = "blablablatruc666bla1234bla5678";
    __buffer_var unsigned char txt[64] = "This is a test! This is a test! This is a test! This is a test! ";
    __buffer_var unsigned char txt_cpy[64] = { 0 };
    // The iv must be of the same size as the block size (i.e. 192 bits)
    __buffer_var unsigned char iv_buf[24] = "123456789012345678901234";
    __unused __buffer_var unsigned char iv_cpy[24] = "123456789012345678901234";
    memcpy(txt_cpy, txt, 64);

    puts("ORIGINAL TEXT:\n");
    print_buf(txt, 64);

    puts("TESTING ENCRYPTION (CBC MODE)...\n");

    aes_context_t ctx = { 0 };
    aes_key_t key = { 0 };
    aes_block_t iv = { 0 };
    if (!picoutil_aes_key_init(&key, AES_KEY_SIZE_256, key_buf, 32))
    {
        printf("Failed to initialize key\n");
        return;
    }
    if (!picoutil_aes_iv_init(&iv, AES_BLOCK_SIZE_192, iv_buf))
    {
        printf("Failed to initialize IV\n");
        return;
    }
    if (!picoutil_aes_context_init(&ctx, AES_MODE_CBC, AES_DIR_ENCRYPT, AES_KEY_SIZE_256, AES_BLOCK_SIZE_192, &key, &iv))
    {
        printf("Failed to initialize context\n");
        return;
    }
    picoutil_aes_key_destroy(&key);
    picoutil_aes_iv_destroy(&iv);

    aes_result_t res = picoutil_aes_process(&ctx, txt, 64);
    if (res.error)
    {
        printf("Failed to encrypt\n");
        return;
    }

    printf("MY IMPLEMENTATION:\n");
    print_buf(res.data, res.data_size);

    puts("TESTING DECRYPTION (CBC MODE)...\n");
    ctx.dir = AES_DIR_DECRYPT;
    aes_result_t old_res = (aes_result_t) { .data = res.data, .data_size = res.data_size };
    res = picoutil_aes_process(&ctx, res.data, res.data_size);
    if (res.error)
    {
        printf("Failed to decrypt\n");
        return;
    }

    printf("MY IMPLEMENTATION:\n");
    print_buf(res.data, res.data_size);

    if (memcmp(txt, res.data, 64) != 0)
        printf("FAILURE: %s != %s\n", txt, (const char*) strndupa((const char*) res.data, 64));
    else
        printf("SUCCESS: %s == %s\n", txt, (const char*) strndupa((const char*) res.data, 64));

    picoutil_aes_result_destroy(&old_res);
    picoutil_aes_result_destroy(&res);
    picoutil_aes_context_deinit(&ctx);

    picoutil_static_allocator_dump_hdrs();
}