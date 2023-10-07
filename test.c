#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdarg.h>

#if 0
[[nodiscard]] __attribute__((warn_unused_result))
void* test_func(void)
{
    return NULL;
}
_Static_assert(__builtin_has_attribute(test_func, warn_unused_result), "test_func should have warn_unused_result attribute"); // ok
_Static_assert(__builtin_has_attribute(test_func, nodiscard), "test_func should have nodiscard attribute"); // fails

int main(void)
{
    void* p = test_func();
    (void)p;
    return 0;
}
#else

intptr_t foo(void* ptr, intptr_t i, intptr_t j, ...)
{
    (void)ptr;
    printf("i == %" PRIdPTR "\n", i);
    printf("j == %" PRIdPTR "\n\n", j);
    return i + j;
}

void* test(void* f, ...)
{
    va_list args;
    va_start(args, f);
    size_t n = 0;
    while (va_arg(args, void*) != (void*)-1)
        ++n;
    va_end(args);
    printf("n == %zu\n", n);
    __builtin_return(__builtin_apply((void (*)())f, __builtin_apply_args(), (n + 1) * 8));
}

#define PP_CAT(a, b) CAT_PRIMITIVE(a, b)
#define CAT_PRIMITIVE(a, b) a##b

#define APPLY(FUNC, STACKSIZE, ...)                                                                     \
    ({                                                                                                  \
        uint64_t PP_CAT(apply,__LINE__)(...)                                                            \
        {                                                                                               \
            __builtin_return(__builtin_apply((void (*)())(FUNC), __builtin_apply_args(), (STACKSIZE))); \
        }                                                                                               \
        auto PP_CAT(ret, __LINE__) =  PP_CAT(apply, __LINE__)(__VA_ARGS__);                         \
        PP_CAT(ret, __LINE__);                                                                          \
    })
    
#define APPLY_WITH_ARGS(FUNC, STACKSIZE, ARGS)                                                                     \
    ({                                                                                                  \
        uint64_t PP_CAT(apply,__LINE__)(void)                                                            \
        {                                                                                               \
            __builtin_return(__builtin_apply((void (*)())(FUNC), ARGS, (STACKSIZE))); \
        }                                                                                               \
        auto PP_CAT(ret, __LINE__) =  PP_CAT(apply, __LINE__)();                         \
        PP_CAT(ret, __LINE__);                                                                          \
    })

void print_buf(uint8_t* buf, size_t size)
{
    for (size_t i = 0; i < size; ++i)
        printf((i + 1) % 4 == 0 ? "%02x\n" : "%02x ", buf[i]);
    printf("\n");
}

#include <float.h>

int main(void)
{
    test((void*)foo, 42, 50, (void*)-1);
    //test((void*)write, 1, "Hello, world!\n", 14, (void*)-1);
    void lambda_test(int fd, ...)
    {
        __builtin_return(__builtin_apply((void (*)())write, __builtin_apply_args(), 3 * 8));
    }
    lambda_test(1, "Hello, world!\n", 14);
    APPLY(foo, 16, NULL, 42, 50);

    void* test2(...)
    {
        return __builtin_apply_args();
    }
    void* args = test2("Here are some numbers: %d, %d, %d\n", (int)42, (int)50, (int)100);
    __builtin_apply((void (*)())printf, args, 3 * sizeof(int));

    APPLY_WITH_ARGS(printf, 3 * sizeof(int), args);

    uint8_t buf[16] = { 0 };
    print_buf(buf, sizeof(buf));
    APPLY(&memcpy, 5, buf + 4, "Hello", 5);
    print_buf(buf, sizeof(buf));

    _Float16 b = 0.5;
    _Decimal32 a = 0.5;
    printf("%zu  %zu  %zu  %zu  %zu  %zu\n", sizeof(b), sizeof(float), sizeof(double), sizeof(__float80), sizeof(long double), sizeof(_Float128));

    return 0;
}

#endif