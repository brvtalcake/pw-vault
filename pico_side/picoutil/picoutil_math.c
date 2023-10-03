#include <picoutil.h>

__restore_macro(__always_inline)

#include <pico/rand.h>
#include <hardware/structs/rosc.h>

#include <picoutil_fix_macros.h>

__const
uint64_t powu64(uint64_t base, uint64_t exp)
{
    if (exp == 0 && base == 0)
        return 0;
    if (exp == 0)
        return 1;
    if (exp == 1)
        return base;
    if (exp % 2 == 0)
        return powu64(base * base, exp / 2);
    else
        return base * powu64(base * base, (exp - 1) / 2);
}

__const
int64_t pows64(int64_t base, int64_t exp)
{
    if ((exp == 0 && base == 0) || exp < 0)
        return 0;
    if (exp == 0)
        return 1;
    if (exp == 1)
        return base;
    if (exp % 2 == 0)
        return pows64(base * base, exp / 2);
    else
        return base * pows64(base * base, (exp - 1) / 2);
}

bool picoutil_randbit(void)
{
    // TODO: Make use of `rosc_hw->randombit` instead of `get_rand64()`

    // For now, we'll just go with the default SDK implementation
    return get_rand_64() & 1;
}

// TODO: All the `picoutil_rand` functions shall use `rosc_hw->randombit` (via `picoutil_randbit`)
uint8_t picoutil_rand8(void)
{
    return (uint8_t)(get_rand_32() & 0xFF);
}

uint16_t picoutil_rand16(void)
{
    return (uint16_t)(get_rand_32() & 0xFFFF);
}

uint32_t picoutil_rand32(void)
{
    return get_rand_32();
}

uint64_t picoutil_rand64(void)
{
    return get_rand_64();
}

uint64_t picoutil_randn(uint64_t n)
{
    uint64_t result = 0;
    for (uint64_t i = 0; i < n; i++)
    {
        result |= (uint64_t)picoutil_randbit() << i;
    }
    return result;
}
