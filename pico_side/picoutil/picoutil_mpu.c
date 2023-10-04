// TODO: Think about how the memory regions should be defined and restricted, etc.

#include <picoutil.h>

__restore_macro(__always_inline)

// Put includes here
#include <hardware/address_mapped.h>
#include <hardware/regs/addressmap.h>
#include <hardware/regs/m0plus.h>
#include <hardware/structs/mpu.h>

#include <stdbool.h>

#include <picoutil_fix_macros.h>

#ifdef PICOUTIL_MPU_MAX_REGIONS
    #undef PICOUTIL_MPU_MAX_REGIONS
#endif
#define PICOUTIL_MPU_MAX_REGIONS 8

/**
 * @brief A memory region as those used by the ARMv6-M MPU
 * 
 */
struct memory_region
{
    uint32_t base;
    uint32_t size;
    uint32_t attr;
    struct memory_subregion
    {
        uint32_t base;
        uint32_t size;
    } subregions[8];
};

enum mpu_attribute
{
    MPU_ATTR_XN = 1 << 28, // eXecute Never
    MPU_ATTR_AP = 1 << 24, // Access Permissions
    MPU_ATTR_TEX = 1 << 19, // Type Extension
    MPU_ATTR_S = 1 << 18, // Shareable
    MPU_ATTR_C = 1 << 17, // Cacheable
    MPU_ATTR_B = 1 << 16, // Bufferable
    MPU_ATTR_SRD = 0xFF << 8, // Sub-Region Disable
    MPU_ATTR_SIZE = 0x1F << 1, // Region Size
    MPU_ATTR_ENABLE = 1 << 0, // Region Enable
};

__unused
static struct memory_region regions[PICOUTIL_MPU_MAX_REGIONS] = {
    [0] = /* Cortex-m0+ register region */
    {
        .base = (uint32_t)(PPB_BASE),
        .size = (uint32_t)((PPB_BASE + M0PLUS_MPU_RASR_OFFSET + sizeofexpr(mpu_hw->rasr)) - (PPB_BASE)),
        .attr = MPU_ATTR_XN | MPU_ATTR_AP | MPU_ATTR_TEX | MPU_ATTR_S | MPU_ATTR_C | MPU_ATTR_B | MPU_ATTR_SRD | MPU_ATTR_SIZE | MPU_ATTR_ENABLE,
        .subregions = {
            [0] = /* MPU subregion */
            {
                .base = (uint32_t)(PPB_BASE + M0PLUS_MPU_TYPE_OFFSET),
                .size = (uint32_t)((PPB_BASE + M0PLUS_MPU_RASR_OFFSET + sizeofexpr(mpu_hw->rasr)) - (PPB_BASE + M0PLUS_MPU_TYPE_OFFSET)),
            },
        },
    },
};

bool picoutil_is_mpu_active(void)
{
    return (mpu_hw->ctrl & M0PLUS_MPU_CTRL_ENABLE_BITS) != 0;
}

void picoutil_mpu_enable(void)
{
    hw_set_bits(&mpu_hw->ctrl, M0PLUS_MPU_CTRL_ENABLE_BITS);
}

void picoutil_mpu_disable(void)
{
    hw_clear_bits(&mpu_hw->ctrl, M0PLUS_MPU_CTRL_ENABLE_BITS);
}
