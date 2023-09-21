#ifndef PICOUTIL_H
#define PICOUTIL_H

#ifndef BEGIN_DECLS
    #ifdef __cplusplus
        #define BEGIN_DECLS extern "C" {
    #else
        #define BEGIN_DECLS
    #endif
#endif
#ifndef END_DECLS
    #ifdef __cplusplus
        #define END_DECLS }
    #else
        #define END_DECLS
    #endif
#endif

#ifndef PICO_HAS_XOSC_SUBSYSTEM
    // ROSC should not be used as an entropy source if MCU is running with it as the clock source
    // (i.e. we don't have a "better" clock source (the XOSC))
    #define PICO_RAND_ENTROPY_SRC_ROSC 0
#endif

#ifndef PICOUTIL_STATIC_BYTES_SIZE
    #define PICOUTIL_STATIC_BYTES_SIZE (1024 * 2 * 2 * 2 * 2)
#endif

#ifdef UNIQUE
    #undef UNIQUE
#endif
#define UNIQUE(X) PP_CAT(PP_CAT(X, _), __LINE__)

#ifdef sizeofexpr
    #undef sizeofexpr
#endif
#define sizeofexpr(EXPR) (sizeof(__typeof__(EXPR)))

#ifdef DECAY
    #undef DECAY
#endif
#define DECAY(ARR) (&(*(ARR)) + 0)

#ifdef ATTRIBUTE
    #undef ATTRIBUTE
#endif
#ifdef ATTRIBUTE_WITH_PARAMS
    #undef ATTRIBUTE_WITH_PARAMS
#endif
#if !defined(__GNUC__) && !defined(__clang__)
    #define ATTRIBUTE(X)
    #define ATTRIBUTE_WITH_PARAMS(X, ...)
#else
    #define ATTRIBUTE(X) __attribute__((PP_CAT(__, PP_CAT(X, __))))
    #define ATTRIBUTE_WITH_PARAMS(X, ...) __attribute__((PP_CAT(__, PP_CAT(X, __))(__VA_ARGS__)))
#endif

#if (defined(__GNUC__) || defined(__clang__)) && !defined(UNLIKELY)
    #define UNLIKELY(X) __builtin_expect(!!(X), 0)
#else
    #define UNLIKELY(X) (X)
#endif
#if (defined(__GNUC__) || defined(__clang__)) && !defined(LIKELY)
    #define LIKELY(X) __builtin_expect(!!(X), 1)
#else
    #define LIKELY(X) (X)
#endif

#ifndef _GNU_SOURCE
    #define _GNU_SOURCE 1
#endif

#include <third_party/map_macro/map.h>
#include <third_party/chaos-pp/chaos/preprocessor.h>
#include <pico/platform.h>
#include <hardware/sync.h>
#include <hardware/divider.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>

#ifdef POW_2_PRED
    #undef POW_2_PRED
#endif
#define POW_2_PRED(s, state) CHAOS_PP_BOOL(ARG2(UNTUPLE(state)))

#ifdef POW_2_OP
    #undef POW_2_OP
#endif
#define POW_2_OP(s, state) (CHAOS_PP_ARBITRARY_MUL(ARG1(UNTUPLE(state)), CHAOS_PP_ARBITRARY_PROMOTE(2)), CHAOS_PP_DEC(ARG2(UNTUPLE(state))))

#ifdef POW_2_LOOP
    #undef POW_2_LOOP
#endif
#define POW_2_LOOP(X) CHAOS_PP_EXPR(CHAOS_PP_WHILE(POW_2_PRED, POW_2_OP, (CHAOS_PP_ARBITRARY_PROMOTE(1), X)))

#ifdef POW_PRED
    #undef POW_PRED
#endif
#define POW_PRED(s, state) CHAOS_PP_BOOL(ARG3(UNTUPLE(state)))

#ifdef POW_OP
    #undef POW_OP
#endif
#define POW_OP(s, state) (CHAOS_PP_ARBITRARY_MUL(ARG1(UNTUPLE(state)), CHAOS_PP_ARBITRARY_PROMOTE(ARG2(UNTUPLE(state)))), ARG2(UNTUPLE(state)), CHAOS_PP_DEC(ARG3(UNTUPLE(state))))

#ifdef POW_LOOP
    #undef POW_LOOP
#endif
#define POW_LOOP(X, Y) CHAOS_PP_EXPR(CHAOS_PP_WHILE(POW_PRED, POW_OP, (CHAOS_PP_ARBITRARY_PROMOTE(1), X, Y)))

#ifdef POW
    #undef POW
#endif
#define POW(X, Y) CHAOS_PP_ARBITRARY_DEMOTE(ARG1(UNTUPLE(POW_LOOP(X, Y))))

#ifdef ARG3
    #undef ARG3
#endif
#define ARG3(...) ARG3_PRIMITIVE(__VA_ARGS__)

#ifdef ARG3_PRIMITIVE
    #undef ARG3_PRIMITIVE
#endif
#define ARG3_PRIMITIVE(X, Y, Z, ...) Z

#ifdef POW_2
    #undef POW_2
#endif
#define POW_2(X) CHAOS_PP_ARBITRARY_DEMOTE(ARG1(UNTUPLE(POW_2_LOOP(X))))

#ifdef EXPAND_FLAGS_LOOP_MACRO
    #undef EXPAND_FLAGS_LOOP_MACRO
#endif
#define EXPAND_FLAGS_LOOP_MACRO(s, i, elem, data) CHAOS_PP_COMMA_IF(CHAOS_PP_BOOL(i)) elem = 1U << i

#ifdef EXPAND_FLAGS
    #undef EXPAND_FLAGS
#endif
// EXPAND_FLAGS(FLAG1, FLAG2, FLAG3, ...) -> FLAG1 = 1U << 0, FLAG2 = 1U << 1, FLAG3 = 1U << 2, ...
#define EXPAND_FLAGS(...) CHAOS_PP_SEQ_FOR_EACH_I(EXPAND_FLAGS_LOOP_MACRO, CHAOS_PP_TUPLE_TO_SEQ(CHAOS_PP_VARIADIC_SIZE(__VA_ARGS__), (__VA_ARGS__)), 0)

#ifdef DECL_FLAG_TYPE
    #undef DECL_FLAG_TYPE
#endif
#define DECL_FLAG_TYPE(TYPENAME, ...) enum TYPENAME { CHAOS_PP_EXPR(EXPAND_FLAGS(__VA_ARGS__)) }

#ifdef DECL_FLAGS
    #undef DECL_FLAGS
#endif
#define DECL_FLAGS(...) enum { CHAOS_PP_EXPR(EXPAND_FLAGS(__VA_ARGS__)) }

#ifdef IDENTITY
    #undef IDENTITY
#endif
#define IDENTITY(...) IDENTITY_PRIMITIVE(IDENTITY_PRIMITIVE(IDENTITY_PRIMITIVE(IDENTITY_PRIMITIVE(IDENTITY_PRIMITIVE(IDENTITY_PRIMITIVE(IDENTITY_PRIMITIVE(IDENTITY_PRIMITIVE(IDENTITY_PRIMITIVE(IDENTITY_PRIMITIVE(IDENTITY_PRIMITIVE(IDENTITY_PRIMITIVE(IDENTITY_PRIMITIVE(IDENTITY_PRIMITIVE(__VA_ARGS__))))))))))))))

#ifdef IDENTITY_PRIMITIVE
    #undef IDENTITY_PRIMITIVE
#endif
#define IDENTITY_PRIMITIVE(...) __VA_ARGS__

#ifdef ARG1
    #undef ARG1
#endif
#define ARG1(...) ARG1_PRIMITIVE(__VA_ARGS__)

#ifdef ARG1_PRIMITIVE
    #undef ARG1_PRIMITIVE
#endif
#define ARG1_PRIMITIVE(X, ...) X

#ifdef ARG2
    #undef ARG2
#endif
#define ARG2(...) ARG2_PRIMITIVE(__VA_ARGS__)

#ifdef ARG2_PRIMITIVE
    #undef ARG2_PRIMITIVE
#endif
#define ARG2_PRIMITIVE(X, Y, ...) Y

#ifdef SKIP_ARG1
    #undef SKIP_ARG1
#endif
#define SKIP_ARG1(X, ...) __VA_ARGS__

#ifdef TUPLE
    #undef TUPLE
#endif
#define TUPLE(...) ( MAP_LIST(IDENTITY, __VA_ARGS__) )

#ifdef TUPLE_WITH
    #undef TUPLE_WITH
#endif
#define TUPLE_WITH(...) TUPLE_WITH_PRIMITIVE(__VA_ARGS__)

#if defined(TUPLE_WITH_PRIMITIVE)
    #undef TUPLE_WITH_PRIMITIVE
#endif
#define TUPLE_WITH_PRIMITIVE(...) (__VA_ARGS__, ID_AND_RIGHT_PAREN

#ifdef ID_AND_RIGHT_PAREN
    #undef ID_AND_RIGHT_PAREN
#endif
#define ID_AND_RIGHT_PAREN(...) IDENTITY(__VA_ARGS__))

#ifdef UNTUPLE
    #undef UNTUPLE
#endif
#define UNTUPLE(T) IDENTITY T

#ifdef PREPARE_INTERLEAVE_LOOP_MACRO
    #undef PREPARE_INTERLEAVE_LOOP_MACRO
#endif
#define PREPARE_INTERLEAVE_LOOP_MACRO(s, i, elem, data) CHAOS_PP_COMMA_IF(i) TUPLE(data, elem)

#ifdef INTERLEAVE_PRIMITIVE
    #undef INTERLEAVE_PRIMITIVE
#endif
#define INTERLEAVE_PRIMITIVE(TO_INTERLEAVE_AND_ELEM) ARG1(UNTUPLE(TO_INTERLEAVE_AND_ELEM)) ARG2(UNTUPLE(TO_INTERLEAVE_AND_ELEM))

#ifdef PREPARE_INTERLEAVE
    #undef PREPARE_INTERLEAVE
#endif
#define PREPARE_INTERLEAVE(TO_INTERLEAVE, ...) CHAOS_PP_EXPR(CHAOS_PP_SEQ_FOR_EACH_I(PREPARE_INTERLEAVE_LOOP_MACRO, CHAOS_PP_TUPLE_TO_SEQ(CHAOS_PP_VARIADIC_SIZE(__VA_ARGS__), (__VA_ARGS__)), TO_INTERLEAVE ))

#ifdef INTERLEAVE
    #undef INTERLEAVE
#endif
#define INTERLEAVE(TO_INTERLEAVE, ...) ARG1(__VA_ARGS__) MAP(INTERLEAVE_PRIMITIVE, PREPARE_INTERLEAVE(TO_INTERLEAVE, SKIP_ARG1(__VA_ARGS__)))

#ifdef HAS_FLAG
    #undef HAS_FLAG
#endif
#define HAS_FLAG(FLAG, VALUE) (((FLAG) & (VALUE)) == (FLAG))

#ifdef PP_CAT
    #undef PP_CAT
#endif
#define PP_CAT(X, Y) PP_CAT_PRIMITIVE(X, Y)

#ifdef PP_CAT_PRIMITIVE
    #undef PP_CAT_PRIMITIVE
#endif
#define PP_CAT_PRIMITIVE(X, Y) X ## Y

#ifdef HAS_ONE_OF_FLAGS
    #undef HAS_ONE_OF_FLAGS
#endif
#define HAS_ONE_OF_FLAGS(FLAGS, VALUE)                                      \
    __extension__                                                           \
    ({                                                                      \
        int PP_CAT(all_flags, __LINE__) = INTERLEAVE(|, UNTUPLE(FLAGS));    \
        int PP_CAT(value, __LINE__) = (VALUE);                              \
        HAS_FLAG(PP_CAT(value, __LINE__), PP_CAT(all_flags, __LINE__));     \
    })

BEGIN_DECLS

typedef uint8_t byte_t;

void      picoutil_static_allocator_init(bool safe);
bool      picoutil_static_allocator_set_safe(bool safe);
bool      picoutil_static_allocator_is_safe(void);

ATTRIBUTE(malloc) ATTRIBUTE(warn_unused_result)
void*     __time_critical_func(picoutil_static_alloc_aligned)(size_t size, size_t requested_align);
ATTRIBUTE(malloc) ATTRIBUTE(warn_unused_result)
void*     __time_critical_func(picoutil_static_alloc)(size_t size);
ATTRIBUTE(malloc) ATTRIBUTE(warn_unused_result)
void*     __time_critical_func(picoutil_static_calloc_aligned)(size_t count, size_t size, size_t requested_align);
ATTRIBUTE(malloc) ATTRIBUTE(warn_unused_result)
void*     __time_critical_func(picoutil_static_calloc)(size_t count, size_t size);
ATTRIBUTE(warn_unused_result)
void*     __time_critical_func(picoutil_static_realloc_aligned)(void* ptr, size_t size, size_t requested_align);
ATTRIBUTE(warn_unused_result)
void*     __time_critical_func(picoutil_static_realloc)(void* ptr, size_t size);

void      __time_critical_func(picoutil_static_free)(void* ptr);
void      __time_critical_func(picoutil_static_free_all)(void);
void      __time_critical_func(picoutil_static_free_all_except)(void** ptr, size_t count);

uintptr_t picoutil_static_bytes_get_start_addr(void);
uintptr_t picoutil_static_bytes_get_end_addr(void);
void      picoutil_static_allocator_dump_hdrs(void);
void      picoutil_static_allocator_memdump(uintptr_t range[2]);

enum barrier_target
{
    BARRIER_DATA = 0x1,
    BARRIER_INS = 0x2,
    BARRIER_ALL = BARRIER_DATA | BARRIER_INS,

    BARRIER_TARGET_COUNT = 3
};

enum barrier_option
{
    BARRIER_SY = 0x1,
    BARRIER_ST = 0x2,
    BARRIER_ISH = 0x4,
    BARRIER_ISHST = 0x8,
    BARRIER_NSH = 0x10,
    BARRIER_NSHST = 0x20,
    BARRIER_OSH = 0x40,
    BARRIER_OSHST = 0x80,
    BARRIER_ALL_OPTIONS = BARRIER_SY,

    BARRIER_OPTIONS_COUNT = 9
};

static inline
void picoutil_memset_explicit(void* ptr, byte_t value, size_t size)
{
    byte_t* ptr_ = (byte_t*)ptr;
    for (size_t i = 0; i < size; i++)
    {
        byte_t* ptr_i = ptr_ + i;
        /* `value` and `ptr_i` must be in one of the following registers: r0 to r7 */
        pico_default_asm_volatile(
            "strb %[value], [%[ptr_i]]\n\t"
            : /* no output (at least no output wanted) */
            : [value] "l" (value), [ptr_i] "l" (ptr_i)
            : "memory"
        );
    }
    pico_default_asm_volatile(
        "\tisb sy\n\tdsb sy\n\t" : : "r" (ptr_) /* mark it as input */ : "memory"
    );
}

static inline
void __time_critical_func(picoutil_memory_barrier)(enum barrier_option option)
{
    switch (option)
    {
        case 0:
            break;
        case BARRIER_SY:
            pico_default_asm_volatile(
                "dmb sy"
            );
            break;
        case BARRIER_ST:
            pico_default_asm_volatile(
                "dmb st"
            );
            break;
        case BARRIER_ISH:
            pico_default_asm_volatile(
                "dmb ish"
            );
            break;
        case BARRIER_ISHST:
            pico_default_asm_volatile(
                "dmb ishst"
            );
            break;
        case BARRIER_NSH:
            pico_default_asm_volatile(
                "dmb nsh"
            );
            break;
        case BARRIER_NSHST:
            pico_default_asm_volatile(
                "dmb nshst"
            );
            break;
        case BARRIER_OSH:
            pico_default_asm_volatile(
                "dmb osh"
            );
            break;
        case BARRIER_OSHST:
            pico_default_asm_volatile(
                "dmb oshst"
            );
            break;
        default:
            pico_default_asm_volatile(
                "dmb sy"
            );
            break;
    }
}

static inline
void __time_critical_func(picoutil_sync_barrier)(enum barrier_target target, enum barrier_option option)
{
    switch (target)
    {
        case 0:
            break;
        case BARRIER_DATA:
            switch (option)
            {
                case 0:
                    break;
                default:
                    ATTRIBUTE(fallthrough);
                case BARRIER_SY:
                    pico_default_asm_volatile(
                        "dsb sy"
                    );
                    break;
                case BARRIER_ST:
                    pico_default_asm_volatile(
                        "dsb st"
                    );
                    break;
                case BARRIER_ISH:
                    pico_default_asm_volatile(
                        "dsb ish"
                    );
                    break;
                case BARRIER_ISHST:
                    pico_default_asm_volatile(
                        "dsb ishst"
                    );
                    break;
                case BARRIER_NSH:
                    pico_default_asm_volatile(
                        "dsb nsh"
                    );
                    break;
                case BARRIER_NSHST:
                    pico_default_asm_volatile(
                        "dsb nshst"
                    );
                    break;
                case BARRIER_OSH:
                    pico_default_asm_volatile(
                        "dsb osh"
                    );
                    break;
                case BARRIER_OSHST:
                    pico_default_asm_volatile(
                        "dsb oshst"
                    );
                    break;
            }
            break;
        case BARRIER_INS:
            pico_default_asm_volatile(
                "isb sy"
            );
            break;
        default:
            ATTRIBUTE(fallthrough);
        case BARRIER_ALL:
            picoutil_sync_barrier(BARRIER_DATA, option);
            picoutil_sync_barrier(BARRIER_INS, option);
            break;
    }
}


#ifdef picoutil_atomic_op
    #undef picoutil_atomic_op
#endif
#define picoutil_atomic_op(OP, ORDER)                                                                                                               \
    {                                                                                                                                               \
        switch (ORDER)                                                                                                                              \
        {                                                                                                                                           \
            case memory_order_relaxed:                                                                                                              \
                /* Only atomicity is guaranteed */                                                                                                  \
                /* Unsupported if the operation is not lock-free (a lock should be used instead) */                                                 \
                OP;                                                                                                                                 \
                break;                                                                                                                              \
            case memory_order_consume:                                                                                                              \
                /* no reads or writes in the current thread dependent on the value currently loaded can be reordered before this load */            \
                /* writes to data-dependent variables in other threads that release the same atomic variable are visible in the current thread */   \
                /* FIXME: maybe not appropriate */                                                                                                  \
                goto PP_CAT(acq_rel, __LINE__);                                                                                                     \
            case memory_order_acquire:                                                                                                              \
                /* all writes in other threads that release the same atomic variable are visible in the current thread */                           \
                __compiler_memory_barrier();                                                                                                        \
                picoutil_sync_barrier(BARRIER_ALL, BARRIER_ST /* since releasing is a store */);                                                    \
                __compiler_memory_barrier();                                                                                                        \
                OP;                                                                                                                                 \
                /* no reads or writes in the current thread can be reordered before this load */                                                    \
                __compiler_memory_barrier();                                                                                                        \
                break;                                                                                                                              \
            case memory_order_release:                                                                                                              \
                /* no reads or writes in the current thread can be reordered after this store */                                                    \
                __compiler_memory_barrier();                                                                                                        \
                OP;                                                                                                                                 \
                __compiler_memory_barrier();                                                                                                        \
                /* all writes in the current thread are visible in other threads that acquire the same atomic variable */                           \
                /* and writes that carry a dependency into the atomic variable become visible in other threads that consume the same atomic */      \
                picoutil_sync_barrier(BARRIER_ALL, BARRIER_SY);                                                                                     \
                __compiler_memory_barrier();                                                                                                        \
                break;                                                                                                                              \
            case memory_order_acq_rel:                                                                                                              \
PP_CAT(acq_rel, __LINE__):                                                                                                                          \
                /* a read-modify-write operation with this memory order is both an acquire operation and a release operation */                     \
                /* no memory reads or writes in the current thread can be reordered before the load, nor after the store */                         \
                /* all writes in other threads that release the same atomic variable are visible before the modification */                         \
                /* and the modification is visible in other threads that acquire the same atomic variable */                                        \
                __compiler_memory_barrier();                                                                                                        \
                picoutil_sync_barrier(BARRIER_ALL, BARRIER_SY);                                                                                     \
                __compiler_memory_barrier();                                                                                                        \
                OP;                                                                                                                                 \
                __compiler_memory_barrier();                                                                                                        \
                picoutil_sync_barrier(BARRIER_ALL, BARRIER_SY);                                                                                     \
                __compiler_memory_barrier();                                                                                                        \
                break;                                                                                                                              \
            case memory_order_seq_cst:                                                                                                              \
                __compiler_memory_barrier();                                                                                                        \
                picoutil_sync_barrier(BARRIER_ALL, BARRIER_SY);                                                                                     \
                __compiler_memory_barrier();                                                                                                        \
                OP;                                                                                                                                 \
                __compiler_memory_barrier();                                                                                                        \
                picoutil_sync_barrier(BARRIER_ALL, BARRIER_SY);                                                                                     \
                __compiler_memory_barrier();                                                                                                        \
                break;                                                                                                                              \
            default:                                                                                                                                \
                break;                                                                                                                              \
        }                                                                                                                                           \
    }

typedef enum
{
    AES_KEY_SIZE_128 = 0,
    AES_KEY_SIZE_192 = 1,
    AES_KEY_SIZE_256 = 2
} aes_key_size;

#ifdef TYPEOF
    #undef TYPEOF
#endif
#define TYPEOF(EXPR)                \
    _Generic((EXPR),                \
        uint8_t: uint8_t,           \
        uint16_t: uint16_t,         \
        uint32_t: uint32_t,         \
        uint64_t: uint64_t,         \
        int8_t: int8_t,             \
        int16_t: int16_t,           \
        int32_t: int32_t,           \
        int64_t: int64_t,           \
        void: void,                 \
        /* Add types here */        \
        default: __typeof__((EXPR)) \
    )

#ifdef picoutil_aes_round_count
    #undef picoutil_aes_round_count
#endif
#define picoutil_aes_round_count(KEY_SIZE)                                                              \
    ((KEY_SIZE) == AES_KEY_SIZE_128 ? 10 : ((KEY_SIZE) == AES_KEY_SIZE_192 ? 12 :                       \
    ((KEY_SIZE) == AES_KEY_SIZE_256 ? 14 : -1)))

#ifdef picoutil_aes_key_word_count
    #undef picoutil_aes_key_word_count
#endif
#define picoutil_aes_key_word_count(KEY_SIZE)                                                       \
    ((KEY_SIZE) == AES_KEY_SIZE_128 ? 4 : ((KEY_SIZE) == AES_KEY_SIZE_192 ? 6 :                     \
    ((KEY_SIZE) == AES_KEY_SIZE_256 ? 8 : -1)))

#ifdef picoutil_aes_key_byte_count
    #undef picoutil_aes_key_byte_count
#endif
#define picoutil_aes_key_byte_count(KEY_SIZE) (picoutil_aes_key_word_count(KEY_SIZE) * sizeof(aes_word_t))

typedef enum
{
    AES_BLOCK_SIZE_128 = 0,
    AES_BLOCK_SIZE_192 = 1,
    AES_BLOCK_SIZE_256 = 2
} aes_block_size;

#ifdef picoutil_aes_block_word_count
    #undef picoutil_aes_block_word_count
#endif
#define picoutil_aes_block_word_count(BLOCK_SIZE)                                                               \
    ((BLOCK_SIZE) == AES_BLOCK_SIZE_128 ? 4 : ((BLOCK_SIZE) == AES_BLOCK_SIZE_192 ? 6 :                         \
    ((BLOCK_SIZE) == AES_BLOCK_SIZE_256 ? 8 : -1)))

typedef enum
{
    AES_MODE_ECB = 0,
    AES_MODE_CBC = 1,
    AES_MODE_CTR = 2,
    AES_MODE_CFB = 3,
    AES_MODE_OFB = 4,
    AES_MODE_CCM = 5,
    AES_MODE_GCM = 6
} aes_mode;

typedef enum
{
    AES_DIR_ENCRYPT = 0,
    AES_DIR_DECRYPT = 1
} aes_dir;

typedef uint8_t byte_t;
typedef uint32_t aes_word_t;

#ifdef VECTOR
    #undef VECTOR
#endif
// Only for clarity purposes
// Sizes are in bytes
#define VECTOR(...)

typedef struct aes_key
{
    /* VECTOR(key_size / 8) */
    /* byte_t* key; */
    aes_word_t* key;
    aes_key_size key_size;
} aes_key_t;

typedef struct aes_key_expanded
{
    /* VECTOR(pico_aes_round_count(key_size), 16) */
    /* byte_t** round_keys; */
    aes_word_t** round_keys;
    size_t round_count;
} aes_key_expanded_t;

typedef struct aes_block
{
    aes_word_t* block;
    aes_block_size block_size;
} aes_block_t;

typedef struct aes_context
{
    
} aes_context_t;

void        picoutil_aes_init(void);
bool        picoutil_aes_error(void);

aes_block_t __time_critical_func(picoutil_aes_encrypt_block)(aes_block_t block, aes_key_t key);
aes_block_t __time_critical_func(picoutil_aes_encrypt_block_until)(aes_block_t block, aes_key_t key, size_t num_round);

void        picoutil_test_key_schedule(void);
void        picoutil_test_sub_bytes(void);
void        picoutil_test_shift_rows(void);
void        picoutil_test_mix_columns(void);
void        picoutil_test_encryption_ecb_mode(size_t num_rounds);

END_DECLS

#ifdef DIVMOD_RAW
    #undef DIVMOD_RAW
#endif
#define DIVMOD_RAW(X, Y)                                                                                    \
    __extension__                                                                                           \
    ({                                                                                                      \
        divmod_result_t PP_CAT(x_, __LINE__) = (X);                                                         \
        divmod_result_t PP_CAT(y_, __LINE__) = (Y);                                                         \
        PP_CAT(x_, __LINE__) / PP_CAT(y_, __LINE__) | PP_CAT(x_, __LINE__) % PP_CAT(y_, __LINE__) << 32;    \
    })
#ifdef HW_DIVMODu32
    #undef HW_DIVMODu32
#endif
#ifdef HW_MODu32
    #undef HW_MODu32
#endif
#ifdef HW_DIVu32
    #undef HW_DIVu32
#endif
#ifdef DIVMODu32
    #undef DIVMODu32
#endif
#ifdef MODu32
    #undef MODu32
#endif
#ifdef DIVu32
    #undef DIVu32
#endif
#define HW_DIVMODu32(X, Y) ((divmod_result_t) hw_divider_divmod_u32((uint32_t)(X), (uint32_t)(Y)))
#define HW_MODu32(X, Y) ((uint32_t) hw_divider_u32_remainder((uint32_t)(X), (uint32_t)(Y)))
#define HW_DIVu32(X, Y) ((uint32_t) hw_divider_u32_quotient((uint32_t)(X), (uint32_t)(Y)))
#define DIVMODu32(X, Y) __builtin_choose_expr(  \
    __builtin_constant_p((X) / (Y)) &&          \
    __builtin_constant_p((X) % (Y)),            \
    DIVMOD_RAW((X), (Y)),                       \
    HW_DIVMODu32((X), (Y))                      \
)
#define MODu32(X, Y) __builtin_choose_expr( \
    __builtin_constant_p((X) % (Y)),        \
    ((X) % (Y)),                            \
    HW_MODu32((X), (Y))                     \
)
#define DIVu32(X, Y) __builtin_choose_expr( \
    __builtin_constant_p((X) / (Y)),        \
    ((X) / (Y)),                            \
    HW_DIVu32((X), (Y))                     \
)

#ifdef MUL
    #undef MUL
#endif
#define MUL(A, B) __fast_mul((A), (B))

#ifdef ROTATE
    #undef ROTATE
#endif
#define ROTATE(L_OR_R, X, Y) (PP_CAT(ROTATE_, L_OR_R)((X), (Y)))

#ifdef ROTATE_RAW_LEFT
    #undef ROTATE_RAW_LEFT
#endif
#define ROTATE_RAW_LEFT(X, Y) ((X) << (Y) | (X) >> ((sizeof(X) * 8) - (Y)))

#ifdef ROTATE_RAW_RIGHT
    #undef ROTATE_RAW_RIGHT
#endif
#define ROTATE_RAW_RIGHT(X, Y) ((X) >> (Y) | (X) << ((sizeof(X) * 8) - (Y)))

#ifdef ROTATE_RIGHT
    #undef ROTATE_RIGHT
#endif
#define ROTATE_RIGHT(X, Y)                                  \
    __builtin_choose_expr(                                  \
        __builtin_constant_p(ROTATE_RAW_RIGHT((X), (Y))) || \
        __builtin_constant_p((X))                        || \
        __builtin_constant_p((Y)),                          \
        ROTATE_RAW_RIGHT((X), (Y)),                         \
        picoutil_rotate_right((X), (Y))                     \
    )

#ifdef ROTATE_LEFT
    #undef ROTATE_LEFT
#endif
#define ROTATE_LEFT(X, Y)                                   \
    __builtin_choose_expr(                                  \
        __builtin_constant_p(ROTATE_RAW_LEFT((X), (Y))) ||  \
        __builtin_constant_p((X))                       ||  \
        __builtin_constant_p((Y)),                          \
        ROTATE_RAW_LEFT((X), (Y)),                          \
        picoutil_rotate_left((X), (Y))                      \
    )

#ifdef picoutil_rotate_right
    #undef picoutil_rotate_right
#endif
#define picoutil_rotate_right(X, Y)                                     \
    ({                                                                  \
        __typeof__((X)) PP_CAT(ret_, __LINE__);                         \
        if (sizeof((X)) != 32)                                          \
            PP_CAT(ret_, __LINE__) = ROTATE_RAW_RIGHT((X), (Y));        \
        else                                                            \
            PP_CAT(ret_, __LINE__) = picoutil_rotate_right_32((X), (Y));\
        PP_CAT(ret_, __LINE__);                                         \
    })

#ifdef picoutil_rotate_left
    #undef picoutil_rotate_left
#endif
#define picoutil_rotate_left(X, Y)                                      \
    ({                                                                  \
        __typeof__((X)) PP_CAT(ret_, __LINE__);                         \
        if (sizeof((X)) != 32)                                          \
            PP_CAT(ret_, __LINE__) = ROTATE_RAW_LEFT((X), (Y));         \
        else                                                            \
            PP_CAT(ret_, __LINE__) = picoutil_rotate_left_32((X), (Y)); \
        PP_CAT(ret_, __LINE__);                                         \
    })

#ifdef pico_default_asm_inline
    #undef pico_default_asm_inline
#endif
#define pico_default_asm_inline(...) __asm__ inline (".syntax unified\n" __VA_ARGS__)

#ifdef pico_default_asm_inline_volatile
    #undef pico_default_asm_inline_volatile
#endif
#define pico_default_asm_inline_volatile(...) __asm__ inline volatile (".syntax unified\n" __VA_ARGS__)

BEGIN_DECLS

static inline ATTRIBUTE(const)
uint32_t picoutil_rotate_right_32(uint32_t x, uint32_t y)
{
    register uint32_t x_reg = x;
    register uint32_t y_reg = MODu32(y, 32);
    // `x_reg` is input AND output
    pico_default_asm(
        "\trors %[x], %[y]\n\t"
        : [x] "+l" (x_reg)
        : [y] "l" (y_reg)
        : "cc"
    );
    return x_reg;
}

static inline ATTRIBUTE(const)
uint32_t picoutil_rotate_left_32(uint32_t x, uint32_t y)
{
    // Take a value for `actual_y` such that the right rotation by its value is equivalent to the wanted left rotation
    uint32_t actual_y = 32 - MODu32(y, 32);
    return picoutil_rotate_right_32(x, actual_y);
}

END_DECLS

#ifdef ROL
    #undef ROL
#endif
#ifdef ROR
    #undef ROR
#endif
#define ROL(X, Y) ROTATE(LEFT, (X), (Y))
#define ROR(X, Y) ROTATE(RIGHT, (X), (Y))

#ifdef DIGIT_COUNT
    #undef DIGIT_COUNT
#endif
#define DIGIT_COUNT(X)                                                      \
    __extension__                                                           \
    ({                                                                      \
        size_t PP_CAT(ret_, __LINE__) = 0;                                  \
        if (sizeofexpr(X) <= 32)                                            \
        {                                                                   \
            __typeof__(X) PP_CAT(x_, __LINE__) = (X);                       \
            while (PP_CAT(x_, __LINE__) != 0)                               \
            {                                                               \
                PP_CAT(x_, __LINE__) = DIVu32(PP_CAT(x_, __LINE__), 10);    \
                PP_CAT(ret_, __LINE__)++;                                   \
            }                                                               \
        }                                                                   \
        else                                                                \
        {                                                                   \
            __typeof__(X) PP_CAT(x_, __LINE__) = (X);                       \
            while (PP_CAT(x_, __LINE__) != 0)                               \
            {                                                               \
                PP_CAT(x_, __LINE__) /= 10;                                 \
                PP_CAT(ret_, __LINE__)++;                                   \
            }                                                               \
        }                                                                   \
        PP_CAT(ret_, __LINE__);                                             \
    })

#ifdef ROL_ARR
    #undef ROL_ARR
#endif
#ifdef ROR_ARR
    #undef ROR_ARR
#endif
#define ROL_ARR(ARR, SIZE, Y)                                                                                       \
    __extension__                                                                                                   \
    ({                                                                                                              \
        __typeof__(ARR[0]) PP_CAT(temp_, __LINE__)[(SIZE)];                                                         \
        memset(                                                                                                     \
            PP_CAT(temp_, __LINE__),                                                                                \
            0,                                                                                                      \
            (SIZE) * sizeofexpr(ARR[0])                                                                             \
        );                                                                                                          \
        for (size_t PP_CAT(i_, __LINE__) = 0;                                                                       \
             PP_CAT(i_, __LINE__) < (SIZE);                                                                         \
             PP_CAT(i_, __LINE__)++)                                                                                \
        {                                                                                                           \
            PP_CAT(temp_, __LINE__)[MODu32(PP_CAT(i_, __LINE__) + (Y), (SIZE))] = (ARR)[PP_CAT(i_, __LINE__)];      \
        }                                                                                                           \
        for (size_t PP_CAT(i_, __LINE__) = 0;                                                                       \
             PP_CAT(i_, __LINE__) < (SIZE);                                                                         \
             PP_CAT(i_, __LINE__)++)                                                                                \
        {                                                                                                           \
            (ARR)[PP_CAT(i_, __LINE__)] = PP_CAT(temp_, __LINE__)[PP_CAT(i_, __LINE__)];                            \
        }                                                                                                           \
    })

#ifdef ROR_ARR
    #undef ROR_ARR
#endif
#define ROR_ARR(ARR, SIZE, Y) ROL_ARR((ARR), (SIZE), (MODu32((SIZE) - (Y), (SIZE))))

#ifdef ROL_DIGITS
    #undef ROL_DIGITS
#endif
#ifdef ROR_DIGITS
    #undef ROR_DIGITS
#endif
// Rotate the digits to the left instead of the bits
#define ROL_DIGITS(X, Y)                                                                                                                \
    __extension__                                                                                                                       \
    ({                                                                                                                                  \
        __typeof__(X) PP_CAT(ret_, __LINE__) = 0;                                                                                       \
        size_t PP_CAT(dig_count_, __LINE__) = DIGIT_COUNT(X);                                                                           \
        if (sizeofexpr(X) <= 32 && sizeofexpr(Y) <= 32)                                                                                 \
        {                                                                                                                               \
            __typeof__(X) PP_CAT(x_, __LINE__) = (X);                                                                                   \
            __typeof__(Y) PP_CAT(y_, __LINE__) = MODu32((Y), PP_CAT(dig_count_, __LINE__));                                             \
            uint8_t PP_CAT(x_digits_, __LINE__)[PP_CAT(dig_count_, __LINE__)];                                                          \
            memset(PP_CAT(x_digits_, __LINE__), 0, count_of(PP_CAT(x_digits_, __LINE__)));                                              \
            for (size_t PP_CAT(i_, __LINE__) = 0;                                                                                       \
                 PP_CAT(i_, __LINE__) < PP_CAT(dig_count_, __LINE__);                                                                   \
                 PP_CAT(i_, __LINE__)++)                                                                                                \
            {                                                                                                                           \
                divmod_result_t PP_CAT(divmod_, __LINE__) = DIVMODu32(PP_CAT(x_, __LINE__), 10);                                        \
                PP_CAT(x_digits_, __LINE__)[PP_CAT(i_, __LINE__)] = to_remainder_u32(PP_CAT(divmod_, __LINE__));                        \
                PP_CAT(x_, __LINE__) = to_quotient_u32(PP_CAT(divmod_, __LINE__));                                                      \
            }                                                                                                                           \
            ROL_ARR(PP_CAT(x_digits_, __LINE__), PP_CAT(dig_count_, __LINE__), PP_CAT(y_, __LINE__));                                   \
            for (size_t PP_CAT(i_, __LINE__) = 0;                                                                                       \
                 PP_CAT(i_, __LINE__) < PP_CAT(dig_count_, __LINE__);                                                                   \
                 PP_CAT(i_, __LINE__)++)                                                                                                \
            {                                                                                                                           \
                PP_CAT(ret_, __LINE__) += MUL(PP_CAT(x_digits_, __LINE__)[PP_CAT(i_, __LINE__)], powint(10.0, PP_CAT(i_, __LINE__)));   \
            }                                                                                                                           \
        }                                                                                                                               \
        else                                                                                                                            \
        {                                                                                                                               \
            __typeof__(X) PP_CAT(x_, __LINE__) = (X);                                                                                   \
            __typeof__(Y) PP_CAT(y_, __LINE__) = (Y) % PP_CAT(dig_count_, __LINE__);                                                    \
            uint8_t PP_CAT(x_digits_, __LINE__)[PP_CAT(dig_count_, __LINE__)];                                                          \
            memset(PP_CAT(x_digits_, __LINE__), 0, count_of(PP_CAT(x_digits_, __LINE__)));                                              \
            for (size_t PP_CAT(i_, __LINE__) = 0;                                                                                       \
                 PP_CAT(i_, __LINE__) < PP_CAT(dig_count_, __LINE__);                                                                   \
                 PP_CAT(i_, __LINE__)++)                                                                                                \
            {                                                                                                                           \
                __typeof__(X) UNIQUE(div) = PP_CAT(x_, __LINE__) / 10;                                                                  \
                __typeof__(X) UNIQUE(mod) = PP_CAT(x_, __LINE__) % 10;                                                                  \
                PP_CAT(x_digits_, __LINE__)[PP_CAT(i_, __LINE__)] = UNIQUE(mod);                                                        \
                PP_CAT(x_, __LINE__) = UNIQUE(div);                                                                                     \
            }                                                                                                                           \
            ROL_ARR(PP_CAT(x_digits_, __LINE__), PP_CAT(dig_count_, __LINE__), PP_CAT(y_, __LINE__));                                   \
            for (size_t PP_CAT(i_, __LINE__) = 0;                                                                                       \
                 PP_CAT(i_, __LINE__) < PP_CAT(dig_count_, __LINE__);                                                                   \
                 PP_CAT(i_, __LINE__)++)                                                                                                \
            {                                                                                                                           \
                PP_CAT(ret_, __LINE__) += MUL(PP_CAT(x_digits_, __LINE__)[PP_CAT(i_, __LINE__)], powint(10.0, PP_CAT(i_, __LINE__)));   \
            }                                                                                                                           \
        }                                                                                                                               \
        PP_CAT(ret_, __LINE__);                                                                                                         \
    })
// Rotate the digits to the right instead of the bits
#define ROR_DIGITS(X, Y) ROL_DIGITS((X), (MODu32((DIGIT_COUNT(X) - (Y)), DIGIT_COUNT(X))))



#endif

#ifdef __INTELLISENSE__
    #undef POW
    #define POW(...) 0
#endif