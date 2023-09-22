#include <picoutil.h>

#include <pico/sync.h>

auto_init_recursive_mutex(aes_error_mutex);
static bool aes_error = false;

#include <string.h>

#ifdef GET_ROW
    #undef GET_ROW
#endif
#define GET_ROW(BLOCK, ROW, OUTPUT)                                 \
    byte_t OUTPUT[sizeof(aes_word_t)] = { 0 };                      \
    __extension__                                                   \
    ({                                                              \
        byte_t temp[sizeof(aes_word_t)] = { 0 };                    \
        temp[0] = ((byte_t*)&((BLOCK).block[0]))[(ROW)];            \
        temp[1] = ((byte_t*)&((BLOCK).block[1]))[(ROW)];            \
        temp[2] = ((byte_t*)&((BLOCK).block[2]))[(ROW)];            \
        temp[3] = ((byte_t*)&((BLOCK).block[3]))[(ROW)];            \
        memcpy((OUTPUT), temp, sizeof(aes_word_t));                 \
    })

#ifdef SET_ROW
    #undef SET_ROW
#endif
#define SET_ROW(BLOCK, ROW, INPUT)                                  \
    __extension__                                                   \
    ({                                                              \
        byte_t temp[sizeof(aes_word_t)] = { 0 };                    \
        memcpy(temp, (INPUT), sizeof(aes_word_t));                  \
        ((byte_t*)&((BLOCK).block[0]))[(ROW)] = temp[0];            \
        ((byte_t*)&((BLOCK).block[1]))[(ROW)] = temp[1];            \
        ((byte_t*)&((BLOCK).block[2]))[(ROW)] = temp[2];            \
        ((byte_t*)&((BLOCK).block[3]))[(ROW)] = temp[3];            \
    })

#ifdef GET_COLUMN
    #undef GET_COLUMN
#endif
#define GET_COLUMN(BLOCK, COLUMN, OUTPUT)                           \
    byte_t OUTPUT[sizeof(aes_word_t)] = { 0 };                      \
    __extension__                                                   \
    ({                                                              \
        byte_t temp[sizeof(aes_word_t)] = { 0 };                    \
        temp[0] = ((byte_t*)&((BLOCK).block[(COLUMN)]))[0];         \
        temp[1] = ((byte_t*)&((BLOCK).block[(COLUMN)]))[1];         \
        temp[2] = ((byte_t*)&((BLOCK).block[(COLUMN)]))[2];         \
        temp[3] = ((byte_t*)&((BLOCK).block[(COLUMN)]))[3];         \
        memcpy((OUTPUT), temp, sizeof(aes_word_t));                 \
    })

#ifdef SET_COLUMN
    #undef SET_COLUMN
#endif
#define SET_COLUMN(BLOCK, COLUMN, INPUT)                            \
    __extension__                                                   \
    ({                                                              \
        byte_t temp[sizeof(aes_word_t)] = { 0 };                    \
        memcpy(temp, (INPUT), sizeof(aes_word_t));                  \
        ((byte_t*)&((BLOCK).block[(COLUMN)]))[0] = temp[0];         \
        ((byte_t*)&((BLOCK).block[(COLUMN)]))[1] = temp[1];         \
        ((byte_t*)&((BLOCK).block[(COLUMN)]))[2] = temp[2];         \
        ((byte_t*)&((BLOCK).block[(COLUMN)]))[3] = temp[3];         \
    })

void picoutil_aes_init(void)
{
    picoutil_static_allocator_init(true);
    if (UNLIKELY(!recursive_mutex_is_initialized(&aes_error_mutex)))
        recursive_mutex_init(&aes_error_mutex);
    recursive_mutex_enter_blocking(&aes_error_mutex);
    aes_error = false;
    recursive_mutex_exit(&aes_error_mutex);
}

bool picoutil_aes_error(void)
{
    recursive_mutex_enter_blocking(&aes_error_mutex);
    bool error = aes_error;
    aes_error = false;
    recursive_mutex_exit(&aes_error_mutex);
    return error;
}

ATTRIBUTE(const)
static inline byte_t aes_round_constant_calc_rc(uint8_t round)
{
    switch (round)
    {
        case 0: // Normally ill-formed
            ATTRIBUTE(fallthrough);
        case 1:
            return 0x01;
        case 2:
            return 0x02;
        case 3:
            return 0x04;
        case 4:
            return 0x08;
        case 5:
            return 0x10;
        case 6:
            return 0x20;
        case 7:
            return 0x40;
        case 8:
            return 0x80;
        case 9:
            return 0x1b;
        case 10:
            return 0x36;
        default:
            return (MUL(2, aes_round_constant_calc_rc(round - 1))) ^ 0x11b;
    }
    __unreachable();
    return (byte_t)-1;
}

ATTRIBUTE(const)
static inline aes_word_t aes_round_constant(uint8_t round)
{
    byte_t rcon[4] = { aes_round_constant_calc_rc(round), 0, 0, 0 };
    return *(aes_word_t*)rcon;
}

ATTRIBUTE(const)
static byte_t aes_sbox(byte_t x)
{
    static const byte_t sbox[256] = {
        0x63, 0x7c, 0x77, 0x7b,
        0xf2, 0x6b, 0x6f, 0xc5,
        0x30, 0x01, 0x67, 0x2b,
        0xfe, 0xd7, 0xab, 0x76,
        0xca, 0x82, 0xc9, 0x7d,
        0xfa, 0x59, 0x47, 0xf0,
        0xad, 0xd4, 0xa2, 0xaf,
        0x9c, 0xa4, 0x72, 0xc0,
        0xb7, 0xfd, 0x93, 0x26,
        0x36, 0x3f, 0xf7, 0xcc,
        0x34, 0xa5, 0xe5, 0xf1,
        0x71, 0xd8, 0x31, 0x15,
        0x04, 0xc7, 0x23, 0xc3,
        0x18, 0x96, 0x05, 0x9a,
        0x07, 0x12, 0x80, 0xe2,
        0xeb, 0x27, 0xb2, 0x75,
        0x09, 0x83, 0x2c, 0x1a,
        0x1b, 0x6e, 0x5a, 0xa0,
        0x52, 0x3b, 0xd6, 0xb3,
        0x29, 0xe3, 0x2f, 0x84,
        0x53, 0xd1, 0x00, 0xed,
        0x20, 0xfc, 0xb1, 0x5b,
        0x6a, 0xcb, 0xbe, 0x39,
        0x4a, 0x4c, 0x58, 0xcf,
        0xd0, 0xef, 0xaa, 0xfb,
        0x43, 0x4d, 0x33, 0x85,
        0x45, 0xf9, 0x02, 0x7f,
        0x50, 0x3c, 0x9f, 0xa8,
        0x51, 0xa3, 0x40, 0x8f,
        0x92, 0x9d, 0x38, 0xf5,
        0xbc, 0xb6, 0xda, 0x21,
        0x10, 0xff, 0xf3, 0xd2,
        0xcd, 0x0c, 0x13, 0xec,
        0x5f, 0x97, 0x44, 0x17,
        0xc4, 0xa7, 0x7e, 0x3d,
        0x64, 0x5d, 0x19, 0x73,
        0x60, 0x81, 0x4f, 0xdc,
        0x22, 0x2a, 0x90, 0x88,
        0x46, 0xee, 0xb8, 0x14,
        0xde, 0x5e, 0x0b, 0xdb,
        0xe0, 0x32, 0x3a, 0x0a,
        0x49, 0x06, 0x24, 0x5c,
        0xc2, 0xd3, 0xac, 0x62,
        0x91, 0x95, 0xe4, 0x79,
        0xe7, 0xc8, 0x37, 0x6d,
        0x8d, 0xd5, 0x4e, 0xa9,
        0x6c, 0x56, 0xf4, 0xea,
        0x65, 0x7a, 0xae, 0x08,
        0xba, 0x78, 0x25, 0x2e,
        0x1c, 0xa6, 0xb4, 0xc6,
        0xe8, 0xdd, 0x74, 0x1f,
        0x4b, 0xbd, 0x8b, 0x8a,
        0x70, 0x3e, 0xb5, 0x66,
        0x48, 0x03, 0xf6, 0x0e,
        0x61, 0x35, 0x57, 0xb9,
        0x86, 0xc1, 0x1d, 0x9e,
        0xe1, 0xf8, 0x98, 0x11,
        0x69, 0xd9, 0x8e, 0x94,
        0x9b, 0x1e, 0x87, 0xe9,
        0xce, 0x55, 0x28, 0xdf,
        0x8c, 0xa1, 0x89, 0x0d,
        0xbf, 0xe6, 0x42, 0x68,
        0x41, 0x99, 0x2d, 0x0f,
        0xb0, 0x54, 0xbb, 0x16
    };
    return sbox[x];
}

ATTRIBUTE(const)
static byte_t aes_inv_sbox(byte_t x)
{
    static const byte_t inv_sbox[256] = {
        0x52, 0x09, 0x6a, 0xd5,
        0x30, 0x36, 0xa5, 0x38,
        0xbf, 0x40, 0xa3, 0x9e,
        0x81, 0xf3, 0xd7, 0xfb,
        0x7c, 0xe3, 0x39, 0x82,
        0x9b, 0x2f, 0xff, 0x87,
        0x34, 0x8e, 0x43, 0x44,
        0xc4, 0xde, 0xe9, 0xcb,
        0x54, 0x7b, 0x94, 0x32,
        0xa6, 0xc2, 0x23, 0x3d,
        0xee, 0x4c, 0x95, 0x0b,
        0x42, 0xfa, 0xc3, 0x4e,
        0x08, 0x2e, 0xa1, 0x66,
        0x28, 0xd9, 0x24, 0xb2,
        0x76, 0x5b, 0xa2, 0x49,
        0x6d, 0x8b, 0xd1, 0x25,
        0x72, 0xf8, 0xf6, 0x64,
        0x86, 0x68, 0x98, 0x16,
        0xd4, 0xa4, 0x5c, 0xcc,
        0x5d, 0x65, 0xb6, 0x92,
        0x6c, 0x70, 0x48, 0x50,
        0xfd, 0xed, 0xb9, 0xda,
        0x5e, 0x15, 0x46, 0x57,
        0xa7, 0x8d, 0x9d, 0x84,
        0x90, 0xd8, 0xab, 0x00,
        0x8c, 0xbc, 0xd3, 0x0a,
        0xf7, 0xe4, 0x58, 0x05,
        0xb8, 0xb3, 0x45, 0x06,
        0xd0, 0x2c, 0x1e, 0x8f,
        0xca, 0x3f, 0x0f, 0x02,
        0xc1, 0xaf, 0xbd, 0x03,
        0x01, 0x13, 0x8a, 0x6b,
        0x3a, 0x91, 0x11, 0x41,
        0x4f, 0x67, 0xdc, 0xea,
        0x97, 0xf2, 0xcf, 0xce,
        0xf0, 0xb4, 0xe6, 0x73,
        0x96, 0xac, 0x74, 0x22,
        0xe7, 0xad, 0x35, 0x85,
        0xe2, 0xf9, 0x37, 0xe8,
        0x1c, 0x75, 0xdf, 0x6e,
        0x47, 0xf1, 0x1a, 0x71,
        0x1d, 0x29, 0xc5, 0x89,
        0x6f, 0xb7, 0x62, 0x0e,
        0xaa, 0x18, 0xbe, 0x1b,
        0xfc, 0x56, 0x3e, 0x4b,
        0xc6, 0xd2, 0x79, 0x20,
        0x9a, 0xdb, 0xc0, 0xfe,
        0x78, 0xcd, 0x5a, 0xf4,
        0x1f, 0xdd, 0xa8, 0x33,
        0x88, 0x07, 0xc7, 0x31,
        0xb1, 0x12, 0x10, 0x59,
        0x27, 0x80, 0xec, 0x5f,
        0x60, 0x51, 0x7f, 0xa9,
        0x19, 0xb5, 0x4a, 0x0d,
        0x2d, 0xe5, 0x7a, 0x9f,
        0x93, 0xc9, 0x9c, 0xef,
        0xa0, 0xe0, 0x3b, 0x4d,
        0xae, 0x2a, 0xf5, 0xb0,
        0xc8, 0xeb, 0xbb, 0x3c,
        0x83, 0x53, 0x99, 0x61,
        0x17, 0x2b, 0x04, 0x7e,
        0xba, 0x77, 0xd6, 0x26,
        0xe1, 0x69, 0x14, 0x63,
        0x55, 0x21, 0x0c, 0x7d
    };
    return inv_sbox[x];
}

ATTRIBUTE(const) ATTRIBUTE(flatten)
static inline aes_word_t aes_sub_word(aes_word_t x)
{
    byte_t bytes[sizeof(aes_word_t)] = { 0 };
    memcpy(bytes, &x, sizeof(aes_word_t));
    bytes[0] = aes_sbox(bytes[0]);
    bytes[1] = aes_sbox(bytes[1]);
    bytes[2] = aes_sbox(bytes[2]);
    bytes[3] = aes_sbox(bytes[3]);
    memcpy(&x, bytes, sizeof(aes_word_t));
    return x;
}

ATTRIBUTE(const) ATTRIBUTE(flatten)
static inline aes_word_t aes_inv_sub_word(aes_word_t x)
{
    byte_t bytes[sizeof(aes_word_t)] = { 0 };
    memcpy(bytes, &x, sizeof(aes_word_t));
    bytes[0] = aes_inv_sbox(bytes[0]);
    bytes[1] = aes_inv_sbox(bytes[1]);
    bytes[2] = aes_inv_sbox(bytes[2]);
    bytes[3] = aes_inv_sbox(bytes[3]);
    memcpy(&x, bytes, sizeof(aes_word_t));
    return x;
}

// Rotate the bytes in a 32-bit word left by one
ATTRIBUTE(const) ATTRIBUTE(flatten)
static inline aes_word_t aes_rot_word_left(aes_word_t x)
{
    return ROTATE(LEFT, x, 8);
}

// Rotate the bytes in a 32-bit word right by one
ATTRIBUTE(const) ATTRIBUTE(flatten)
static inline aes_word_t aes_rot_word_right(aes_word_t x)
{
    return ROTATE(RIGHT, x, 8);
}

static aes_key_expanded_t aes_key_schedule(aes_key_t key, ATTRIBUTE(unused) aes_block_size bsize)
{
    picoutil_static_allocator_init(true);
    const aes_word_t N = picoutil_aes_key_word_count(key.key_size);
    const size_t R = picoutil_aes_round_count(key.key_size) + 1;
    // In 32-bit words (aes_word_t)
    const aes_word_t BSIZE = picoutil_aes_block_word_count(bsize);
    // To change when considering block size
    // sizeof(full_expanded_key) (IN BITS) == {4 * 11 * 32 (for AES-128), 4 * 13 * 32 (for AES-192), 4 * 15 * 32 (for AES-256)}
    aes_word_t full_expanded_key[BSIZE * (picoutil_aes_round_count(key.key_size) + 1)];
    picoutil_memset_explicit(full_expanded_key, 0, sizeof(full_expanded_key));
    for (size_t i = 0; i < BSIZE * (picoutil_aes_round_count(key.key_size) + 1); ++i)
    {
        divmod_result_t divmod = DIVMODu32(i, N);
        if (i < N)
            full_expanded_key[i] = key.key[i];
        else if (to_remainder_u32(divmod) == 0)
            full_expanded_key[i] = full_expanded_key[i - N]                 ^
                aes_sub_word(aes_rot_word_right(full_expanded_key[i - 1]))  ^
                aes_round_constant(to_quotient_u32(divmod));
        else if (N > 6 && to_remainder_u32(divmod) == 4)
            full_expanded_key[i] = full_expanded_key[i - N] ^ aes_sub_word(full_expanded_key[i - 1]);
        else
            full_expanded_key[i] = full_expanded_key[i - N] ^ full_expanded_key[i - 1];
    }
    aes_key_expanded_t expanded_key = { .round_keys = picoutil_static_calloc(picoutil_aes_round_count(key.key_size) + 1, sizeof(aes_word_t*)) };
    if (expanded_key.round_keys == NULL)
        return (aes_key_expanded_t){ 0 };
    for (size_t i = 0; i < picoutil_aes_round_count(key.key_size) + 1; ++i)
    {
        expanded_key.round_keys[i] = picoutil_static_calloc(BSIZE, sizeof(aes_word_t));
        if (expanded_key.round_keys[i] == NULL)
        {
            for (size_t j = 0; j < i; ++j)
                picoutil_static_free(expanded_key.round_keys[j]);
            picoutil_static_free(expanded_key.round_keys);
            return (aes_key_expanded_t){ 0 };
        }
        memcpy(expanded_key.round_keys[i], full_expanded_key + (i * BSIZE), BSIZE * sizeof(aes_word_t));
    }
    expanded_key.round_count = picoutil_aes_round_count(key.key_size);
    return expanded_key;
}

static inline void aes_key_expanded_free(aes_key_expanded_t key)
{
    if (key.round_keys != NULL)
    {
        for (size_t i = 0; i < key.round_count + 1; ++i)
            picoutil_static_free(key.round_keys[i]);
        picoutil_static_free(key.round_keys);
    }
}

// This function is taken from Wikipedia example implementation
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

ATTRIBUTE(flatten)
static inline aes_block_t aes_sub_bytes(aes_block_t block)
{
    for (size_t i = 0; i < picoutil_aes_block_word_count(block.block_size); ++i)
        block.block[i] = aes_sub_word(block.block[i]);
    return block;
}

ATTRIBUTE(flatten)
static inline aes_block_t aes_shift_rows(aes_block_t block)
{
    for (size_t i = 0; i < picoutil_aes_block_word_count(block.block_size); ++i)
    {
#if 0
        for (size_t j = 0; j < i; ++j)
            block.block[i] = aes_rot_word_right(block.block[i]);
#else
        GET_ROW(block, i, row);
        aes_word_t full_row = 0;
        memcpy(&full_row, row, sizeof(aes_word_t));
        for (size_t j = 0; j < i; ++j)
            full_row = aes_rot_word_right(full_row);
        memcpy(row, &full_row, sizeof(aes_word_t));
        SET_ROW(block, i, row);
#endif
    }
    return block;
}

static inline aes_block_t aes_mix_columns(aes_block_t block)
{
    for (size_t i = 0; i < sizeof(aes_word_t); ++i)
    {
        GET_COLUMN(block, i, column);
        aes_mul_column_by_poly(column);
        SET_COLUMN(block, i, column);
    }
    return block;
}

static inline aes_block_t aes_add_round_key(aes_block_t block, aes_word_t* round_key)
{
    for (size_t i = 0; i < picoutil_aes_block_word_count(block.block_size); ++i)
    {
        byte_t bytes[sizeof(aes_word_t)] = { 0 };
        byte_t key_bytes[sizeof(aes_word_t)] = { 0 };
        memcpy(bytes, &block.block[i], sizeof(aes_word_t));
        memcpy(key_bytes, &round_key[i], sizeof(aes_word_t));
        bytes[0] ^= key_bytes[0];
        bytes[1] ^= key_bytes[1];
        bytes[2] ^= key_bytes[2];
        bytes[3] ^= key_bytes[3];
        memcpy(&block.block[i], bytes, sizeof(aes_word_t));
    }
    return block;
}

#if 0
// Takes a block and applies the diagonal symmetry to it as if it were a matrix
/*
 * Example:
 *
 * a0 a1 a2 a3        a0 b0 c0 d0
 * b0 b1 b2 b3   ->   a1 b1 c1 d1
 * c0 c1 c2 c3        a2 b2 c2 d2
 * d0 d1 d2 d3        a3 b3 c3 d3
 */
static inline aes_block_t aes_block_apply_diagonal_symmetry(aes_block_t block)
{
    aes_word_t block_cpy[picoutil_aes_block_word_count(block.block_size)];
    picoutil_memset_explicit(block_cpy, 0, sizeof(block_cpy));
    memcpy(block_cpy, block.block, sizeof(block_cpy));

    picoutil_memset_explicit(block.block, 0, sizeof(aes_word_t) * picoutil_aes_block_word_count(block.block_size));
    block.block[0] = (block_cpy[0] & UINT32_C(0xff000000)) | ((block_cpy[1] & UINT32_C(0xff000000)) >> 8) | ((block_cpy[2] & UINT32_C(0xff000000)) >> 16) | ((block_cpy[3] & UINT32_C(0xff000000)) >> 24);
    block.block[1] = ((block_cpy[0] & UINT32_C(0x00ff0000)) << 8) | (block_cpy[1] & UINT32_C(0x00ff0000)) | ((block_cpy[2] & UINT32_C(0x00ff0000)) >> 8) | ((block_cpy[3] & UINT32_C(0x00ff0000)) >> 16);
    block.block[2] = ((block_cpy[0] & UINT32_C(0x0000ff00)) << 16) | ((block_cpy[1] & UINT32_C(0x0000ff00)) << 8) | (block_cpy[2] & UINT32_C(0x0000ff00)) | ((block_cpy[3] & UINT32_C(0x0000ff00)) >> 8);
    block.block[3] = ((block_cpy[0] & UINT32_C(0x000000ff)) << 24) | ((block_cpy[1] & UINT32_C(0x000000ff)) << 16) | ((block_cpy[2] & UINT32_C(0x000000ff)) << 8) | (block_cpy[3] & UINT32_C(0x000000ff));
    
    return block;
}
#endif

static inline aes_block_t aes_prepare_entry(byte_t* entry, aes_block_t block)
{
    picoutil_memset_explicit(block.block, 0, sizeof(aes_word_t) * picoutil_aes_block_word_count(block.block_size));
    byte_t col1[sizeof(aes_word_t)] = { 0 };
    byte_t col2[sizeof(aes_word_t)] = { 0 };
    byte_t col3[sizeof(aes_word_t)] = { 0 };
    byte_t col4[sizeof(aes_word_t)] = { 0 };
    memcpy(col1, entry, sizeof(aes_word_t));
    memcpy(col2, entry + sizeof(aes_word_t), sizeof(aes_word_t));
    memcpy(col3, entry + (sizeof(aes_word_t) * 2), sizeof(aes_word_t));
    memcpy(col4, entry + (sizeof(aes_word_t) * 3), sizeof(aes_word_t));
    SET_COLUMN(block, 0, col1);
    SET_COLUMN(block, 1, col2);
    SET_COLUMN(block, 2, col3);
    SET_COLUMN(block, 3, col4);
    return block;
}

aes_block_t __time_critical_func(picoutil_aes_encrypt_block)(aes_block_t block, aes_key_t key)
{
    aes_key_expanded_t expanded_key = aes_key_schedule(key, block.block_size);
    if (expanded_key.round_keys == NULL)
    {
        recursive_mutex_enter_blocking(&aes_error_mutex);
        aes_error = true;
        recursive_mutex_exit(&aes_error_mutex);
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

aes_block_t __time_critical_func(picoutil_aes_encrypt_block_until)(aes_block_t block, aes_key_t key, size_t num_round)
{
    aes_key_expanded_t expanded_key = aes_key_schedule(key, block.block_size);
    if (expanded_key.round_keys == NULL)
    {
        recursive_mutex_enter_blocking(&aes_error_mutex);
        aes_error = true;
        recursive_mutex_exit(&aes_error_mutex);
        return block;
    }
#if 0
    aes_block_t round_key_block = { 0 };
    round_key_block.block_size = block.block_size;
    round_key_block.block = picoutil_static_calloc(picoutil_aes_block_word_count(block.block_size), sizeof(aes_word_t));
    if (round_key_block.block == NULL)
    {
        aes_key_expanded_free(expanded_key);
        recursive_mutex_enter_blocking(&aes_error_mutex);
        aes_error = true;
        recursive_mutex_exit(&aes_error_mutex);
        return block;
    }
    memcpy(round_key_block.block, expanded_key.round_keys[0], picoutil_aes_block_word_count(block.block_size) * sizeof(aes_word_t));
    block = aes_add_round_key(aes_block_apply_diagonal_symmetry(block), aes_block_apply_diagonal_symmetry(round_key_block).block);
#else
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
#endif
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

// NOTE: Try combining these (above and below) into two single functions (aes_round and aes_inv_round)
// TODO: aes_inv_sub_bytes, aes_inv_shift_rows, aes_inv_mix_columns, aes_inv_add_round_key

// TODO: aes_encrypt, aes_decrypt
// TODO: aes_cbc_encrypt, aes_cbc_decrypt
// TODO: aes_ctr_encrypt, aes_ctr_decrypt
// ...

// TODO: Maybe consider adding other AES variants with different block sizes (not only different key sizes)

#include <inttypes.h>
#include <stdio.h>

void print_block(aes_block_t block)
{
    for (size_t i = 0; i < picoutil_aes_block_word_count(block.block_size); ++i)
    {
        GET_ROW(block, i, row);
        for (size_t j = 0; j < sizeof(aes_word_t); ++j)
            printf("%" PRIx8 " ", row[j]);
        puts("");
    }
    puts("");
}

void print_key(aes_key_t key)
{
    for (size_t i = 0; i < picoutil_aes_key_word_count(key.key_size); ++i)
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

#if 0
// Works
void picoutil_test_key_schedule(void)
{
    puts("TESTING KEY SCHEDULE...");
    unsigned char raw_key[] = "Thats my Kung Fu";
    aes_key_t key = { 0 };
    key.key_size = AES_KEY_SIZE_128;
    key.key = picoutil_static_calloc(4, 4);
    if (key.key == NULL)
    {
        printf("Failed to allocate memory\n");
        return;
    }
    memcpy(key.key, raw_key, sizeof(raw_key) - 1);
    aes_key_expanded_t expanded_key = aes_key_schedule(key, AES_BLOCK_SIZE_128);
    if (expanded_key.round_keys == NULL)
    {
        printf("Failed to allocate memory\n");
        picoutil_static_free(key.key);
        return;
    }
    /*
     * Round 0: 54 68 61 74 73 20 6D 79 20 4B 75 6E 67 20 46 75
     * Round 1: E2 32 FC F1 91 12 91 88 B1 59 E4 E6 D6 79 A2 93
     * Round 2: 56 08 20 07 C7 1A B1 8F 76 43 55 69 A0 3A F7 FA
     * Round 3: D2 60 0D E7 15 7A BC 68 63 39 E9 01 C3 03 1E FB
     * Round 4: A1 12 02 C9 B4 68 BE A1 D7 51 57 A0 14 52 49 5B
     * Round 5: B1 29 3B 33 05 41 85 92 D2 10 D2 32 C6 42 9B 69
     * Round 6: BD 3D C2 B7 B8 7C 47 15 6A 6C 95 27 AC 2E 0E 4E
     * Round 7: CC 96 ED 16 74 EA AA 03 1E 86 3F 24 B2 A8 31 6A
     * Round 8: 8E 51 EF 21 FA BB 45 22 E4 3D 7A 06 56 95 4B 6C
     * Round 9: BF E2 BF 90 45 59 FA B2 A1 64 80 B4 F7 F1 CB D8
     * Round 10: 28 FD DE F8 6D A4 24 4A CC C0 A4 FE 3B 31 6F 26
     */
    byte_t expected_keys[11][16] = {
        [0] = { 0x54, 0x68, 0x61, 0x74, 0x73, 0x20, 0x6D, 0x79, 0x20, 0x4B, 0x75, 0x6E, 0x67, 0x20, 0x46, 0x75 },
        [1] = { 0xE2, 0x32, 0xFC, 0xF1, 0x91, 0x12, 0x91, 0x88, 0xB1, 0x59, 0xE4, 0xE6, 0xD6, 0x79, 0xA2, 0x93 },
        [2] = { 0x56, 0x08, 0x20, 0x07, 0xC7, 0x1A, 0xB1, 0x8F, 0x76, 0x43, 0x55, 0x69, 0xA0, 0x3A, 0xF7, 0xFA },
        [3] = { 0xD2, 0x60, 0x0D, 0xE7, 0x15, 0x7A, 0xBC, 0x68, 0x63, 0x39, 0xE9, 0x01, 0xC3, 0x03, 0x1E, 0xFB },
        [4] = { 0xA1, 0x12, 0x02, 0xC9, 0xB4, 0x68, 0xBE, 0xA1, 0xD7, 0x51, 0x57, 0xA0, 0x14, 0x52, 0x49, 0x5B },
        [5] = { 0xB1, 0x29, 0x3B, 0x33, 0x05, 0x41, 0x85, 0x92, 0xD2, 0x10, 0xD2, 0x32, 0xC6, 0x42, 0x9B, 0x69 },
        [6] = { 0xBD, 0x3D, 0xC2, 0xB7, 0xB8, 0x7C, 0x47, 0x15, 0x6A, 0x6C, 0x95, 0x27, 0xAC, 0x2E, 0x0E, 0x4E },
        [7] = { 0xCC, 0x96, 0xED, 0x16, 0x74, 0xEA, 0xAA, 0x03, 0x1E, 0x86, 0x3F, 0x24, 0xB2, 0xA8, 0x31, 0x6A },
        [8] = { 0x8E, 0x51, 0xEF, 0x21, 0xFA, 0xBB, 0x45, 0x22, 0xE4, 0x3D, 0x7A, 0x06, 0x56, 0x95, 0x4B, 0x6C },
        [9] = { 0xBF, 0xE2, 0xBF, 0x90, 0x45, 0x59, 0xFA, 0xB2, 0xA1, 0x64, 0x80, 0xB4, 0xF7, 0xF1, 0xCB, 0xD8 },
        [10] = { 0x28, 0xFD, 0xDE, 0xF8, 0x6D, 0xA4, 0x24, 0x4A, 0xCC, 0xC0, 0xA4, 0xFE, 0x3B, 0x31, 0x6F, 0x26 }
    };
    for (size_t i = 0; i < expanded_key.round_count + 1; ++i)
    {
        printf("Round key %zu: \n", i);
        aes_block_t block = { 0 };
        block.block_size = AES_BLOCK_SIZE_128;
        block.block = expanded_key.round_keys[i];
        print_block(block);
        printf("\n");
        printf("Expected key %zu: \n", i);
        print_buf(expected_keys[i], count_of(expected_keys[i]));
        printf("\n\n");
    }
    aes_key_expanded_free(expanded_key);
    picoutil_static_free(key.key);
}

// Works
void picoutil_test_sub_bytes(void)
{
    puts("TESTING SUB BYTES...");
    unsigned char raw_block[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    aes_block_t block = { 0 };
    block.block_size = AES_BLOCK_SIZE_128;
    block.block = picoutil_static_calloc(4, 4);
    if (block.block == NULL)
    {
        printf("Failed to allocate memory\n");
        return;
    }
    memcpy(block.block, raw_block, sizeof(raw_block));
    block = aes_sub_bytes(block);
    /*
     * SubBytes: 63 CA B7 04 09 53 D0 51 CD 60 E0 E7 BA 70 E1 8C
     */
    byte_t expected_block[16] = { 124, 119, 123, 242, 107, 111, 197, 48, 1, 103, 43, 254, 215, 171, 118, 202 };
    printf("Block: \n");
    print_block(block);
    printf("Expected block: \n");
    print_buf(expected_block, count_of(expected_block));
    puts("");
    picoutil_static_free(block.block);
}

void char_buf_set_rows(byte_t rows[4][4], byte_t* buf)
{
    for (size_t i = 0; i < 4; ++i)
    {
        buf[i] = rows[i][0];
        buf[i + 4] = rows[i][1];
        buf[i + 8] = rows[i][2];
        buf[i + 12] = rows[i][3];
    }

}

void char_buf_set_cols(byte_t cols[4][4], byte_t* buf)
{
    for (size_t i = 0; i < 4; ++i)
    {
        buf[i * 4] = cols[i][0];
        buf[i * 4 + 1] = cols[i][1];
        buf[i * 4 + 2] = cols[i][2];
        buf[i * 4 + 3] = cols[i][3];
    }
}

// Works
void picoutil_test_shift_rows(void)
{
    puts("TESTING SHIFT ROWS...");
    byte_t row1[4] = { 1, 2, 3, 4 };
    byte_t row2[4] = { 5, 6, 7, 8 };
    byte_t row3[4] = { 9, 10, 11, 12 };
    byte_t row4[4] = { 13, 14, 15, 16 };
    unsigned char raw_block[16] = { 0 };
    byte_t rows[4][4] = {
        [0] = {1, 2, 3, 4},
        [1] = {5, 6, 7, 8},
        [2] = {9, 10, 11, 12},
        [3] = {13, 14, 15, 16}
    };
    char_buf_set_rows(rows, raw_block);
    aes_block_t block = { 0 };
    block.block_size = AES_BLOCK_SIZE_128;
    block.block = picoutil_static_calloc(4, 4);
    if (block.block == NULL)
    {
        printf("Failed to allocate memory\n");
        return;
    }
    SET_ROW(block, 0, row1);
    SET_ROW(block, 1, row2);
    SET_ROW(block, 2, row3);
    SET_ROW(block, 3, row4);
    printf("Block (before): \n");
    print_block(block);
    printf("\n");
    printf("Raw block (before): \n");
    print_buf(raw_block, count_of(raw_block));
    printf("\n");
    block = aes_shift_rows(block);
    /*
     * ShiftRows: 1 2 3 4 6 7 8 5 11 12 9 10 16 13 14 15
     */
    byte_t expected_block[16] = { 1, 2, 3, 4, 6, 7, 8, 5, 11, 12, 9, 10, 16, 13, 14, 15 };
    printf("Block: \n");
    print_block(block);
    printf("Expected block: \n");
    print_buf(expected_block, count_of(expected_block));
    puts("");
    picoutil_static_free(block.block);
}

// Works
void picoutil_test_mix_columns(void)
{
    puts("TESTING MIX COLUMNS...");

    byte_t col1[4] = { 219, 19, 83, 69 };
    byte_t col2[4] = { 242, 10, 34, 92 };
    byte_t col3[4] = { 1, 1, 1, 1 };
    byte_t col4[4] = { 198, 198, 198, 198 };

    aes_block_t block = { 0 };
    block.block_size = AES_BLOCK_SIZE_128;
    block.block = picoutil_static_calloc(4, 4);
    if (block.block == NULL)
    {
        printf("Failed to allocate memory\n");
        return;
    }
    SET_COLUMN(block, 0, col1);
    SET_COLUMN(block, 1, col2);
    SET_COLUMN(block, 2, col3);
    SET_COLUMN(block, 3, col4);
    printf("Block (before): \n");
    print_block(block);
    block = aes_mix_columns(block);
    /*
     * MixColumns: 142, 159, 1, 198,  77, 220, 1, 198,  161, 88, 1, 198,  188, 157, 1, 198
     */
    __unused byte_t expected_block[16] = { 142, 159, 1, 198,  77, 220, 1, 198,  161, 88, 1, 198,  188, 157, 1, 198 };
    __unused byte_t expected_cols[4][4] = {
        [0] = { 142, 77, 161, 188 },
        [1] = { 159, 220, 88, 157 },
        [2] = { 1, 1, 1, 1 },
        [3] = { 198, 198, 198, 198 }
    };
    printf("Block: \n");
    print_block(block);
    picoutil_static_free(block.block);
}

#endif

#include "../aes_test/aes.h"

// Doesn't work
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
    block1 = picoutil_aes_encrypt_block_until(block1, key1, num_rounds);
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