#ifndef PICOFLASH_H
#define PICOFLASH_H


// This feature is harder to implement than I thought
#define PICOFLASH_DISABLE_THREAD_STATUS_AUTO_UPDATE
#ifdef PICOFLASH_ONE_THREAD
    #undef PICOFLASH_DISABLE_THREAD_STATUS_AUTO_UPDATE
    #define PICOFLASH_DISABLE_THREAD_STATUS_AUTO_UPDATE
#endif

#ifdef ATTRIBUTE
    #undef ATTRIBUTE
#endif
#ifdef ATTRIBUTE_WITH_PARAMS
    #undef ATTRIBUTE_WITH_PARAMS
#endif
#ifndef __GNUC__
    #define ATTRIBUTE(X)
    #define ATTRIBUTE_WITH_PARAMS(X, ...)
#else
    #define ATTRIBUTE(X) __attribute__((PP_CAT(__, PP_CAT(X, __))))
    #define ATTRIBUTE_WITH_PARAMS(X, ...) __attribute__((PP_CAT(__, PP_CAT(X, __))(__VA_ARGS__)))
#endif

#ifndef _GNU_SOURCE
    #define _GNU_SOURCE 1
#endif

#include <picoutil.h>

#include <third_party/map_macro/map.h>
#include <third_party/chaos-pp/chaos/preprocessor.h>
#include <pico/platform.h>
#include <hardware/sync.h>

#include <stdint.h>
#include <stdbool.h>
#include <signal.h>

static inline
uint32_t picoflash_disable_interrupts(void)
{
    uint32_t ret = save_and_disable_interrupts();
    picoutil_sync_barrier(BARRIER_DATA, BARRIER_SY);
    picoutil_sync_barrier(BARRIER_INS, BARRIER_SY);
    return ret;
}

static inline
void picoflash_restore_interrupts(uint32_t state)
{
    picoutil_sync_barrier(BARRIER_DATA, BARRIER_SY);
    picoutil_sync_barrier(BARRIER_INS, BARRIER_SY);
    restore_interrupts(state);
}

bool picoflash_init(bool multicore, bool multithread);

#if defined(PICOFLASH_SOURCE) && PICOFLASH_SOURCE
    #ifdef PICOFLASH_DISABLE_THREAD_STATUS_AUTO_UPDATE
        #undef picoflash_auto_update_threaded_status
        #define picoflash_auto_update_threaded_status(...) ((void)0)
    #else
        void picoflash_auto_update_threaded_status(void);
    #endif
#endif

#ifdef PICOFLASH_UNIMPLEMENTED_SYMBOLS
bool picoflash_iolock_init(void);

bool picoflash_lock_whole(void);
bool picoflash_trylock_whole(void);
bool picoflash_unlock_whole(void);

bool picoflash_read(uint32_t addr, uint8_t *buf, size_t len);
bool picoflash_read_unlocked(uint32_t addr, uint8_t *buf, size_t len);

bool picoflash_write(uint32_t addr, const uint8_t *buf, size_t len);
bool picoflash_write_unlocked(uint32_t addr, const uint8_t *buf, size_t len);

bool picoflash_erase(uint32_t addr, size_t len);
bool picoflash_erase_unlocked(uint32_t addr, size_t len);

bool picoflash_erase_ranges(const uint64_t *ranges, size_t num_ranges);
bool picoflash_erase_ranges_unlocked(const uint64_t *ranges, size_t num_ranges);

bool picoflash_write_ranges(const uint64_t *ranges, const uint8_t *buf, size_t num_ranges);
bool picoflash_write_ranges_unlocked(const uint64_t *ranges, const uint8_t *buf, size_t num_ranges);

#ifdef PF_MEM_RANGE
    #undef PF_MEM_RANGE
#endif
#define PF_MEM_RANGE(FROM, TO) (((uint64_t)(FROM) << 32) | (TO))

#endif

#endif