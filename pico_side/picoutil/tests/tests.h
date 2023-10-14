#ifndef TESTS_H
#define TESTS_H

#include <picoutil.h>

__zero_used_regs
void picoutil_test_encryption_ecb_mode(size_t num_rounds);
__zero_used_regs
void test_aes_encrypt_decrypt_ecb(void);
__zero_used_regs
void test_aes_encrypt_decrypt_cbc(void);
__zero_used_regs
// Do not test reference implementation
void test_aes_encrypt_decrypt_ecb_256_blocksize_192_keysize_no_reference(void);
__zero_used_regs
void test_aes_encrypt_decrypt_cbc_192_blocksize_256_keysize_no_reference(void);

__deprecated_msg("Just here in case I need to make tests, but do not use this function")
aes_block_t __time_critical_func(picoutil_aes_encrypt_block)(aes_block_t block, aes_key_t key);

__deprecated_msg("Just here in case I need to make tests, but do not use this function")
aes_block_t __time_critical_func(picoutil_aes_encrypt_block_until)(aes_block_t block, aes_key_t key, size_t num_round);

void test_interqueue(void);

#endif
