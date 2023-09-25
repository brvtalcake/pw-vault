#ifndef PICOUTIL_H
#define PICOUTIL_H

#include <picoutil_version.h>
// TODO: Create picoutil_config.h.in and use CMake to generate all stuffs like `PICO_HAS_XOSC_SUBSYSTEM` and `PICOUTIL_STATIC_BYTES_SIZE`
/* #include <picoutil_config.h> */

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

#ifdef NULL_T
    #error "NULL_T is already defined"
    #undef NULL_T
#endif
#define NULL_T(TYPE) ((TYPE){ 0 })

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
#define ATTRIBUTE(X) __attribute__((PP_CAT(__, PP_CAT(X, __))))
#define ATTRIBUTE_WITH_PARAMS(X, ...) __attribute__((PP_CAT(__, PP_CAT(X, __))(__VA_ARGS__)))

#ifndef UNLIKELY
    #define UNLIKELY(X) __builtin_expect(!!(X), 0)
#endif
#ifndef LIKELY
    #define LIKELY(X) __builtin_expect(!!(X), 1)
#endif

#ifdef HAS_ATTRIBUTE
    #undef HAS_ATTRIBUTE
#endif
#ifdef HAS_ATTRIBUTE_WITH_PARAMS
    #undef HAS_ATTRIBUTE_WITH_PARAMS
#endif
#define HAS_ATTRIBUTE(X, ATTR) __builtin_has_attribute((X), ATTRIBUTE(ATTR))
#define HAS_ATTRIBUTE_WITH_PARAMS(X, ATTR, ...) __builtin_has_attribute((X), ATTRIBUTE_WITH_PARAMS(ATTR, __VA_ARGS__))


#ifdef TYPES_COMPATIBLE
    #undef TYPES_COMPATIBLE
#endif
#define TYPES_COMPATIBLE(TYPE1, TYPE2) __builtin_types_compatible_p(__typeof__(TYPE1), __typeof__(TYPE2))

#ifdef TYPEOF
    #undef TYPEOF
#endif
#define TYPEOF(EXPR) __typeof__(EXPR)

#ifdef IS_COMPTIME
    #undef IS_COMPTIME
#endif
#define IS_COMPTIME(EXPR) __builtin_constant_p(EXPR)

#ifdef CLASSIFY_EXPR
    #undef CLASSIFY_EXPR
#endif
#define CLASSIFY_EXPR(EXPR) __builtin_classify_type(EXPR)

#ifndef _GNU_SOURCE
    #define _GNU_SOURCE 1
#endif

#include <third_party/map_macro/map.h>
#include <third_party/chaos-pp/chaos/preprocessor.h>
#include CHAOS_PP_PLACEHOLDERS(1)

#include <pico/platform.h>
#include <hardware/sync.h>
#include <hardware/divider.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h> // For memory_order enum
#include <stdalign.h>
#include <stddef.h>

#include <picoutil_opt_buf.h>

#ifdef STRINGIFY
    #undef STRINGIFY
#endif
#define STRINGIFY(X) STRINGIFY_PRIMITIVE(X)

#ifdef STRINGIFY_PRIMITIVE
    #undef STRINGIFY_PRIMITIVE
#endif
#define STRINGIFY_PRIMITIVE(X) #X

#ifdef STRCATIFY
    #undef STRCATIFY
#endif
#define STRCATIFY(...) STRCATIFY_PRIMITIVE(__VA_ARGS__)

#ifdef STRCATIFY_PRIMITIVE
    #undef STRCATIFY_PRIMITIVE
#endif
#define STRCATIFY_PRIMITIVE(...) STRINGIFY(CHAOS_PP_VARIADIC_CAT(__VA_ARGS__))

#ifdef TO_SEQ
    #undef TO_SEQ
#endif
#define TO_SEQ(...) TO_SEQ_PRIMITIVE(__VA_ARGS__)

#ifdef TO_SEQ_PRIMITIVE
    #undef TO_SEQ_PRIMITIVE
#endif
#define TO_SEQ_PRIMITIVE(...) CHAOS_PP_EXPR(CHAOS_PP_TUPLE_TO_SEQ(CHAOS_PP_VARIADIC_SIZE(__VA_ARGS__), (__VA_ARGS__)))

#ifdef VA_RANGE
    #undef VA_RANGE
#endif
#define VA_RANGE(FROM, TO, ...) VA_RANGE_PRIMITIVE(FROM, TO, __VA_ARGS__)

#ifdef VA_RANGE_PRIMITIVE
    #undef VA_RANGE_PRIMITIVE
#endif
#define VA_RANGE_PRIMITIVE(FROM, TO, ...) CHAOS_PP_EXPR(CHAOS_PP_SEQ_RANGE(FROM, CHAOS_PP_SUB(CHAOS_PP_INC(TO), FROM), TO_SEQ(__VA_ARGS__)))

#ifndef __unused
    #define __unused ATTRIBUTE(unused)
#endif
#ifndef __used
    #define __used ATTRIBUTE(used)
#endif
#ifndef __packed
    #define __packed ATTRIBUTE(packed)
#endif
#ifndef __aligned
    #define __aligned(X) ATTRIBUTE_WITH_PARAMS(aligned, X)
#endif
#ifndef __wur
    #define __wur ATTRIBUTE(warn_unused_result)
#endif
#ifndef __sentinel
    #define __sentinel ATTRIBUTE(sentinel)
#endif
#ifndef __section
    #define __section(SECTION) ATTRIBUTE_WITH_PARAMS(section, SECTION)
#endif
#ifndef __malloc
    #define __malloc ATTRIBUTE(malloc)
#endif
#ifndef __freefunc
    #define __freefunc(FUNC) ATTRIBUTE_WITH_PARAMS(malloc, FUNC)
#endif
#ifndef __fmtfunc
    #define __fmtfunc(FUNCLIKE, FORMAT_INDEX, FIRST_TO_CHECK) ATTRIBUTE_WITH_PARAMS(format, PP_CAT(__, PP_CAT(FUNCLIKE, __)), FORMAT_INDEX, FIRST_TO_CHECK)
#endif
#ifndef __pure
    #define __pure ATTRIBUTE(pure)
#endif
#ifdef __const
    #undef __const
#endif
#define __const ATTRIBUTE(const)
#ifndef __noreturn
    #include <stdnoreturn.h>
    #define __noreturn noreturn
#endif
#ifndef __deprecated
    #define __deprecated ATTRIBUTE(deprecated)
#endif
#ifndef __deprecated_msg
    #define __deprecated_msg(MSG) ATTRIBUTE_WITH_PARAMS(deprecated, MSG)
#endif
#ifndef __flatten
    #define __flatten ATTRIBUTE(flatten)
#endif
#ifndef __always_inline
    #define __always_inline inline ATTRIBUTE(always_inline)
#endif
#ifndef __noinline
    #define __noinline ATTRIBUTE(noinline)
#endif
// Useless macro for this project since raspberry pi pico doesn't support shared libraries
#ifndef __symver
    #ifdef __ELF__
        #define __symver(SYMBOL, ISDEFAULT, LIBNAME, VERSTR)                                                    \
            CHAOS_PP_VARIADIC_IF(CHAOS_PP_BOOL(ISDEFAULT))                                                      \
            (                                                                                                   \
                ATTRIBUTE_WITH_PARAMS(                                                                          \
                    symver,                                                                                     \
                    STRCATIFY(CHAOS_PP_PUSH(SYMBOL, CHAOS_PP_PUSH(@, CHAOS_PP_PUSH(@, LIBNAME))), _) VERSTR     \
                )                                                                                               \
            )                                                                                                   \
            (                                                                                                   \
                ATTRIBUTE_WITH_PARAMS(                                                                          \
                    symver,                                                                                     \
                    STRCATIFY(CHAOS_PP_PUSH(SYMBOL, CHAOS_PP_PUSH(@, LIBNAME)), _) VERSTR                       \
                )                                                                                               \
            )
    #else
        #define __symver(SYMBOL, ISDEFAULT, LIBNAME, VERSTR)
    #endif
#endif
#ifndef __cold
    #define __cold ATTRIBUTE(cold)
#endif
#ifndef __hot
    #define __hot ATTRIBUTE(hot)
#endif
#ifndef __fallthrough
    #define __fallthrough ATTRIBUTE(fallthrough)
#endif
#ifndef __unreachable
    #define __unreachable() __builtin_unreachable()
#endif
#ifndef __assume
    #define __assume(EXPR) ATTRIBUTE_WITH_PARAMS(assume, !!(EXPR))
#endif
#ifndef __nonnull_params
    #define __nonnull_params(...) CHAOS_PP_EXPR(CHAOS_PP_SEQ_FOR_EACH(ATTRIBUTE_WITH_PARAMS_(nonnull, _1), TO_SEQ(__VA_ARGS__)))
#endif
#ifndef __wna
    #define __wna(ALIGN) ATTRIBUTE_WITH_PARAMS(warn_if_not_aligned, ALIGN)
#endif
#ifndef __mallocsize
    #define __mallocsize(SIZE) ATTRIBUTE_WITH_PARAMS(alloc_size, SIZE)
#endif
#ifndef __callocsize
    #define __callocsize(COUNT, ELEMSIZE) ATTRIBUTE_WITH_PARAMS(alloc_size, COUNT, ELEMSIZE)
#endif
#ifndef __allocalign
    #define __allocalign(ALIGN) ATTRIBUTE_WITH_PARAMS(alloc_align, ALIGN)
#endif
#ifndef __copy_attr
    #define __copy_attr(TYPE) ATTRIBUTE_WITH_PARAMS(copy, TYPE)
#endif
#ifndef __api_change
    #define __api_change(OLD, NEW) ATTRIBUTE(unavailable) __deprecated_msg(STRINGIFY(OLD) " has been deprecated. Use " STRINGIFY(NEW) " instead.")
#endif
#ifndef __may_alias
    #define __may_alias ATTRIBUTE(may_alias)
#endif
#ifndef __aliases
    #define __aliases(VAR) ATTRIBUTE(may_alias) __copy_attr(VAR)
#endif
#ifndef __use_designated
    #define __use_designated ATTRIBUTE(designated_init)
#endif
#ifndef __little_endian
    #define __little_endian ATTRIBUTE_WITH_PARAMS(scalar_storage_order, "little-endian")
#endif
#ifndef __big_endian
    #define __big_endian ATTRIBUTE_WITH_PARAMS(scalar_storage_order, "big-endian")
#endif
#ifndef __mk_alias
    #define __mk_alias(ALIAS, TARGET) ATTRIBUTE_WITH_PARAMS(alias, STRINGIFY(TARGET)) __copy_attr(ALIAS) ALIAS
#endif
#ifndef __buffer_type
    #define __buffer_type ATTRIBUTE(nonstring)
#endif
#ifndef __cleanup
    #define __cleanup(FUNC) ATTRIBUTE_WITH_PARAMS(cleanup, FUNC)
#endif
#ifndef __common
    #define __common ATTRIBUTE(common)
#endif
#ifndef __nocommon
    #define __nocommon ATTRIBUTE(nocommon)
#endif
#ifndef __param_access
    #define __param_access(ACCESSMODE, PARAMINDEX, ...) ATTRIBUTE_WITH_PARAMS(access, ACCESSMODE, PARAMINDEX, __VA_ARGS__)
#endif
#ifndef __ro_param
    #define __ro_param(PARAMINDEX, ...) __param_access(read_only, PARAMINDEX, __VA_ARGS__)
#endif
#ifndef __wo_param
    #define __wo_param(PARAMINDEX, ...) __param_access(write_only, PARAMINDEX, __VA_ARGS__)
#endif
#ifndef __rw_param
    #define __rw_param(PARAMINDEX, ...) __param_access(read_write, PARAMINDEX, __VA_ARGS__)
#endif
#ifndef __no_access_param
    #define __no_access_param(PARAMINDEX, ...) __param_access(none, PARAMINDEX, __VA_ARGS__)
#endif
#ifndef __isr
    #define __isr ATTRIBUTE(interrupt)
#endif
#ifndef __interrupt
    #define __interrupt ATTRIBUTE(interrupt) __isr
#endif
#ifndef __zero_regs
    #define __zero_regs(WHICHONES) ATTRIBUTE_WITH_PARAMS(zero_call_used_regs, WHICHONES)
#endif
#ifndef __zero_used_regs
    #define __zero_used_regs __zero_regs("used")
#endif
#ifndef __zero_all_regs
    #define __zero_all_regs __zero_regs("all")
#endif
#ifndef __malloca
    #define __malloca(SIZE) __builtin_alloca_with_align((SIZE), __alignof__(max_align_t))
#endif
#ifndef __malloca_aligned
    #define __malloca_aligned(SIZE, ALIGN) __builtin_alloca_with_align((SIZE), (ALIGN))
#endif
#ifndef __calloca
    #define __calloca(COUNT, ELEMSIZE) picoutil_memset_explicit(__builtin_alloca_with_align((COUNT) * (ELEMSIZE), __alignof__(max_align_t)), 0, (COUNT) * (ELEMSIZE))
#endif
#ifndef __calloca_aligned
    #define __calloca_aligned(COUNT, ELEMSIZE, ALIGN) picoutil_memset_explicit(__builtin_alloca_with_align((COUNT) * (ELEMSIZE), (ALIGN)), 0, (COUNT) * (ELEMSIZE))
#endif
#ifndef __time_critical_func
    #define __time_critical_func(FUNC) __not_in_flash_func(FUNC)
#endif
#ifndef __min_size
    // Te be used in array function parameters declaration / definition (e.g. `void foo(int array[__min_size(10)]);`)
    #define __min_size(SIZE) static (SIZE)
#endif
#ifndef __DEQUALIFY
    #define __DEQUALIFY(TYPE, VAR) ((VAR)(uintptr_t)(const volatile void *)(VAR))
#endif
#ifdef DEQUALIFY_PTR
    #undef DEQUALIFY_PTR
#endif
#define DEQUALIFY_PTR(VAR) __DEQUALIFY(__typeof__(VAR), VAR)

#ifndef __offsetof
    #define __offsetof(TYPE, MEMBER) offsetof(TYPE, MEMBER)
#endif

#ifndef __containerof
    // Meant to be used like this: `__containerof(struct_ptr, struct_type, member_name)`
    #define __containerof(x, s, m) ({					            \
    const volatile __typeof(((s *)0)->m) *__x = (x);		        \
    __DEQUALIFY(s *, (const volatile char *)__x - __offsetof(s, m));\
})
#endif
#ifndef __assume_aligned
    #define __assume_aligned(PTR, ALIGN) __builtin_assume_aligned((PTR), (ALIGN))
#endif
#ifndef __artificial
    #define __artificial ATTRIBUTE(artificial)
#endif

#ifdef ATTRIBUTE_ID
    #undef ATTRIBUTE_ID
#endif
#define ATTRIBUTE_ID() ATTRIBUTE

#ifdef ATTRIBUTE_WITH_PARAMS_ID
    #undef ATTRIBUTE_WITH_PARAMS_ID
#endif
#define ATTRIBUTE_WITH_PARAMS_ID() ATTRIBUTE_WITH_PARAMS

#ifdef ATTRIBUTE_
    #undef ATTRIBUTE_
#endif
#define ATTRIBUTE_ CHAOS_PP_LAMBDA(ATTRIBUTE_ID)()

#ifdef ATTRIBUTE_WITH_PARAMS_
    #undef ATTRIBUTE_WITH_PARAMS_
#endif
#define ATTRIBUTE_WITH_PARAMS_ CHAOS_PP_LAMBDA(ATTRIBUTE_WITH_PARAMS_ID)()

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
#if !CHAOS_PP_VARIADICS
    #define PP_CAT(X, Y) PP_CAT_PRIMITIVE(X, Y)
#else
    #define PP_CAT(...) PP_CAT_PRIMITIVE(__VA_ARGS__)
#endif

#ifdef PP_CAT_PRIMITIVE
    #undef PP_CAT_PRIMITIVE
#endif
#if !CHAOS_PP_VARIADICS
    #define PP_CAT_PRIMITIVE(X, Y) X ## Y
#else
    #define PP_CAT_PRIMITIVE(...) CHAOS_PP_VARIADIC_CAT(__VA_ARGS__)
#endif

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

#define byte_t __buffer_type byte_t

void      picoutil_static_allocator_init(bool safe);
bool      picoutil_static_allocator_set_safe(bool safe);
bool      picoutil_static_allocator_is_safe(void);

__malloc __wur __mallocsize(1) __allocalign(2)
void*     __time_critical_func(picoutil_static_alloc_aligned)(size_t size, size_t requested_align);
__malloc __wur __mallocsize(1)
void*     __time_critical_func(picoutil_static_alloc)(size_t size);
__malloc __wur __callocsize(1, 2) __allocalign(3)
void*     __time_critical_func(picoutil_static_calloc_aligned)(size_t count, size_t size, size_t requested_align);
__malloc __wur __callocsize(1, 2)
void*     __time_critical_func(picoutil_static_calloc)(size_t count, size_t size);
__wur
void*     __time_critical_func(picoutil_static_realloc_aligned)(void* ptr, size_t size, size_t requested_align);
__wur
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

__artificial __always_inline
static inline void picoutil_memset_explicit(void* ptr, byte_t value, size_t size)
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

__artificial __always_inline
static inline void __time_critical_func(picoutil_memory_barrier)(enum barrier_option option)
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

__artificial __always_inline
static inline void __time_critical_func(picoutil_sync_barrier)(enum barrier_target target, enum barrier_option option)
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
    aes_key_t* key;
    aes_key_expanded_t* key_expanded;
    aes_block_t* iv;
    aes_mode mode;
    aes_dir dir;
    aes_key_size key_size;
    aes_block_size block_size;
} aes_context_t;

typedef struct aes_result
{
    byte_t* data;
    size_t data_size;
    bool error;
} aes_result_t;

void picoutil_aes_init(void);

bool picoutil_aes_key_init(aes_key_t* key, aes_key_size ksize, byte_t* buf, size_t bufsize);
__sentinel __zero_used_regs
bool picoutil_aes_context_init_impl(aes_context_t* ctx, aes_mode mode, aes_dir dir, aes_key_size key_size, aes_block_size block_size, ...);
#ifdef picoutil_aes_context_init
    #undef picoutil_aes_context_init
#endif
/**
 * @brief Initialize an AES context
 * @param CTX The context pointer to initialize
 * @param MODE The mode of operation
 * @param DIR The direction of operation
 * @param KEY_SIZE The key size
 * @param BLOCK_SIZE The block size
 * @param ... The key and IV (if needed) (key_size and block_size bytes long, respectively)
 * @return true if the context was initialized successfully, false otherwise
 * @note The key and IV are copied to the context, so they can be freed after the call. They are assumed to be of type `aes_key_t*` and `aes_block_t*`, respectively
 */
#define picoutil_aes_context_init(CTX, MODE, DIR, KEY_SIZE, BLOCK_SIZE, ...) aes_context_init_impl((CTX), (MODE), (DIR), (KEY_SIZE), (BLOCK_SIZE), __VA_ARGS__, NULL)
void picoutil_aes_context_deinit(aes_context_t* ctx);

__wur __sentinel __zero_used_regs
aes_result_t __time_critical_func(picoutil_aes_process_impl)(aes_context_t* ctx, byte_t* data, size_t data_size, ...);
#ifdef picoutil_aes_process
    #undef picoutil_aes_process
#endif
/**
 * @brief Process data with an AES context
 * @param CTX The context to use
 * @param DATA The data to process
 * @param DATA_SIZE The size of the data to process
 * @param ... The key and IV (if not already set in the context) (key_size and block_size bytes long, respectively)
 * @return The processed data
 * @note The optional key and IV are not copied to the context and are priviledged over the ones in the context if any. They are assumed to be of type `aes_key_t*` and `aes_block_t*`, respectively
 */
#define picoutil_aes_process(CTX, DATA, DATA_SIZE, ...) picoutil_aes_process_impl((CTX), (DATA), (DATA_SIZE), __VA_ARGS__, NULL)
// aes_block_t __time_critical_func(picoutil_aes_encrypt_block)(aes_block_t block, aes_key_t key);
// aes_block_t __time_critical_func(picoutil_aes_encrypt_block_until)(aes_block_t block, aes_key_t key, size_t num_round);

__zero_used_regs
void picoutil_test_encryption_ecb_mode(size_t num_rounds);

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

__artificial __const __always_inline
static inline uint32_t picoutil_rotate_right_32(uint32_t x, uint32_t y)
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

__artificial __const __always_inline
static inline uint32_t picoutil_rotate_left_32(uint32_t x, uint32_t y)
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
    #define TEST_PREPROCESSOR 1
    
    #undef POW
    #define POW(...) 0
    #undef CHAOS_PP_EXPR
    #define CHAOS_PP_EXPR(...)
#endif

#if defined(TEST_PREPROCESSOR) && TEST_PREPROCESSOR
/*
 * TODO: Write a `VA_RANGE` that could be called like this:
 * `VA_RANGE(FROM, TO)(...)`
 */
VA_RANGE(0, 2, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p)

CHAOS_PP_EXPAND(CHAOS_PP_INVOKE(_0 CHAOS_PP_LAMBDA(+) _1 CHAOS_PP_LAMBDA(&&) CHAOS_PP_INC_(_1), 1, 2))

__symver(picoutil_aes_init, 1, PICOUTIL, "1.0.0")

__nonnull_params(0)
__nonnull_params(1, 2, 3)
#endif