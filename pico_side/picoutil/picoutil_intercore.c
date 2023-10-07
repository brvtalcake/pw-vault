#include <picoutil.h>

__restore_macro(__always_inline)

// Put includes here
#include <hardware/exception.h>
#include <hardware/irq.h>
#include <pico/bootrom.h>
#include <pico/util/queue.h>
#include <pico/multicore.h>
#include <pico/sync.h>

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <picoutil_fix_macros.h>

#ifndef PICOUTIL_INTERCORE_LOCKOUT_MAGIC_START
    #define PICOUTIL_INTERCORE_LOCKOUT_MAGIC_START UINT32_C(0xdeadbeef)
#endif
#ifndef PICOUTIL_INTERCORE_LOCKOUT_MAGIC_END
    #define PICOUTIL_INTERCORE_LOCKOUT_MAGIC_END UINT32_C(0xfeedface)
#endif
#ifndef PICOUTIL_INTERCORE_TASK_RDY_MAGIC
    #define PICOUTIL_INTERCORE_TASK_RDY_MAGIC UINT32_C(0xfacef00d)
#endif
#ifndef PICOUTIL_INTERCORE_RES_RDY_MAGIC
    #define PICOUTIL_INTERCORE_RES_RDY_MAGIC UINT32_C(0xdeadf00d)
#endif
#ifndef PICOUTIL_INTERCORE_OK_MAGIC
    #define PICOUTIL_INTERCORE_OK_MAGIC UINT32_C(0x0badface)
#endif

#ifdef APPLY
    #undef APPLY
#endif
// A little bit of black magic from GCC and C23 here
#define APPLY(FUNC, STACKSIZE, ...)                                                                     \
    ({                                                                                                  \
        int64_t UNIQUE(apply)(...)                                                                      \
        {                                                                                               \
            __builtin_return(__builtin_apply((void (*)())(FUNC), __builtin_apply_args(), (STACKSIZE))); \
        }                                                                                               \
        auto UNIQUE(ret) =  UNIQUE(apply)(__VA_ARGS__);                                                 \
        UNIQUE(ret);                                                                                    \
    })

#define APPLY_ARGS(FUNC, STACKSIZE, ARGS)                                               \
    ({                                                                                  \
        int64_t UNIQUE(apply)(void)                                                     \
        {                                                                               \
            __builtin_return(__builtin_apply((void (*)())(FUNC), (ARGS), (STACKSIZE))); \
        }                                                                               \
        auto UNIQUE(ret) =  UNIQUE(apply)();                                            \
        UNIQUE(ret);                                                                    \
    })

#ifndef PICOUTIL_CORE1_FAILURE
    #define PICOUTIL_CORE1_FAILURE INT_MIN
#endif

#if 0
// Can't work because __builtin_apply_args() allocate args on stack
void* picoutil_get_applicable_args(...)
{ return __builtin_apply_args(); }
#endif

struct core1_task
{
    int (*func)(void*);
    void* args;
    void* ret_mem;
    size_t ret_size;

    int exit_code;
};
typedef struct core1_task core1_task_t;

struct core1_result
{
    void* ret_mem;
    size_t ret_size;
  
    int exit_code;
};
typedef struct core1_result core1_result_t;

struct core1_task_units
{
    enum
    {
        CORE1_TASK_FUNC,
        CORE1_TASK_ARGS,
        CORE1_TASK_RET_MEM,
        CORE1_TASK_RET_SIZE
    } tag;
    union
    {
        int (*func)(void*);
        void* args;
        void* ret_mem;
        size_t ret_size;
    } payload;
};
typedef struct core1_task_units core1_task_units_t;

// For use by core 1 only.
// This is a `queue_t` of `struct core1_task`.
// It may be needed to enqueue tasks in core 1 if one arrives before the previous one is finished, hence the necessity of this queue.
static queue_t core1_task_queue = { 0 };

// Core 1 will put results here, core 0 will read them.
// This is a `queue_t` of `struct core1_result`.
// As soon as core 1 finished sending a result in this queue, it has to seen a `PICOUTIL_INTERCORE_RES_RDY_MAGIC` signal in the intercore FIFO.
static queue_t result_queue = { 0 };

// Core 0 will put tasks here, core 1 will read them.
// This is a `queue_t` of `struct core1_task_units`.
// As soon as core 0 finished sending a task in this queue, it has to seen a `PICOUTIL_INTERCORE_TASK_RDY_MAGIC` signal in the intercore FIFO.
// This queue is only a sort of communication buffer from core 0 to core 1.
static queue_t incoming_task_queue = { 0 };

// Final results from core 1 will be put here, core 0 will read them.
static queue_t core0_result_queue = { 0 };

static struct core1_task running_task = { 0 };
static bool core1_failed = false;

static bool core1_ok = false;

// To be used "internally" by the library (maybe could even be static)
bool picoutil_core1_ok(void)
{
    bool old = core1_ok;
    core1_ok = false;
    return old;
}

void picoutil_fail_core1(void)
{
    if (get_core_num() != 1)
        exit(EXIT_FAILURE);
    core1_failed = true;
    picoutil_free_all_in_core(1);
    char dummy = 0;
    picoutil_return_to_core1(dummy, PICOUTIL_CORE1_FAILURE);
}

#ifdef picoutil_return_to_core1
    #undef picoutil_return_to_core1
#endif
// To be used in functions passed to the core1 through the intercore-FIFOs, that are to be ran on it
#define picoutil_return_to_core1(REAL_RET, EXIT_CODE)                                               \
    APPLY(&memcpy, running_task.ret_size, running_task.ret_mem, &REAL_RET, running_task.ret_size);  \
    return (EXIT_CODE)

static void install_fifo_isr(void)
{
    if (get_core_num() != 1)
        exit(EXIT_FAILURE);
    irq_set_exclusive_handler(SIO_IRQ_PROC1, picoutil_isr_intercore_fifo_core1);
    irq_set_enabled(SIO_IRQ_PROC1, true);
    irq_set_priority(SIO_IRQ_PROC1, 0);
}

static inline void prepare_core1(void)
{
    if (get_core_num() != 1)
        exit(EXIT_FAILURE);
    multicore_fifo_clear_irq();
    multicore_fifo_drain();
    install_fifo_isr();

    queue_init(&core1_task_queue, sizeof(core1_task_t), PICOUTIL_MAX_CORE1_TASKS);
    queue_init(&result_queue, sizeof(core1_result_t), (PICOUTIL_MAX_CORE1_TASKS) * 2);
    queue_init(&core0_result_queue, sizeof(core1_result_t), (PICOUTIL_MAX_CORE1_TASKS) * 2);
    // Even though we only need 4 units, allocate a bit more just in case, so core 0 is less likely to block or trigger errors because of a full queue
    queue_init(&incoming_task_queue, sizeof(core1_task_units_t), 4 * 2);
}

__noreturn
void picoutil_core1_entry(void)
{
    if (get_core_num() != 1)
        picoutil_log(LOG_FATAL, "picoutil_core1_entry() should only be called from core 1");
    prepare_core1();
    // TODO: Not finished yet
    __unreachable();
}

__interrupt
void picoutil_isr_intercore_fifo_core1(void)
{
    uint32_t interrupt_state = picoutil_disable_interrupts();
    if (get_core_num() != 1)
    {
        picoutil_restore_interrupts(interrupt_state);
        exit(EXIT_FAILURE);
    }
    if (UNLIKELY(!multicore_fifo_rvalid()))
        goto end;
    uint32_t magic = multicore_fifo_pop_blocking();
    switch (magic)
    {
        case PICOUTIL_INTERCORE_LOCKOUT_MAGIC_START:
            // TODO: Not implemented yet. Supposed to lockout core 1 from running any task until core 0 is done with its current task
            //     and has sent a `PICOUTIL_INTERCORE_LOCKOUT_MAGIC_END` signal
            break;
        case PICOUTIL_INTERCORE_LOCKOUT_MAGIC_END:
            // TODO: See the above case
            break;
        case PICOUTIL_INTERCORE_TASK_RDY_MAGIC:
            // An incoming task from core 0 is ready to be treated
            char buf[sizeof(core1_task_units_t)] = { 0 };
            core1_task_t task = { 0 };
            while (!queue_is_empty(&incoming_task_queue))
            {
                queue_remove_blocking(&incoming_task_queue, buf);
                core1_task_units_t unit = *reinterpret_cast(core1_task_units_t*, DECAY(buf));
                switch (unit.tag)
                {
                    case CORE1_TASK_FUNC:
                        task.func = unit.payload.func;
                        break;
                    case CORE1_TASK_ARGS:
                        task.args = unit.payload.args;
                        break;
                    case CORE1_TASK_RET_MEM:
                        task.ret_mem = unit.payload.ret_mem;
                        break;
                    case CORE1_TASK_RET_SIZE:
                        task.ret_size = unit.payload.ret_size;                        
                        break;
                    default:
                        picoutil_log(LOG_ERROR, "Invalid tag in core1_task_units_t: %d", unit.tag);
                }

                // Since we empty the queue each time we get an interrupt, the queue is supposed to have only one task in it, so only 4 units
                // Hence, if we have all 4 units but still have elems in the queue, that's an error
                if (task.func && task.args && task.ret_mem && task.ret_size && !queue_is_empty(&incoming_task_queue))
                    picoutil_log(LOG_FATAL, "The incoming task queue is not supposed to have more than one task in it, or have more \"units\" than the 4 required ones");                    
            }
            if (!task.func || !task.args || !task.ret_mem || !task.ret_size)
                picoutil_log(LOG_FATAL, "A task is composed of 4 \"units\". Only %d provided", (task.func ? 1 : 0) + (task.args ? 1 : 0) + (task.ret_mem ? 1 : 0) + (task.ret_size ? 1 : 0));
            // Add `task` to `core1_task_queue`
            queue_add_blocking(&core1_task_queue, &task);
            if (UNLIKELY(!multicore_fifo_wready()))
                picoutil_log(LOG_FATAL, "Multicore FIFO is not ready to write");
            multicore_fifo_push_blocking(PICOUTIL_INTERCORE_OK_MAGIC); // We received everything 5/5
            __sev();
            break;
        case PICOUTIL_INTERCORE_RES_RDY_MAGIC:
            break; // Should not happen
        case PICOUTIL_INTERCORE_OK_MAGIC:
            break; // We don't care
        default:
            picoutil_log(LOG_ERROR, "Invalid magic number in intercore FIFO: %x", magic);
            break;
    }
    if (multicore_fifo_rvalid())
        multicore_fifo_drain();
    
end:
    uint32_t fifo_st = multicore_fifo_get_status();
    if (fifo_st & SIO_FIFO_ST_WOF_BITS || fifo_st & SIO_FIFO_ST_ROE_BITS)
    {
        picoutil_log(LOG_ERROR, "A read occurred when the FIFO was empty, or a write occurred when the FIFO was full");
        multicore_fifo_drain();
        multicore_fifo_clear_irq();
    }
    picoutil_restore_interrupts(interrupt_state);
}

// TODO: Implement an interrupt handler for core 0 that would be triggered when core 1 receives an interrupt from its FIFO
//       (i.e. after core 0 has sent something), and that would wait for core 1 to send a `PICOUTIL_INTERCORE_OK_MAGIC`
//       signal before returning to normal state
//       - It requires to handle potential "deadlocks"
//       - Everytime core 0 sends signal to core 1, put a tight loop that checks for a global static variable `core1_ok` to be true
//         (which would only be set to true after the previously mentioned interrupt handler receives a `PICOUTIL_INTERCORE_OK_MAGIC` signal,
//         and which would be reset after the loop). This way we ensure that core 1 is "ok" and has treated signal from core 0.
//       - The interrupt handler in core 0 should not wait for any signal from core 1 if it is triggered because of invalid push/pop in the FIFO
//         (i.e. make sure to differentiate between those cases)
//       - Make use of `__sev()`, `__wfe()` and `__wfi()` when possible, to avoid "tight loops"

__interrupt
// To be triggered when sending to core 1 and waiting for its "ok" response
void picoutil_isr_intercore_fifo_core01(void)
{
    uint32_t interrupt_state = picoutil_disable_interrupts();
    if (get_core_num() != 0)
    {
        picoutil_restore_interrupts(interrupt_state);
        exit(EXIT_FAILURE);
    }
    uint32_t fifo_st = multicore_fifo_get_status(); // The previously mentioned "differentiation" is done here
    if (fifo_st & SIO_FIFO_ST_WOF_BITS || fifo_st & SIO_FIFO_ST_ROE_BITS)
    {
        picoutil_log(LOG_ERROR, "A read occurred when the FIFO was empty, or a write occurred when the FIFO was full");
        multicore_fifo_drain();
        multicore_fifo_clear_irq();
        picoutil_restore_interrupts(interrupt_state);
        return;
    }
    uint32_t magic = 0;
retry:
    if (UNLIKELY(!multicore_fifo_pop_timeout_us(1000, &magic)))
    {
        picoutil_log(LOG_ERROR, "Timeout while waiting for core 1 to send a signal");
        multicore_fifo_drain();
        multicore_fifo_clear_irq();
        picoutil_restore_interrupts(interrupt_state);
        return;
    }
    if (magic != PICOUTIL_INTERCORE_OK_MAGIC)
    {
        picoutil_log(LOG_ERROR, "Invalid magic number in intercore FIFO: %x", magic);
        goto retry;
    }
    switch (magic)
    {
        case PICOUTIL_INTERCORE_LOCKOUT_MAGIC_START: // Don't care
            break;
        case PICOUTIL_INTERCORE_LOCKOUT_MAGIC_END: // Don't care
            break;
        case PICOUTIL_INTERCORE_TASK_RDY_MAGIC: // Don't care
            break;
        case PICOUTIL_INTERCORE_RES_RDY_MAGIC: // Don't care
#if 0
            // A result from core 1 is ready to be treated
            char buf[sizeof(core1_result_t)] = { 0 };
            core1_result_t result = { 0 };
            while (!queue_is_empty(&result_queue))
            {
                queue_remove_blocking(&result_queue, buf);
                result = *reinterpret_cast(core1_result_t*, DECAY(buf));
                // Since we empty the queue each time we get an interrupt, the queue is supposed to have only one result in it
                if (!queue_is_empty(&result_queue))
                    picoutil_log(LOG_FATAL, "The result queue is not supposed to have more than one result in it");
            }
            if (!result.ret_mem || !result.ret_size)
                picoutil_log(LOG_FATAL, "A result is composed of 2 \"units\". Only %d provided", (result.ret_mem ? 1 : 0) + (result.ret_size ? 1 : 0));
            // Add `result` to `core0_result_queue`
            queue_add_blocking(&core0_result_queue, &result);
            // We don't need to tell core 1 it's ok (we don't really care about this in core 1's context)
            // And in fact, we SHOULDN'T do it AT ALL, since it would trigger this interrupt handler again, and we don't want that
#endif
            // Commented out because it is actually the code for `picoutil_isr_intercore_fifo_core00()` (the ISR to be triggered when core 0 receives a result from core 1)
            break;
        case PICOUTIL_INTERCORE_OK_MAGIC:
            core1_ok = true;
            break;
        default:
            picoutil_log(LOG_ERROR, "Invalid magic number in intercore FIFO: %x", magic);
            break;
    }
    if (multicore_fifo_rvalid())
        multicore_fifo_drain();

    picoutil_restore_interrupts(interrupt_state);
}

__flatten
static inline void ensure_core1_ok(void)
{
    if (get_core_num() != 0)
        exit(EXIT_FAILURE);
    while (!picoutil_core1_ok())
        tight_loop_contents(); // ... At least for now
}

void picoutil_async_exec(int (*func)(void*), void* args, void* ret_buf, size_t ret_size)
{
    if (get_core_num() != 0)
        exit(EXIT_FAILURE);
    core1_task_units_t units[4] = { 0 };
    units[0].tag = CORE1_TASK_FUNC;
    units[0].payload.func = func;
    units[1].tag = CORE1_TASK_ARGS;
    units[1].payload.args = args;
    units[2].tag = CORE1_TASK_RET_MEM;
    units[2].payload.ret_mem = ret_buf;
    units[3].tag = CORE1_TASK_RET_SIZE;
    units[3].payload.ret_size = ret_size;
    for (size_t i = 0; i < 4; ++i)
        queue_add_blocking(&incoming_task_queue, &units[i]);
    if (UNLIKELY(!multicore_fifo_wready()))
        picoutil_log(LOG_FATAL, "Multicore FIFO is not ready to write when it should be");
    multicore_fifo_push_blocking(PICOUTIL_INTERCORE_TASK_RDY_MAGIC);
    __sev();
    ensure_core1_ok();
}