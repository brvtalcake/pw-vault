#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

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
