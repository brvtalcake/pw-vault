#include <picoutil.h>

#include <pico/sync.h>
// TODO: Maybe configure the hardware interpolator to accelerate the speed of some calculations
/* #include <hardware/interp.h> */

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

typedef struct aes_user_data
{
    aes_block_t* blocks;
    size_t block_count;
} aes_user_data_t;

#ifdef GET_ROW
    #undef GET_ROW
#endif
#define GET_ROW(BLOCK, ROW, OUTPUT)                                             \
    picoutil_memset_explicit(OUTPUT, 0, sizeof(OUTPUT));                        \
    __extension__                                                               \
    ({                                                                          \
        byte_t temp[picoutil_aes_block_word_count((BLOCK).block_size)];         \
        for ( size_t UNIQUE(i) = 0;                                             \
              UNIQUE(i) < picoutil_aes_block_word_count((BLOCK).block_size);    \
              ++UNIQUE(i))                                                      \
            temp[UNIQUE(i)] = ((byte_t*)&((BLOCK).block[UNIQUE(i)]))[(ROW)];    \
        memcpy((OUTPUT), temp, sizeof(OUTPUT));                                 \
    })

/*     byte_t OUTPUT[picoutil_aes_block_word_count((BLOCK).block_size)];           \ */

#ifdef SET_ROW
    #undef SET_ROW
#endif
#define SET_ROW(BLOCK, ROW, INPUT)                                              \
    __extension__                                                               \
    ({                                                                          \
        byte_t temp[picoutil_aes_block_word_count((BLOCK).block_size)];         \
        picoutil_memset_explicit(temp, 0, sizeof(temp));                        \
        memcpy(temp, (INPUT), sizeof(temp));                                    \
        for ( size_t UNIQUE(i) = 0;                                             \
              UNIQUE(i) < picoutil_aes_block_word_count((BLOCK).block_size);    \
              ++UNIQUE(i))                                                      \
            ((byte_t*)&((BLOCK).block[UNIQUE(i)]))[(ROW)] = temp[UNIQUE(i)];    \
    })

#ifdef GET_COLUMN
    #undef GET_COLUMN
#endif
#define GET_COLUMN(BLOCK, COLUMN, OUTPUT)                                       \
    byte_t OUTPUT[sizeof(aes_word_t)] = { 0 };                                  \
    __extension__                                                               \
    ({                                                                          \
        byte_t temp[sizeof(aes_word_t)] = { 0 };                                \
        for (size_t UNIQUE(i) = 0; UNIQUE(i) < sizeof(aes_word_t); ++UNIQUE(i)) \
            temp[UNIQUE(i)] = ((byte_t*)&((BLOCK).block[(COLUMN)]))[UNIQUE(i)]; \
        memcpy((OUTPUT), temp, sizeof(aes_word_t));                             \
    })

/*     byte_t OUTPUT[sizeof(aes_word_t)] = { 0 };                      \
    __extension__                                                   \
    ({                                                              \
        byte_t temp[sizeof(aes_word_t)] = { 0 };                    \
        temp[0] = ((byte_t*)&((BLOCK).block[(COLUMN)]))[0];         \
        temp[1] = ((byte_t*)&((BLOCK).block[(COLUMN)]))[1];         \
        temp[2] = ((byte_t*)&((BLOCK).block[(COLUMN)]))[2];         \
        temp[3] = ((byte_t*)&((BLOCK).block[(COLUMN)]))[3];         \
        memcpy((OUTPUT), temp, sizeof(aes_word_t));                 \
    })

 */
#ifdef SET_COLUMN
    #undef SET_COLUMN
#endif
#define SET_COLUMN(BLOCK, COLUMN, INPUT)                                        \
    __extension__                                                               \
    ({                                                                          \
        byte_t temp[sizeof(aes_word_t)] = { 0 };                                \
        memcpy(temp, (INPUT), sizeof(aes_word_t));                              \
        for (size_t UNIQUE(i) = 0; UNIQUE(i) < sizeof(aes_word_t); ++UNIQUE(i)) \
            ((byte_t*)&((BLOCK).block[(COLUMN)]))[UNIQUE(i)] = temp[UNIQUE(i)]; \
    })

/*     __extension__                                                   \
    ({                                                              \
        byte_t temp[sizeof(aes_word_t)] = { 0 };                    \
        memcpy(temp, (INPUT), sizeof(aes_word_t));                  \
        ((byte_t*)&((BLOCK).block[(COLUMN)]))[0] = temp[0];         \
        ((byte_t*)&((BLOCK).block[(COLUMN)]))[1] = temp[1];         \
        ((byte_t*)&((BLOCK).block[(COLUMN)]))[2] = temp[2];         \
        ((byte_t*)&((BLOCK).block[(COLUMN)]))[3] = temp[3];         \
    }) */

void picoutil_aes_init(void)
{
    picoutil_static_allocator_init(true);
}

__const
static byte_t aes_galois_table_1(byte_t a)
{
    return a;
}

// To multiply by 2
__const
static byte_t aes_galois_table_2(byte_t a)
{
    static const byte_t table[256] = {
        0x00, 0x02, 0x04, 0x06,
        0x08, 0x0a, 0x0c, 0x0e,
        0x10, 0x12, 0x14, 0x16,
        0x18, 0x1a, 0x1c, 0x1e,
        0x20, 0x22, 0x24, 0x26,
        0x28, 0x2a, 0x2c, 0x2e,
        0x30, 0x32, 0x34, 0x36,
        0x38, 0x3a, 0x3c, 0x3e,
        0x40, 0x42, 0x44, 0x46,
        0x48, 0x4a, 0x4c, 0x4e,
        0x50, 0x52, 0x54, 0x56,
        0x58, 0x5a, 0x5c, 0x5e,
        0x60, 0x62, 0x64, 0x66,
        0x68, 0x6a, 0x6c, 0x6e,
        0x70, 0x72, 0x74, 0x76,
        0x78, 0x7a, 0x7c, 0x7e,	
        0x80, 0x82, 0x84, 0x86,
        0x88, 0x8a, 0x8c, 0x8e,
        0x90, 0x92, 0x94, 0x96,
        0x98, 0x9a, 0x9c, 0x9e,
        0xa0, 0xa2, 0xa4, 0xa6,
        0xa8, 0xaa, 0xac, 0xae,
        0xb0, 0xb2, 0xb4, 0xb6,
        0xb8, 0xba, 0xbc, 0xbe,
        0xc0, 0xc2, 0xc4, 0xc6,
        0xc8, 0xca, 0xcc, 0xce,
        0xd0, 0xd2, 0xd4, 0xd6,
        0xd8, 0xda, 0xdc, 0xde,
        0xe0, 0xe2, 0xe4, 0xe6,
        0xe8, 0xea, 0xec, 0xee,
        0xf0, 0xf2, 0xf4, 0xf6,
        0xf8, 0xfa, 0xfc, 0xfe,
        0x1b, 0x19, 0x1f, 0x1d,
        0x13, 0x11, 0x17, 0x15,
        0x0b, 0x09, 0x0f, 0x0d,
        0x03, 0x01, 0x07, 0x05,
        0x3b, 0x39, 0x3f, 0x3d,
        0x33, 0x31, 0x37, 0x35,
        0x2b, 0x29, 0x2f, 0x2d,
        0x23, 0x21, 0x27, 0x25,
        0x5b, 0x59, 0x5f, 0x5d,
        0x53, 0x51, 0x57, 0x55,
        0x4b, 0x49, 0x4f, 0x4d,
        0x43, 0x41, 0x47, 0x45,
        0x7b, 0x79, 0x7f, 0x7d,
        0x73, 0x71, 0x77, 0x75,
        0x6b, 0x69, 0x6f, 0x6d,
        0x63, 0x61, 0x67, 0x65,
        0x9b, 0x99, 0x9f, 0x9d,
        0x93, 0x91, 0x97, 0x95,
        0x8b, 0x89, 0x8f, 0x8d,
        0x83, 0x81, 0x87, 0x85,
        0xbb, 0xb9, 0xbf, 0xbd,
        0xb3, 0xb1, 0xb7, 0xb5,
        0xab, 0xa9, 0xaf, 0xad,
        0xa3, 0xa1, 0xa7, 0xa5,
        0xdb, 0xd9, 0xdf, 0xdd,
        0xd3, 0xd1, 0xd7, 0xd5,
        0xcb, 0xc9, 0xcf, 0xcd,
        0xc3, 0xc1, 0xc7, 0xc5,
        0xfb, 0xf9, 0xff, 0xfd,
        0xf3, 0xf1, 0xf7, 0xf5,
        0xeb, 0xe9, 0xef, 0xed,
        0xe3, 0xe1, 0xe7, 0xe5
    };
    return table[a];
}

__const
static byte_t aes_galois_table_3(byte_t a)
{
    static const byte_t table[256] = {
        0x00, 0x03, 0x06, 0x05,
        0x0c, 0x0f, 0x0a, 0x09,
        0x18, 0x1b, 0x1e, 0x1d,
        0x14, 0x17, 0x12, 0x11,
        0x30, 0x33, 0x36, 0x35,
        0x3c, 0x3f, 0x3a, 0x39,
        0x28, 0x2b, 0x2e, 0x2d,
        0x24, 0x27, 0x22, 0x21,
        0x60, 0x63, 0x66, 0x65,
        0x6c, 0x6f, 0x6a, 0x69,
        0x78, 0x7b, 0x7e, 0x7d,
        0x74, 0x77, 0x72, 0x71,
        0x50, 0x53, 0x56, 0x55,
        0x5c, 0x5f, 0x5a, 0x59,
        0x48, 0x4b, 0x4e, 0x4d,
        0x44, 0x47, 0x42, 0x41,
        0xc0, 0xc3, 0xc6, 0xc5,
        0xcc, 0xcf, 0xca, 0xc9,
        0xd8, 0xdb, 0xde, 0xdd,
        0xd4, 0xd7, 0xd2, 0xd1,
        0xf0, 0xf3, 0xf6, 0xf5,
        0xfc, 0xff, 0xfa, 0xf9,
        0xe8, 0xeb, 0xee, 0xed,
        0xe4, 0xe7, 0xe2, 0xe1,
        0xa0, 0xa3, 0xa6, 0xa5,
        0xac, 0xaf, 0xaa, 0xa9,
        0xb8, 0xbb, 0xbe, 0xbd,
        0xb4, 0xb7, 0xb2, 0xb1,
        0x90, 0x93, 0x96, 0x95,
        0x9c, 0x9f, 0x9a, 0x99,
        0x88, 0x8b, 0x8e, 0x8d,
        0x84, 0x87, 0x82, 0x81,	
        0x9b, 0x98, 0x9d, 0x9e,
        0x97, 0x94, 0x91, 0x92,
        0x83, 0x80, 0x85, 0x86,
        0x8f, 0x8c, 0x89, 0x8a,
        0xab, 0xa8, 0xad, 0xae,
        0xa7, 0xa4, 0xa1, 0xa2,
        0xb3, 0xb0, 0xb5, 0xb6,
        0xbf, 0xbc, 0xb9, 0xba,
        0xfb, 0xf8, 0xfd, 0xfe,
        0xf7, 0xf4, 0xf1, 0xf2,
        0xe3, 0xe0, 0xe5, 0xe6,
        0xef, 0xec, 0xe9, 0xea,	
        0xcb, 0xc8, 0xcd, 0xce,
        0xc7, 0xc4, 0xc1, 0xc2,
        0xd3, 0xd0, 0xd5, 0xd6,
        0xdf, 0xdc, 0xd9, 0xda,	
        0x5b, 0x58, 0x5d, 0x5e,
        0x57, 0x54, 0x51, 0x52,
        0x43, 0x40, 0x45, 0x46,
        0x4f, 0x4c, 0x49, 0x4a,
        0x6b, 0x68, 0x6d, 0x6e,
        0x67, 0x64, 0x61, 0x62,
        0x73, 0x70, 0x75, 0x76,
        0x7f, 0x7c, 0x79, 0x7a,	
        0x3b, 0x38, 0x3d, 0x3e,
        0x37, 0x34, 0x31, 0x32,
        0x23, 0x20, 0x25, 0x26,
        0x2f, 0x2c, 0x29, 0x2a,
        0x0b, 0x08, 0x0d, 0x0e,
        0x07, 0x04, 0x01, 0x02,
        0x13, 0x10, 0x15, 0x16,
        0x1f, 0x1c, 0x19, 0x1a
    };
    return table[a];
}

__const
static byte_t aes_galois_table_9(byte_t a)
{
    static const byte_t table[256] = {
        0x00, 0x09, 0x12, 0x1b,
        0x24, 0x2d, 0x36, 0x3f,
        0x48, 0x41, 0x5a, 0x53,
        0x6c, 0x65, 0x7e, 0x77,
        0x90, 0x99, 0x82, 0x8b,
        0xb4, 0xbd, 0xa6, 0xaf,
        0xd8, 0xd1, 0xca, 0xc3,
        0xfc, 0xf5, 0xee, 0xe7,
        0x3b, 0x32, 0x29, 0x20,
        0x1f, 0x16, 0x0d, 0x04,
        0x73, 0x7a, 0x61, 0x68,
        0x57, 0x5e, 0x45, 0x4c,
        0xab, 0xa2, 0xb9, 0xb0,
        0x8f, 0x86, 0x9d, 0x94,
        0xe3, 0xea, 0xf1, 0xf8,
        0xc7, 0xce, 0xd5, 0xdc,
        0x76, 0x7f, 0x64, 0x6d,
        0x52, 0x5b, 0x40, 0x49,
        0x3e, 0x37, 0x2c, 0x25,
        0x1a, 0x13, 0x08, 0x01,
        0xe6, 0xef, 0xf4, 0xfd,
        0xc2, 0xcb, 0xd0, 0xd9,
        0xae, 0xa7, 0xbc, 0xb5,
        0x8a, 0x83, 0x98, 0x91,
        0x4d, 0x44, 0x5f, 0x56,
        0x69, 0x60, 0x7b, 0x72,
        0x05, 0x0c, 0x17, 0x1e,
        0x21, 0x28, 0x33, 0x3a,
        0xdd, 0xd4, 0xcf, 0xc6,
        0xf9, 0xf0, 0xeb, 0xe2,
        0x95, 0x9c, 0x87, 0x8e,
        0xb1, 0xb8, 0xa3, 0xaa,	
        0xec, 0xe5, 0xfe, 0xf7,
        0xc8, 0xc1, 0xda, 0xd3,
        0xa4, 0xad, 0xb6, 0xbf,
        0x80, 0x89, 0x92, 0x9b,	
        0x7c, 0x75, 0x6e, 0x67,
        0x58, 0x51, 0x4a, 0x43,
        0x34, 0x3d, 0x26, 0x2f,
        0x10, 0x19, 0x02, 0x0b,
        0xd7, 0xde, 0xc5, 0xcc,
        0xf3, 0xfa, 0xe1, 0xe8,
        0x9f, 0x96, 0x8d, 0x84,
        0xbb, 0xb2, 0xa9, 0xa0,
        0x47, 0x4e, 0x55, 0x5c,
        0x63, 0x6a, 0x71, 0x78,
        0x0f, 0x06, 0x1d, 0x14,
        0x2b, 0x22, 0x39, 0x30,
        0x9a, 0x93, 0x88, 0x81,
        0xbe, 0xb7, 0xac, 0xa5,
        0xd2, 0xdb, 0xc0, 0xc9,
        0xf6, 0xff, 0xe4, 0xed,
        0x0a, 0x03, 0x18, 0x11,
        0x2e, 0x27, 0x3c, 0x35,
        0x42, 0x4b, 0x50, 0x59,
        0x66, 0x6f, 0x74, 0x7d,	
        0xa1, 0xa8, 0xb3, 0xba,
        0x85, 0x8c, 0x97, 0x9e,
        0xe9, 0xe0, 0xfb, 0xf2,
        0xcd, 0xc4, 0xdf, 0xd6,
        0x31, 0x38, 0x23, 0x2a,
        0x15, 0x1c, 0x07, 0x0e,
        0x79, 0x70, 0x6b, 0x62,
        0x5d, 0x54, 0x4f, 0x46
    };
    return table[a];
}

__const
static byte_t aes_galois_table_11(byte_t a)
{
    static const byte_t table[256] = {
        0x00, 0x0b, 0x16, 0x1d,
        0x2c, 0x27, 0x3a, 0x31,
        0x58, 0x53, 0x4e, 0x45,
        0x74, 0x7f, 0x62, 0x69,
        0xb0, 0xbb, 0xa6, 0xad,
        0x9c, 0x97, 0x8a, 0x81,
        0xe8, 0xe3, 0xfe, 0xf5,
        0xc4, 0xcf, 0xd2, 0xd9,
        0x7b, 0x70, 0x6d, 0x66,
        0x57, 0x5c, 0x41, 0x4a,
        0x23, 0x28, 0x35, 0x3e,
        0x0f, 0x04, 0x19, 0x12,
        0xcb, 0xc0, 0xdd, 0xd6,
        0xe7, 0xec, 0xf1, 0xfa,
        0x93, 0x98, 0x85, 0x8e,
        0xbf, 0xb4, 0xa9, 0xa2,
        0xf6, 0xfd, 0xe0, 0xeb,
        0xda, 0xd1, 0xcc, 0xc7,
        0xae, 0xa5, 0xb8, 0xb3,
        0x82, 0x89, 0x94, 0x9f,
        0x46, 0x4d, 0x50, 0x5b,
        0x6a, 0x61, 0x7c, 0x77,
        0x1e, 0x15, 0x08, 0x03,
        0x32, 0x39, 0x24, 0x2f,
        0x8d, 0x86, 0x9b, 0x90,
        0xa1, 0xaa, 0xb7, 0xbc,
        0xd5, 0xde, 0xc3, 0xc8,
        0xf9, 0xf2, 0xef, 0xe4,
        0x3d, 0x36, 0x2b, 0x20,
        0x11, 0x1a, 0x07, 0x0c,
        0x65, 0x6e, 0x73, 0x78,
        0x49, 0x42, 0x5f, 0x54,
        0xf7, 0xfc, 0xe1, 0xea,
        0xdb, 0xd0, 0xcd, 0xc6,
        0xaf, 0xa4, 0xb9, 0xb2,
        0x83, 0x88, 0x95, 0x9e,
        0x47, 0x4c, 0x51, 0x5a,
        0x6b, 0x60, 0x7d, 0x76,
        0x1f, 0x14, 0x09, 0x02,
        0x33, 0x38, 0x25, 0x2e,
        0x8c, 0x87, 0x9a, 0x91,
        0xa0, 0xab, 0xb6, 0xbd,
        0xd4, 0xdf, 0xc2, 0xc9,
        0xf8, 0xf3, 0xee, 0xe5,
        0x3c, 0x37, 0x2a, 0x21,
        0x10, 0x1b, 0x06, 0x0d,
        0x64, 0x6f, 0x72, 0x79,
        0x48, 0x43, 0x5e, 0x55,
        0x01, 0x0a, 0x17, 0x1c,
        0x2d, 0x26, 0x3b, 0x30,
        0x59, 0x52, 0x4f, 0x44,
        0x75, 0x7e, 0x63, 0x68,
        0xb1, 0xba, 0xa7, 0xac,
        0x9d, 0x96, 0x8b, 0x80,
        0xe9, 0xe2, 0xff, 0xf4,
        0xc5, 0xce, 0xd3, 0xd8,
        0x7a, 0x71, 0x6c, 0x67,
        0x56, 0x5d, 0x40, 0x4b,
        0x22, 0x29, 0x34, 0x3f,
        0x0e, 0x05, 0x18, 0x13,
        0xca, 0xc1, 0xdc, 0xd7,
        0xe6, 0xed, 0xf0, 0xfb,
        0x92, 0x99, 0x84, 0x8f,
        0xbe, 0xb5, 0xa8, 0xa3
    };
    return table[a];
}

__const
static byte_t aes_galois_table_13(byte_t a)
{
    static const byte_t table[256] = {
        0x00, 0x0d, 0x1a, 0x17,
        0x34, 0x39, 0x2e, 0x23,
        0x68, 0x65, 0x72, 0x7f,
        0x5c, 0x51, 0x46, 0x4b,
        0xd0, 0xdd, 0xca, 0xc7,
        0xe4, 0xe9, 0xfe, 0xf3,
        0xb8, 0xb5, 0xa2, 0xaf,
        0x8c, 0x81, 0x96, 0x9b,
        0xbb, 0xb6, 0xa1, 0xac,
        0x8f, 0x82, 0x95, 0x98,
        0xd3, 0xde, 0xc9, 0xc4,
        0xe7, 0xea, 0xfd, 0xf0,
        0x6b, 0x66, 0x71, 0x7c,
        0x5f, 0x52, 0x45, 0x48,
        0x03, 0x0e, 0x19, 0x14,
        0x37, 0x3a, 0x2d, 0x20,
        0x6d, 0x60, 0x77, 0x7a,
        0x59, 0x54, 0x43, 0x4e,
        0x05, 0x08, 0x1f, 0x12,
        0x31, 0x3c, 0x2b, 0x26,
        0xbd, 0xb0, 0xa7, 0xaa,
        0x89, 0x84, 0x93, 0x9e,
        0xd5, 0xd8, 0xcf, 0xc2,
        0xe1, 0xec, 0xfb, 0xf6,
        0xd6, 0xdb, 0xcc, 0xc1,
        0xe2, 0xef, 0xf8, 0xf5,
        0xbe, 0xb3, 0xa4, 0xa9,
        0x8a, 0x87, 0x90, 0x9d,
        0x06, 0x0b, 0x1c, 0x11,
        0x32, 0x3f, 0x28, 0x25,
        0x6e, 0x63, 0x74, 0x79,
        0x5a, 0x57, 0x40, 0x4d,
        0xda, 0xd7, 0xc0, 0xcd,
        0xee, 0xe3, 0xf4, 0xf9,
        0xb2, 0xbf, 0xa8, 0xa5,
        0x86, 0x8b, 0x9c, 0x91,
        0x0a, 0x07, 0x10, 0x1d,
        0x3e, 0x33, 0x24, 0x29,
        0x62, 0x6f, 0x78, 0x75,
        0x56, 0x5b, 0x4c, 0x41,
        0x61, 0x6c, 0x7b, 0x76,
        0x55, 0x58, 0x4f, 0x42,
        0x09, 0x04, 0x13, 0x1e,
        0x3d, 0x30, 0x27, 0x2a,
        0xb1, 0xbc, 0xab, 0xa6,
        0x85, 0x88, 0x9f, 0x92,
        0xd9, 0xd4, 0xc3, 0xce,
        0xed, 0xe0, 0xf7, 0xfa,
        0xb7, 0xba, 0xad, 0xa0,
        0x83, 0x8e, 0x99, 0x94,
        0xdf, 0xd2, 0xc5, 0xc8,
        0xeb, 0xe6, 0xf1, 0xfc,
        0x67, 0x6a, 0x7d, 0x70,
        0x53, 0x5e, 0x49, 0x44,
        0x0f, 0x02, 0x15, 0x18,
        0x3b, 0x36, 0x21, 0x2c,
        0x0c, 0x01, 0x16, 0x1b,
        0x38, 0x35, 0x22, 0x2f,
        0x64, 0x69, 0x7e, 0x73,
        0x50, 0x5d, 0x4a, 0x47,
        0xdc, 0xd1, 0xc6, 0xcb,
        0xe8, 0xe5, 0xf2, 0xff,
        0xb4, 0xb9, 0xae, 0xa3,
        0x80, 0x8d, 0x9a, 0x97
    };
    return table[a];
}

__const
static byte_t aes_galois_table_14(byte_t a)
{
    static const byte_t table[256] = {
        0x00, 0x0e, 0x1c, 0x12,
        0x38, 0x36, 0x24, 0x2a,
        0x70, 0x7e, 0x6c, 0x62,
        0x48, 0x46, 0x54, 0x5a,
        0xe0, 0xee, 0xfc, 0xf2,
        0xd8, 0xd6, 0xc4, 0xca,
        0x90, 0x9e, 0x8c, 0x82,
        0xa8, 0xa6, 0xb4, 0xba,
        0xdb, 0xd5, 0xc7, 0xc9,
        0xe3, 0xed, 0xff, 0xf1,
        0xab, 0xa5, 0xb7, 0xb9,
        0x93, 0x9d, 0x8f, 0x81,
        0x3b, 0x35, 0x27, 0x29,
        0x03, 0x0d, 0x1f, 0x11,
        0x4b, 0x45, 0x57, 0x59,
        0x73, 0x7d, 0x6f, 0x61,
        0xad, 0xa3, 0xb1, 0xbf,
        0x95, 0x9b, 0x89, 0x87,
        0xdd, 0xd3, 0xc1, 0xcf,
        0xe5, 0xeb, 0xf9, 0xf7,
        0x4d, 0x43, 0x51, 0x5f,
        0x75, 0x7b, 0x69, 0x67,
        0x3d, 0x33, 0x21, 0x2f,
        0x05, 0x0b, 0x19, 0x17,
        0x76, 0x78, 0x6a, 0x64,
        0x4e, 0x40, 0x52, 0x5c,
        0x06, 0x08, 0x1a, 0x14,
        0x3e, 0x30, 0x22, 0x2c,
        0x96, 0x98, 0x8a, 0x84,
        0xae, 0xa0, 0xb2, 0xbc,
        0xe6, 0xe8, 0xfa, 0xf4,
        0xde, 0xd0, 0xc2, 0xcc,
        0x41, 0x4f, 0x5d, 0x53,
        0x79, 0x77, 0x65, 0x6b,
        0x31, 0x3f, 0x2d, 0x23,
        0x09, 0x07, 0x15, 0x1b,
        0xa1, 0xaf, 0xbd, 0xb3,
        0x99, 0x97, 0x85, 0x8b,
        0xd1, 0xdf, 0xcd, 0xc3,
        0xe9, 0xe7, 0xf5, 0xfb,
        0x9a, 0x94, 0x86, 0x88,
        0xa2, 0xac, 0xbe, 0xb0,
        0xea, 0xe4, 0xf6, 0xf8,
        0xd2, 0xdc, 0xce, 0xc0,
        0x7a, 0x74, 0x66, 0x68,
        0x42, 0x4c, 0x5e, 0x50,
        0x0a, 0x04, 0x16, 0x18,
        0x32, 0x3c, 0x2e, 0x20,
        0xec, 0xe2, 0xf0, 0xfe,
        0xd4, 0xda, 0xc8, 0xc6,
        0x9c, 0x92, 0x80, 0x8e,
        0xa4, 0xaa, 0xb8, 0xb6,
        0x0c, 0x02, 0x10, 0x1e,
        0x34, 0x3a, 0x28, 0x26,
        0x7c, 0x72, 0x60, 0x6e,
        0x44, 0x4a, 0x58, 0x56,
        0x37, 0x39, 0x2b, 0x25,
        0x0f, 0x01, 0x13, 0x1d,
        0x47, 0x49, 0x5b, 0x55,
        0x7f, 0x71, 0x63, 0x6d,
        0xd7, 0xd9, 0xcb, 0xc5,
        0xef, 0xe1, 0xf3, 0xfd,
        0xa7, 0xa9, 0xbb, 0xb5,
        0x9f, 0x91, 0x83, 0x8d
    };
    return table[a];
}

__const
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

__const
static inline aes_word_t aes_round_constant(uint8_t round)
{
    byte_t rcon[4] = { aes_round_constant_calc_rc(round), 0, 0, 0 };
    return *(aes_word_t*)rcon;
}

__const
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

__const
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

__const __flatten
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

__const __flatten
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
__const __flatten
static inline aes_word_t aes_rot_word_left(aes_word_t x)
{
    return ROTATE(LEFT, x, 8);
}

// Rotate the bytes in a 32-bit word right by one
__const __flatten
static inline aes_word_t aes_rot_word_right(aes_word_t x)
{
    return ROTATE(RIGHT, x, 8);
}

__const __always_inline __flatten
static inline size_t max_between(size_t a, size_t b)
{
    return a > b ? a : b;
}

static aes_key_expanded_t aes_key_schedule(aes_key_t key, ATTRIBUTE(unused) aes_block_size bsize)
{
    picoutil_static_allocator_set_safe(true);
    
    const aes_word_t N = picoutil_aes_key_word_count(key.key_size);
    // In 32-bit words (aes_word_t)
    const aes_word_t BSIZE = picoutil_aes_block_word_count(bsize);
    const ptrdiff_t R_ = picoutil_aes_round_count(key.key_size, bsize);
    static const size_t MAX_RND_COUNT = 14;
    static const size_t MAX_BLOCK_WORD_CNT = 256 / (sizeof(aes_word_t) * 8); // = 8
    if (R_ < 0 || BSIZE * R_ > MAX_BLOCK_WORD_CNT * (MAX_RND_COUNT + 1)) // Sanity checks to avoid stack overflow
        return NULL_T(aes_key_expanded_t);
    const size_t R = (size_t)R_ + 1;

    aes_word_t full_expanded_key[BSIZE * R];
    picoutil_memset_explicit(full_expanded_key, 0, sizeof(full_expanded_key));

    for (size_t i = 0; i < BSIZE * R; ++i)
    {
        divmod_result_t divmod = DIVMODu32(i, N);
        if (i < N)
            full_expanded_key[i] = key.key[i];
        else if (to_remainder_u32(divmod) == 0) // && i >= N
            full_expanded_key[i] = full_expanded_key[i - N]                 ^
                aes_sub_word(aes_rot_word_right(full_expanded_key[i - 1]))  ^
                aes_round_constant(to_quotient_u32(divmod));
        else if (N > 6 && to_remainder_u32(divmod) == 4) // && i >= N
            full_expanded_key[i] = full_expanded_key[i - N] ^ aes_sub_word(full_expanded_key[i - 1]);
        else // if (N <= 6 || to_remainder_u32(divmod) != 4) // && i >= N
            full_expanded_key[i] = full_expanded_key[i - N] ^ full_expanded_key[i - 1];
    }
    aes_key_expanded_t expanded_key = { .round_keys = picoutil_static_calloc_aligned(R, sizeof(aes_word_t*), alignof(aes_word_t*)) };
    if (expanded_key.round_keys == NULL)
        return (aes_key_expanded_t){ 0 };
    for (size_t i = 0; i < R; ++i)
    {
        expanded_key.round_keys[i] = picoutil_static_calloc_aligned(BSIZE, sizeof(aes_word_t), alignof(aes_word_t));
        if (expanded_key.round_keys[i] == NULL)
        {
            for (size_t j = 0; j < i; ++j)
                picoutil_static_free(expanded_key.round_keys[j]);
            picoutil_static_free(expanded_key.round_keys);
            return (aes_key_expanded_t){ 0 };
        }
        memcpy(expanded_key.round_keys[i], full_expanded_key + (i * BSIZE), BSIZE * sizeof(aes_word_t));
    }
    expanded_key.round_count = R - 1;
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

// The first implementation is taken from Wikipedia example implementation
// https://en.wikipedia.org/wiki/Rijndael_MixColumns#Implementation_example (in C, with the C# version below it)
// The second implementation is based on table lookups which may be better since it's less exposed to timing attacks (I guess)
// It is however less memory efficient since we need to store 256 * 6 bytes for the lookup tables
static void aes_mul_column_by_poly(byte_t bytes[__min_size (sizeof(aes_word_t))] /* r */)
{
#if 0
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
#else
    // Use table lookups for faster multiplication
    __unused static const byte_t poly[sizeof(aes_word_t)] = { 0x02, 0x01, 0x01, 0x03 };
    byte_t temp[sizeof(aes_word_t)] = { 0 };

    // temp[0] = 2*bytes[0] + 3*bytes[1] + 1*bytes[2] + 1*bytes[3];
    temp[0] = aes_galois_table_2(bytes[0]) ^ aes_galois_table_3(bytes[1]) ^ aes_galois_table_1(bytes[2]) ^ aes_galois_table_1(bytes[3]);
    // temp[1] = 1*bytes[0] + 2*bytes[1] + 3*bytes[2] + 1*bytes[3];
    temp[1] = aes_galois_table_1(bytes[0]) ^ aes_galois_table_2(bytes[1]) ^ aes_galois_table_3(bytes[2]) ^ aes_galois_table_1(bytes[3]);
    // temp[2] = 1*bytes[0] + 1*bytes[1] + 2*bytes[2] + 3*bytes[3];
    temp[2] = aes_galois_table_1(bytes[0]) ^ aes_galois_table_1(bytes[1]) ^ aes_galois_table_2(bytes[2]) ^ aes_galois_table_3(bytes[3]);
    // temp[3] = 3*bytes[0] + 1*bytes[1] + 1*bytes[2] + 2*bytes[3];
    temp[3] = aes_galois_table_3(bytes[0]) ^ aes_galois_table_1(bytes[1]) ^ aes_galois_table_1(bytes[2]) ^ aes_galois_table_2(bytes[3]);

    memcpy(bytes, temp, sizeof(aes_word_t));
#endif
}

static void aes_inv_mul_column_by_poly(byte_t bytes[__min_size (sizeof(aes_word_t))] /* r */)
{
    __unused static const byte_t poly[sizeof(aes_word_t)] = { 0x0e, 0x09, 0x0d, 0x0b };
    byte_t temp[sizeof(aes_word_t)] = { 0 };
    
    // temp[0] = 14*bytes[0] + 11*bytes[1] + 13*bytes[2] + 9*bytes[3];
    temp[0] = aes_galois_table_14(bytes[0]) ^ aes_galois_table_11(bytes[1]) ^ aes_galois_table_13(bytes[2]) ^ aes_galois_table_9(bytes[3]);
    // temp[1] = 9*bytes[0] + 14*bytes[1] + 11*bytes[2] + 13*bytes[3];
    temp[1] = aes_galois_table_9(bytes[0]) ^ aes_galois_table_14(bytes[1]) ^ aes_galois_table_11(bytes[2]) ^ aes_galois_table_13(bytes[3]);
    // temp[2] = 13*bytes[0] + 9*bytes[1] + 14*bytes[2] + 11*bytes[3];
    temp[2] = aes_galois_table_13(bytes[0]) ^ aes_galois_table_9(bytes[1]) ^ aes_galois_table_14(bytes[2]) ^ aes_galois_table_11(bytes[3]);
    // temp[3] = 11*bytes[0] + 13*bytes[1] + 9*bytes[2] + 14*bytes[3];
    temp[3] = aes_galois_table_11(bytes[0]) ^ aes_galois_table_13(bytes[1]) ^ aes_galois_table_9(bytes[2]) ^ aes_galois_table_14(bytes[3]);

    memcpy(bytes, temp, sizeof(aes_word_t));
}

// Substitute bytes in a block using the AES S-box
__flatten
static inline aes_block_t aes_sub_bytes(aes_block_t block)
{
    for (size_t i = 0; i < picoutil_aes_block_word_count(block.block_size); ++i)
        block.block[i] = aes_sub_word(block.block[i]);
    return block;
}

__flatten
static inline aes_block_t aes_inv_sub_bytes(aes_block_t block)
{
    for (size_t i = 0; i < picoutil_aes_block_word_count(block.block_size); ++i)
        block.block[i] = aes_inv_sub_word(block.block[i]);
    return block;
}

__flatten
static inline void aes_shift_row_unit(byte_t bytes[__min_size (4)], size_t len)
{
    if (len == 4)
    {
        aes_word_t temp = aes_rot_word_right(*(aes_word_t*)bytes);
        memcpy(bytes, &temp, sizeof(aes_word_t));
    }
    else
    {
        byte_t temparr[len];
        memset(temparr, 0, sizeof(temparr));
        temparr[0] = bytes[1];
        temparr[1] = bytes[2];
        temparr[2] = bytes[3];
        temparr[3] = bytes[0];
        bytes[0] = temparr[0];
        bytes[1] = temparr[1];
        bytes[2] = temparr[2];
        bytes[3] = temparr[3];
    }
}

__flatten
static inline void aes_inv_shift_row_unit(byte_t bytes[__min_size (4)], size_t len)
{
    if (len == 4)
    {
        aes_word_t temp = aes_rot_word_left(*(aes_word_t*)bytes);
        memcpy(bytes, &temp, sizeof(aes_word_t));
    }
    else
    {
        byte_t temparr[len];
        memset(temparr, 0, sizeof(temparr));
        temparr[0] = bytes[3];
        temparr[1] = bytes[0];
        temparr[2] = bytes[1];
        temparr[3] = bytes[2];
        bytes[0] = temparr[0];
        bytes[1] = temparr[1];
        bytes[2] = temparr[2];
        bytes[3] = temparr[3];
    }
}

__flatten
static inline aes_block_t aes_shift_rows(aes_block_t block)
{
    int wc = picoutil_aes_block_word_count(block.block_size);
    if (UNLIKELY(wc < 4) || UNLIKELY(wc > 8))
        return NULL_T(aes_block_t);
    byte_t full_row[wc];
    memset(full_row, 0, sizeof(full_row));
    byte_t row[wc];
    memset(row, 0, sizeof(row));
    for (size_t i = 0; i < sizeof(aes_word_t); ++i)
    {
        GET_ROW(block, i, row);
        memcpy(&full_row, row, picoutil_aes_block_word_count(block.block_size));
        for (size_t j = 0; j < i; ++j)
            aes_shift_row_unit(full_row, picoutil_aes_block_word_count(block.block_size));
        memcpy(row, &full_row, picoutil_aes_block_word_count(block.block_size));
        SET_ROW(block, i, row);
        memset(full_row, 0, picoutil_aes_block_word_count(block.block_size));
    }
    return block;
}

__flatten
static inline aes_block_t aes_inv_shift_rows(aes_block_t block)
{
    int wc = picoutil_aes_block_word_count(block.block_size);
    if (UNLIKELY(wc < 4) || UNLIKELY(wc > 8))
        return NULL_T(aes_block_t);
    byte_t full_row[wc];
    memset(full_row, 0, sizeof(full_row));
    byte_t row[wc];
    memset(row, 0, sizeof(row));
    for (size_t i = 0; i < sizeof(aes_word_t); ++i)
    {
        GET_ROW(block, i, row);
        memcpy(&full_row, row, picoutil_aes_block_word_count(block.block_size));
        for (size_t j = 0; j < i; ++j)
            aes_inv_shift_row_unit(full_row, picoutil_aes_block_word_count(block.block_size));
        memcpy(row, &full_row, picoutil_aes_block_word_count(block.block_size));
        SET_ROW(block, i, row);
        memset(full_row, 0, picoutil_aes_block_word_count(block.block_size));
    }
    return block;
}

static inline aes_block_t aes_mix_columns(aes_block_t block)
{
    for (size_t i = 0; i < picoutil_aes_block_word_count(block.block_size); ++i)
    {
        GET_COLUMN(block, i, column);
        aes_mul_column_by_poly(column);
        SET_COLUMN(block, i, column);
    }
    return block;
}

static inline aes_block_t aes_inv_mix_columns(aes_block_t block)
{
    for (size_t i = 0; i < picoutil_aes_block_word_count(block.block_size); ++i)
    {
        GET_COLUMN(block, i, column);
        aes_inv_mul_column_by_poly(column);
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

static inline aes_block_t aes_add_iv(aes_block_t block, aes_block_t iv)
{
    // TODO
}

static inline aes_block_t aes_prepare_entry(byte_t* entry, aes_block_t block)
{
    picoutil_memset_explicit(block.block, 0, sizeof(aes_word_t) * picoutil_aes_block_word_count(block.block_size));
    byte_t col1[sizeof(aes_word_t)] = { 0 };
    byte_t col2[sizeof(aes_word_t)] = { 0 };
    byte_t col3[sizeof(aes_word_t)] = { 0 };
    byte_t col4[sizeof(aes_word_t)] = { 0 };
    // Potentially unused columns
    __unused byte_t col5[sizeof(aes_word_t)] = { 0 };
    __unused byte_t col6[sizeof(aes_word_t)] = { 0 };
    __unused byte_t col7[sizeof(aes_word_t)] = { 0 };
    __unused byte_t col8[sizeof(aes_word_t)] = { 0 };

    memcpy(col1, entry, sizeof(aes_word_t));
    memcpy(col2, entry + sizeof(aes_word_t), sizeof(aes_word_t));
    memcpy(col3, entry + (sizeof(aes_word_t) * 2), sizeof(aes_word_t));
    memcpy(col4, entry + (sizeof(aes_word_t) * 3), sizeof(aes_word_t));
    SET_COLUMN(block, 0, col1);
    SET_COLUMN(block, 1, col2);
    SET_COLUMN(block, 2, col3);
    SET_COLUMN(block, 3, col4);
    if (picoutil_aes_block_word_count(block.block_size) == 4)
        return block;

    memcpy(col5, entry + (sizeof(aes_word_t) * 4), sizeof(aes_word_t));
    memcpy(col6, entry + (sizeof(aes_word_t) * 5), sizeof(aes_word_t));
    SET_COLUMN(block, 4, col5);
    SET_COLUMN(block, 5, col6);
    if (picoutil_aes_block_word_count(block.block_size) == 6)
        return block;

    memcpy(col7, entry + (sizeof(aes_word_t) * 6), sizeof(aes_word_t));
    memcpy(col8, entry + (sizeof(aes_word_t) * 7), sizeof(aes_word_t));
    SET_COLUMN(block, 6, col7);
    SET_COLUMN(block, 7, col8);
    return block;
}

// To be used if the provided key buffer is not long enough (or too long)
static inline bool aes_prepare_key(byte_t* buf, size_t bufsize, aes_key_size key_size)
{
    byte_t* bufcpy = picoutil_static_calloc(bufsize, sizeof(byte_t));
    if (bufcpy == NULL)
        return false;
    byte_t* temp = picoutil_static_calloc(bufsize, sizeof(byte_t));
    if (temp == NULL)
    {
        picoutil_static_free(bufcpy);
        return false;
    }
    memcpy(bufcpy, buf, bufsize);
    bzero(buf, bufsize);
    size_t expected_size = picoutil_aes_key_word_count(key_size) * sizeof(aes_word_t);
    if (bufsize < expected_size)
    {
        for (size_t i = 0; bufsize * (i + 1) < expected_size; ++i)
        {
            bzero(temp, bufsize);
            memcpy(temp, bufcpy, bufsize);
            for (size_t j = 0; j < bufsize; ++j)
                temp[j] ^= (aes_sub_word((aes_word_t) temp[MODu32(i * j, bufsize)]) & 0xff) + (aes_round_constant(i) & 0xff);
            memcpy(buf + (bufsize * i), temp, bufsize);
        }
        memcpy(buf + (bufsize * (expected_size / bufsize)), bufcpy, expected_size % bufsize);
    }
    else if (bufsize > expected_size)
        memcpy(buf, bufcpy, expected_size);
    picoutil_static_free(bufcpy);
    picoutil_static_free(temp);
    return true;
}

__zero_used_regs
bool picoutil_aes_key_init(aes_key_t* key, aes_key_size ksize, byte_t* buf, size_t bufsize)
{
    if (key == NULL || buf == NULL || bufsize == 0)
        return false;
    key->key_size = ksize;
    key->key = picoutil_static_calloc(picoutil_aes_key_word_count(ksize), sizeof(aes_word_t));
    if (key->key == NULL)
        return false;
    if (!aes_prepare_key(buf, bufsize, ksize))
    {
        picoutil_static_free(key->key);
        return false;
    }
    memcpy(key->key, buf, picoutil_aes_key_word_count(ksize) * sizeof(aes_word_t));
    return true;
}

__sentinel __zero_used_regs
bool picoutil_aes_context_init_impl(aes_context_t* ctx, aes_mode mode, aes_dir dir, aes_key_size key_size, aes_block_size block_size, ...)
{
    if (ctx == NULL)
        return false;

    *ctx = NULL_T(aes_context_t);

    aes_key_t opt_key = { 0 };
    aes_block_t opt_iv = { 0 };
    void* opt_key_ptr = NULL;
    void* opt_iv_ptr = NULL;
    va_list args;
    va_start(args, block_size);

    // Process variable arguments
    if ( LIKELY((opt_key_ptr = va_arg(args, void*)) != NULL) )
    {
        opt_key = *(aes_key_t*) opt_key_ptr;
        if (opt_key.key == NULL || opt_key.key_size != key_size)
            return false;
    }
    else
        goto end_of_opt;
    if ((opt_iv_ptr = va_arg(args, void*)) != NULL)
    {
        opt_iv = *(aes_block_t*) opt_iv_ptr;
        if (opt_iv.block == NULL || opt_iv.block_size != block_size)
            return false;
    }
    else
        goto end_of_opt;
end_of_opt:
    va_end(args);

    ctx->mode = mode;
    ctx->dir = dir;
    ctx->key_size = key_size;
    ctx->block_size = block_size;
    ctx->key = NULL;
    ctx->iv = NULL;
    ctx->key_expanded = NULL;

    if (opt_key_ptr != NULL)
    {
        ctx->key = picoutil_static_alloc_aligned(sizeof(aes_key_t), alignof(aes_key_t));
        if (ctx->key == NULL)
            return false;
        ctx->key->key_size = key_size;
        ctx->key->key = picoutil_static_calloc_aligned(picoutil_aes_key_word_count(key_size), sizeof(aes_word_t), alignof(aes_word_t));
        if (ctx->key->key == NULL)
        {
            picoutil_static_free(ctx->key);
            return false;
        }
        memcpy(ctx->key->key, opt_key.key, picoutil_aes_key_word_count(key_size) * sizeof(aes_word_t));
    }
    else
        ctx->key = NULL;

    if (opt_iv_ptr != NULL)
    {
        ctx->iv = picoutil_static_alloc_aligned(sizeof(aes_block_t), alignof(aes_block_t));
        if (ctx->iv == NULL)
        {
            if (ctx->key != NULL)
            {
                picoutil_static_free(ctx->key->key);
                picoutil_static_free(ctx->key);
            }
        }
        ctx->iv->block_size = block_size;
        ctx->iv->block = picoutil_static_calloc_aligned(picoutil_aes_block_word_count(block_size), sizeof(aes_word_t), alignof(aes_word_t));
        if (ctx->iv->block == NULL)
        {
            if (ctx->key != NULL)
            {
                picoutil_static_free(ctx->key->key);
                picoutil_static_free(ctx->key);
            }
            picoutil_static_free(ctx->iv);
            return false;
        }
        memcpy(ctx->iv->block, opt_iv.block, picoutil_aes_block_word_count(block_size) * sizeof(aes_word_t));
    }
    else
        ctx->iv = NULL;

    if (ctx->key != NULL)
    {
        ctx->key_expanded = picoutil_static_alloc_aligned(sizeof(aes_key_expanded_t), alignof(aes_key_expanded_t));
        if (ctx->key_expanded == NULL)
        {
            if (ctx->iv != NULL)
            {
                picoutil_static_free(ctx->iv->block);
                picoutil_static_free(ctx->iv);
            }
            picoutil_static_free(ctx->key->key);
            picoutil_static_free(ctx->key);
            return false;
        }
        *ctx->key_expanded = aes_key_schedule(*ctx->key, ctx->block_size);
        const aes_key_expanded_t null_keys = NULL_T(aes_key_expanded_t);
        if (memcmp(&(ctx->key_expanded), &null_keys, sizeof(aes_key_expanded_t)) == 0)
        {
            if (ctx->iv != NULL)
            {
                picoutil_static_free(ctx->iv->block);
                picoutil_static_free(ctx->iv);
            }
            picoutil_static_free(ctx->key->key);
            picoutil_static_free(ctx->key);
            picoutil_static_free(ctx->key_expanded);
            return false;
        }
    }
    else
        ctx->key_expanded = NULL;
    return true;
}

__zero_used_regs
void picoutil_aes_key_destroy(aes_key_t* key)
{
    if (key == NULL)
        return;
    if (key->key != NULL)
    {
        picoutil_static_allocator_set_safe(true);
        picoutil_static_free(key->key);
    }
    *key = NULL_T(aes_key_t);
}

__zero_used_regs
void picoutil_aes_result_destroy(aes_result_t* result)
{
    if (result == NULL)
        return;
    if (result->data != NULL)
    {
        picoutil_static_allocator_set_safe(true);
        picoutil_static_free(result->data);
    }
    *result = NULL_T(aes_result_t);
}

__zero_used_regs
void picoutil_aes_context_deinit(aes_context_t* ctx)
{
    if (ctx == NULL)
        return;
    picoutil_static_allocator_set_safe(true);
    if (ctx->key != NULL)
    {
        if (ctx->key->key != NULL)
            picoutil_static_free(ctx->key->key);
        picoutil_static_free(ctx->key);
    }
    printf("Freed key\n");
    if (ctx->iv != NULL)
    {
        if (ctx->iv->block != NULL)
            picoutil_static_free(ctx->iv->block);
        picoutil_static_free(ctx->iv);
    }
    printf("Freed iv\n");
    if (ctx->key_expanded != NULL)
    {
        printf("Number of keys: %zu\n", ctx->key_expanded->round_count + 1);
        aes_key_expanded_free(*ctx->key_expanded);
        picoutil_static_free(ctx->key_expanded); // Is it needed ?
    }
    printf("Freed key_expanded\n");
    *ctx = NULL_T(aes_context_t);
}

static aes_user_data_t aes_split_bytes(byte_t* bytes, size_t bytes_count, aes_block_size bsize)
{
    aes_user_data_t data = { 0 };
    divmod_result_t divmod = DIVMODu32(bytes_count, sizeof(aes_word_t) * picoutil_aes_block_word_count(bsize));
    data.block_count = to_quotient_u32(divmod) + (to_remainder_u32(divmod) > 0 ? 1 : 0);
    data.blocks = picoutil_static_calloc_aligned(data.block_count, sizeof(aes_block_t), alignof(aes_block_t));
    if (data.blocks == NULL)
        NULL_T(aes_user_data_t);
    for (size_t i = 0; i < data.block_count; ++i)
    {
        data.blocks[i].block_size = bsize;
        data.blocks[i].block = picoutil_static_calloc_aligned(picoutil_aes_block_word_count(bsize), sizeof(aes_word_t), alignof(aes_word_t));
        if (data.blocks[i].block == NULL)
        {
            for (size_t j = 0; j < i; ++j)
                picoutil_static_free(data.blocks[j].block);
            picoutil_static_free(data.blocks);
            return NULL_T(aes_user_data_t);
        }
        aes_prepare_entry(bytes + (i * sizeof(aes_word_t) * picoutil_aes_block_word_count(bsize)), data.blocks[i]);
    }
    return data;
}

static aes_block_t aes_encrypt_ecb(aes_block_t block, aes_key_expanded_t keys)
{
    block = aes_add_round_key(block, keys.round_keys[0]);
    for (size_t i = 1; i < keys.round_count; ++i)
    {
        block = aes_sub_bytes(block);
        block = aes_shift_rows(block);
        block = aes_mix_columns(block);
        block = aes_add_round_key(block, keys.round_keys[i]);
    }
    block = aes_sub_bytes(block);
    block = aes_shift_rows(block);
    block = aes_add_round_key(block, keys.round_keys[keys.round_count]);
    return block;
}

static aes_block_t aes_decrypt_ecb(aes_block_t block, aes_key_expanded_t keys)
{
    block = aes_add_round_key(block, keys.round_keys[keys.round_count]);
    for (size_t i = keys.round_count - 1; i > 0; --i)
    {
        block = aes_inv_shift_rows(block);
        block = aes_inv_sub_bytes(block);
        block = aes_add_round_key(block, keys.round_keys[i]);
        block = aes_inv_mix_columns(block);
    }
    block = aes_inv_shift_rows(block);
    block = aes_inv_sub_bytes(block);
    block = aes_add_round_key(block, keys.round_keys[0]);
    return block;
}

__wur __sentinel __zero_used_regs
aes_result_t __time_critical_func(picoutil_aes_process_impl)(aes_context_t* ctx, byte_t* data, size_t data_size, ...)
{
    if (ctx == NULL || data == NULL || data_size == 0)
        return ({ aes_result_t result = NULL_T(aes_result_t); result.error = true; result; });
    
    aes_key_t opt_key = { 0 };
    aes_block_t opt_iv = { 0 };
    aes_key_expanded_t actual_keys = NULL_T(aes_key_expanded_t);
    aes_block_t actual_iv = NULL_T(aes_block_t);
    void* opt_key_ptr = NULL;
    void* opt_iv_ptr = NULL;
    va_list args;
    va_start(args, data_size);

    // Process variable arguments
    if ((opt_key_ptr = va_arg(args, void*)) != NULL)
    {
        opt_key = *(aes_key_t*) opt_key_ptr;
        if (opt_key.key == NULL || opt_key.key_size != ctx->key_size)
            return ({ aes_result_t result = NULL_T(aes_result_t); result.error = true; result; });
    }
    else
        goto end_of_opt;
    if ((opt_iv_ptr = va_arg(args, void*)) != NULL)
    {
        opt_iv = *(aes_block_t*) opt_iv_ptr;
        if (opt_iv.block == NULL || opt_iv.block_size != ctx->block_size)
            return ({ aes_result_t result = NULL_T(aes_result_t); result.error = true; result; });
    }
    else
        goto end_of_opt;
end_of_opt:
    va_end(args);

    if (opt_key_ptr != NULL)
    {
        actual_keys = aes_key_schedule(opt_key, ctx->block_size);
        const aes_key_expanded_t null_keys = NULL_T(aes_key_expanded_t);
        if (memcmp(&actual_keys, &null_keys, sizeof(aes_key_expanded_t)) == 0)
            return ({ aes_result_t result = NULL_T(aes_result_t); result.error = true; result; });
    }
    else
        actual_keys = *ctx->key_expanded;
    if (opt_iv_ptr != NULL)
        actual_iv = opt_iv;
    else
        actual_iv = *ctx->iv;

    aes_user_data_t user_data = aes_split_bytes(data, data_size, ctx->block_size);
    if (user_data.blocks == NULL)
        return ({ aes_result_t result = NULL_T(aes_result_t); result.error = true; result; });
    for (size_t i = 0; i < user_data.block_count; ++i)
    {
        switch (ctx->mode)
        {
            case AES_MODE_ECB:
                if (ctx->dir == AES_DIR_ENCRYPT)
                    user_data.blocks[i] = aes_encrypt_ecb(user_data.blocks[i], actual_keys);
                else
                    user_data.blocks[i] = aes_decrypt_ecb(user_data.blocks[i], actual_keys);
                break;
            /* Other cases go here too */
            default:
                __unreachable(/* UNIMPLEMENTED */);
                break;
        }
    }
    aes_result_t res = { 0 };
    res.error = false;
    res.data = picoutil_static_calloc_aligned(picoutil_aes_block_word_count(ctx->block_size) * user_data.block_count, sizeof(aes_word_t), alignof(byte_t));
    if (res.data == NULL)
    {
        for (size_t i = 0; i < user_data.block_count; ++i)
            picoutil_static_free(user_data.blocks[i].block);
        picoutil_static_free(user_data.blocks);
        if (opt_key_ptr != NULL)
            aes_key_expanded_free(actual_keys);
        return ({ aes_result_t result = NULL_T(aes_result_t); result.error = true; result; });
    }
    for (size_t i = 0; i < user_data.block_count; ++i)
    {
        memcpy(res.data + (i * sizeof(aes_word_t) * picoutil_aes_block_word_count(ctx->block_size)), user_data.blocks[i].block, sizeof(aes_word_t) * picoutil_aes_block_word_count(ctx->block_size));
        picoutil_static_free(user_data.blocks[i].block);
    }
    res.data_size = picoutil_aes_block_word_count(ctx->block_size) * user_data.block_count * sizeof(aes_word_t);
    picoutil_static_free(user_data.blocks);
    if (opt_key_ptr != NULL) // i.e. if we allocated the key by calling aes_key_schedule
        aes_key_expanded_free(actual_keys);
    return res;
}

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
    int wc = picoutil_aes_block_word_count(block.block_size);
    if (wc < 0)
        printf("Invalid block size\n");
    byte_t* row = __calloca_aligned(wc, sizeof(byte_t), 8);
    if (row == NULL)
        return;
    for (size_t i = 0; i < sizeof(aes_word_t); ++i)
    {
        GET_ROW(block, i, row);
        for (size_t j = 0; j < picoutil_aes_block_word_count(block.block_size); ++j)
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

#include "../aes_test/aes.h"

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

__zero_used_regs
void test_aes_encrypt_decrypt_ecb(void)
{
    __buffer_var unsigned char key_buf[16] = "31415Ap2002#6666";
    __buffer_var unsigned char txt[16] = "This is a test!";

    // ORIGINAL TEXT
    printf("ORIGINAL TEXT:\n");
    print_buf(txt, 16);

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
    aes_result_t res = picoutil_aes_process(&ctx, txt, 16);

    // REFERENCE IMPLEMENTATION
    struct AES_ctx ctx_ref = { 0 };
    AES_init_ctx(&ctx_ref, key_buf);
    AES_ECB_encrypt(&ctx_ref, txt);

    printf("MY IMPLEMENTATION:\n");
    print_buf(res.data, res.data_size);
    printf("REFERENCE IMPLEMENTATION:\n");
    print_buf(txt, 16);

    puts("Test ended");

    picoutil_static_allocator_dump_hdrs();
    
    puts("");
    puts("");
    puts("");
    puts("");
    puts("");

    puts("Context deinitalization...");
    picoutil_aes_context_deinit(&ctx);
    puts("Context deinitalized");
    puts("Result deinitalization...");
    picoutil_aes_result_destroy(&res);
    puts("Result deinitalized");
}