#if 0

#define PICOFLASH_SOURCE 1
#include <picoflash.h>

#ifndef _GNU_SOURCE
    #define _GNU_SOURCE 1
#endif

#include <stdatomic.h>

#include <hardware/regs/addressmap.h>
#include <hardware/flash.h>
#include <hardware/sync.h>
#include <hardware/regs/xip.h>
#include <hardware/structs/xip_ctrl.h>

#include <pico/platform.h>
#include <pico/stdlib.h>
#include <pico/flash.h>
#include <pico/sync.h>
#include <pico/multicore.h>


static atomic_bool picoflash_initialized = false;
#ifndef PICOFLASH_DISABLE_THREAD_STATUS_AUTO_UPDATE
static atomic_bool pico_multithread = false;
static atomic_bool pico_multicore = false;
#endif

static inline 
bool flash_init()
{
    assert(false && "flash_init() not implemented");
}

static inline
bool is_flush_possible(void)
{
    picoflash_auto_update_threaded_status();
    io_ro_32 ret = atomic_load_explicit(&(xip_ctrl_hw->stat), memory_order_acquire);
    return ((ret & XIP_STAT_FLUSH_RDY) == XIP_STAT_FLUSH_RDY);
}

static inline
io_rw_32 primitive_flush(void)
{
    picoflash_auto_update_threaded_status();
    atomic_bool threaded = atomic_load_explicit(&pico_multithread, memory_order_acquire);
    if (threaded)
    {
        xip_ctrl_hw_t* ptr = xip_ctrl_hw;
        io_rw_32 desired = 0x1U;
        io_rw_32 ret = atomic_exchange_explicit(&(ptr->flush), desired, memory_order_release);
        return ret;
    }
    else
    {
        xip_ctrl_hw_t* ptr = xip_ctrl_hw;
        io_rw_32 desired = 0x1U;
        io_rw_32 ret = ptr->flush;
        ptr->flush = desired;
        return ret;
    }
    __builtin_unreachable();
    return (io_rw_32)(-1);
}

#undef WAIT_AND_FLUSH
#define WAIT_AND_FLUSH(...)             \
    do {                                \
        while (!is_flush_possible())    \
        {                               \
            tight_loop_contents();      \
        }                               \
        primitive_flush();              \
        __VA_ARGS__;                    \
    } while (false)

#ifndef PICOFLASH_DISABLE_THREAD_STATUS_AUTO_UPDATE
void picoflash_auto_update_threaded_status(void)
{
    if (atomic_load_explicit(&picoflash_initialized, memory_order_acquire))
    {
        if (atomic_load_explicit(&pico_multicore, memory_order_acquire) || atomic_load_explicit(&pico_multithread, memory_order_acquire))
            return;
        if (!atomic_load_explicit(&pico_multicore, memory_order_acquire))
        {
            uint ret = get_core_num();
            if (ret > 0)
                atomic_store_explicit(&pico_multicore, true, memory_order_release);
            if (0)
                {/* Find a way to detect multithreading / multicore env */}

        }
        if (!atomic_load_explicit(&pico_multithread, memory_order_acquire))
        {
            if (0)
                {/* Find a way to detect multithreading / multicore env */}
        }
    }
}
#endif

void picoflash_flush_cache(void)
{
    picoflash_auto_update_threaded_status();
    WAIT_AND_FLUSH();
}

bool picoflash_init(bool multicore, bool multithread)
{
    atomic_exchange(&picoflash_initialized, true);
#ifndef PICOFLASH_DISABLE_THREAD_STATUS_AUTO_UPDATE
    atomic_exchange(&pico_multithread, multithread);
    atomic_exchange(&pico_multicore, multicore);
#endif
    return flash_init() && picoflash_iolock_init();
}

#undef WAIT_AND_FLUSH

#endif