#include <picoutil.h>

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