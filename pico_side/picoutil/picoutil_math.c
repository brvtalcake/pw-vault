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

bool picoutil_randbit(bool raw)
{
    rosc_hw_t* rosc = rosc_hw;
    if (raw)
        return rosc->randombit & ROSC_RANDOMBIT_BITS;
    else
        return get_rand_64() & 1u;
}

// TODO: All the `picoutil_rand` functions shall use `rosc_hw->randombit` (via `picoutil_randbit`)
uint8_t picoutil_rand8(bool raw)
{
    if (raw)
    {
        uint8_t result = 0;
        __can_vectorize(1)
        for (uint8_t i = 0; i < 8; i++)
        {
            result |= (uint8_t)picoutil_randbit(raw) << i;
        }
        return result;
    }
    else
        return (uint8_t)(get_rand_32() & UINT8_C(0xFF));
}

uint16_t picoutil_rand16(bool raw)
{
    if (raw)
    {
        uint16_t result = 0;
        __can_vectorize(1)
        for (uint8_t i = 0; i < 16; i++)
        {
            result |= (uint16_t)picoutil_randbit(raw) << i;
        }
        return result;
    }
    else
        return (uint16_t)(get_rand_32() & UINT16_C(0xFFFF));
}

uint32_t picoutil_rand32(bool raw)
{
    if (raw)
    {
        uint32_t result = 0;
        __can_vectorize(1)
        for (uint8_t i = 0; i < 32; i++)
        {
            result |= (uint32_t)picoutil_randbit(raw) << i;
        }
        return result;
    }
    else
        return get_rand_32();
}

uint64_t picoutil_rand64(bool raw)
{
    if (raw)
    {
        uint64_t result = 0;
        __can_vectorize(1)
        for (uint8_t i = 0; i < 64; i++)
        {
            result |= (uint64_t)picoutil_randbit(raw) << i;
        }
        return result;
    }
    else
        return get_rand_64();
}

uint64_t picoutil_randn(uint8_t n, bool raw)
{
    uint64_t result = 0;
    __can_vectorize(1)
    for (uint8_t i = 0; i < n; i++)
    {
        result |= (uint64_t)picoutil_randbit(raw) << i;
    }
    return result;
}
