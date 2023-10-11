#include <picoutil.h>

__restore_macro(__always_inline)

// Put includes here
#include <hardware/exception.h>
#include <hardware/irq.h>
#include <pico/bootrom.h>
#include <pico/util/queue.h>
#include <pico/multicore.h>
#include <pico/sync.h>
#include <pico/stdlib.h>
#include <pico/time.h>

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <inttypes.h>
#include <stdatomic.h>

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
#ifndef PICOUTIL_INTERCORE_QUEUE_FULL_MAGIC
    // When a task queue overflows
    #define PICOUTIL_INTERCORE_QUEUE_FULL_MAGIC UINT32_C(0x0badf00d)
#endif

#ifndef PICOUTIL_MAX_CORE1_TASKS
    #define PICOUTIL_MAX_CORE1_TASKS 10U
#endif

#ifndef atomic_get
    #define atomic_get(VAR) ({ atomic_thread_fence(memory_order_acquire); (VAR); })
#endif

#ifdef ASSERT_IN_CORE
    #undef ASSERT_IN_CORE
#endif
#define ASSERT_IN_CORE(CORENUM)                                 \
    if (get_core_num() != (CORENUM))                            \
    {                                                           \
        picoutil_log(                                           \
            LOG_FATAL,                                          \
            "This function should only be called from core %d", \
            (CORENUM)                                           \
        );                                                      \
        exit(EXIT_FAILURE);                                     \
    }

#ifndef PICOUTIL_CORE1_FAILURE
    #define PICOUTIL_CORE1_FAILURE INT_MIN
#endif

struct lockout_state
{
    bool requested;
    bool granted;
};

__suppress_warning(discarded-qualifiers)

#ifdef auto_init_volatile_mutex
    #undef auto_init_volatile_mutex
#endif
#define auto_init_volatile_mutex(name) static __section(".mutex_array") volatile mutex_t name

static volatile struct lockout_state lockout_state = { 0 };

static volatile bool core0_in_isr = false;
auto_init_volatile_mutex(core0_state_mutex);

static volatile bool core1_in_isr = false;
auto_init_mutex(core1_state_mutex);

// For use by core 1 only.
// This is a `queue_t` of `struct core1_task`.
// It may be needed to enqueue tasks in core 1 if one arrives before the previous one is finished, hence the necessity of this queue.
// TODO: put it in `.scratch_y` ?
#if 0
static volatile queue_t core1_task_queue = { 0 } __scratch_y("core1_task_queue");
#else
static volatile queue_t core1_task_queue = { 0 };
#endif

// Core 1 will put results here, core 0 will read them.
// This is a `queue_t` of `struct core1_result`.
// As soon as core 1 finished sending a result in this queue, it has to seen a `PICOUTIL_INTERCORE_RES_RDY_MAGIC` signal in the intercore FIFO.
static volatile queue_t result_queue = { 0 };

// Core 0 will put tasks here, core 1 will read them.
// This is a `queue_t` of `struct core1_task_units`.
// As soon as core 0 finished sending a task in this queue, it has to seen a `PICOUTIL_INTERCORE_TASK_RDY_MAGIC` signal in the intercore FIFO.
// This queue is only a sort of communication buffer from core 0 to core 1.
static volatile queue_t incoming_task_queue = { 0 };

// Final results from core 1 will be put here, core 0 will read them.
// TODO: put it in `.scratch_x` ?
#if 0
static volatile queue_t core0_result_queue = { 0 } __scratch_x("core0_result_queue");
#else
static volatile queue_t core0_result_queue = { 0 };
#endif

static volatile struct core1_task running_task = { 0 };

static volatile bool core1_launched = false;
static volatile bool core1_ok = false;

static volatile bool core0_initialized = false;

static volatile jmp_buf core1_entry_jmp_buf = { 0 };

__interrupt
static void picoutil_isr_intercore_fifo_core1(void);
__interrupt
static void picoutil_isr_intercore_fifo_core0(void);

// To be used "internally" by the library
static bool picoutil_core1_ok(void)
{
    bool old = core1_ok;
    if (old)
        core1_ok = false;
    return old;
}

__noreturn
// To be called within task functions when wanting to return from the task normally
void picoutil_return_to_core1(void* real_ret, int exit_code)
{
    ASSERT_IN_CORE(1);
    if (running_task.ret_size > 0 && running_task.ret_mem != NULL && real_ret != NULL)
        memcpy(running_task.ret_mem, real_ret, running_task.ret_size);
    running_task.exit_code = exit_code;
    longjmp(core1_entry_jmp_buf, exit_code);
}

__noreturn
// To be called within task functions when wanting to abort the task
void picoutil_fail_core1(void)
{
    ASSERT_IN_CORE(1);
    picoutil_free_all_in_core(1);
    running_task.exit_code = PICOUTIL_CORE1_FAILURE;
    longjmp(core1_entry_jmp_buf, PICOUTIL_CORE1_FAILURE);
}

__always_inline
// Get result from core 1 ---> core 0 queue and put it in core 0 result queue
static inline void process_result(void)
{
    ASSERT_IN_CORE(0);
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
    // Add `result` to `core0_result_queue`
    queue_add_blocking(&core0_result_queue, &result);
}

static inline void install_fifo_isr_core1(void)
{
    ASSERT_IN_CORE(1);

    irq_set_exclusive_handler(SIO_IRQ_PROC1, picoutil_isr_intercore_fifo_core1);
    irq_set_enabled(SIO_IRQ_PROC1, true);
    irq_set_priority(SIO_IRQ_PROC1, 0);
}

static inline void install_fifo_isr_core0(void)
{
    ASSERT_IN_CORE(0);

    irq_set_exclusive_handler(SIO_IRQ_PROC0, picoutil_isr_intercore_fifo_core0);
    irq_set_enabled(SIO_IRQ_PROC0, true);
    irq_set_priority(SIO_IRQ_PROC0, 0);
}

static inline void prepare_core0(void)
{
    ASSERT_IN_CORE(0);

    if (UNLIKELY(core0_initialized))
        return;

    multicore_fifo_clear_irq();
    multicore_fifo_drain();
    install_fifo_isr_core0();

    // Queue for core 0 only (could be in `.scratch_x`)
    queue_init(&core0_result_queue, sizeof(core1_result_t), (PICOUTIL_MAX_CORE1_TASKS) * 2);

    // Queue for both cores
    queue_init(&result_queue, sizeof(core1_result_t), 2); // One should be enough, but just in case

    // Queue for both cores
    // Even though we only need 4 units, allocate a bit more just in case, so core 0 is less likely to block or trigger errors because of a full queue
    queue_init(&incoming_task_queue, sizeof(core1_task_units_t), 4 * 2);

    core0_initialized = true;
}

static inline void prepare_core1(void)
{
    ASSERT_IN_CORE(1);

    multicore_fifo_clear_irq();
    multicore_fifo_drain();
    install_fifo_isr_core1();
    
    // Queue for core 1 only
    queue_init(&core1_task_queue, sizeof(core1_task_t), PICOUTIL_MAX_CORE1_TASKS);
}

__noreturn
static void picoutil_core1_entry(void)
{
    ASSERT_IN_CORE(1);

    prepare_core1();
    picoutil_install_exception_handlers();
    core1_launched = true;
    atomic_thread_fence(memory_order_release);
    __sev();

    while (true)
    {
        while (queue_is_empty(&core1_task_queue))
            __wfi();

        char buf[sizeof(core1_task_t)] = { 0 };
        queue_remove_blocking(&core1_task_queue, buf);
        running_task = *reinterpret_cast(core1_task_t*, DECAY(buf));
        if (UNLIKELY(!running_task.func))
            picoutil_log(LOG_FATAL, "Invalid function pointer");
        // The rest of the task doesn't need to be checked (it is possible to privide no args and to return nothing)

        switch (setjmp(core1_entry_jmp_buf))
        {
            case PICOUTIL_CORE1_FAILURE:
                picoutil_log(LOG_FATAL, "Core 1 failed");
                break;
            case 0:
                running_task.func(running_task.args);
                break;
            default:
                // Everything is ok
                // Push a result in the result queue or wait if core 0 is not ready

                core1_result_t result = { 0 };
                result.ret_mem = running_task.ret_mem;
                result.ret_size = running_task.ret_size;
                result.exit_code = running_task.exit_code;
                queue_add_blocking(&result_queue, &result);

                uint32_t out;
                while (true)
                {
                    while (!mutex_try_enter(&core0_state_mutex, &out))
                    {
                        if (out == 1)
                            picoutil_log(LOG_FATAL, "Core 1 is already owning a mutex it should not own at this point");
                        tight_loop_contents();
                    }
                    if (!core0_in_isr)
                        break;
                    mutex_exit(&core0_state_mutex);
                }

                if (UNLIKELY(!multicore_fifo_wready()))
                    picoutil_log(LOG_FATAL, "Multicore FIFO is not ready to write");
                multicore_fifo_push_blocking(PICOUTIL_INTERCORE_RES_RDY_MAGIC);
                mutex_exit(&core0_state_mutex);
                __sev();
                break;
        }        
    }
    __unreachable();
}

void picoutil_launch_core1(void)
{
    ASSERT_IN_CORE(0);

    if (!core1_launched)
    {
        multicore_launch_core1(&picoutil_core1_entry);

        while (!atomic_get(core1_launched))
            __wfe();

        if (!core1_launched)
            picoutil_log(LOG_FATAL, "Core 1 could not be launched");
    }

    core1_launched = true;
}

__interrupt
static void picoutil_isr_intercore_fifo_core1(void)
{
    ASSERT_IN_CORE(1);

    if (UNLIKELY(!mutex_is_initialized(&core1_state_mutex)))
        mutex_init(&core1_state_mutex);
    mutex_enter_blocking(&core1_state_mutex);
    core1_in_isr = true;
    mutex_exit(&core1_state_mutex);

    uint32_t fifo_st = multicore_fifo_get_status();
    if (UNLIKELY(fifo_st & SIO_FIFO_ST_WOF_BITS || fifo_st & SIO_FIFO_ST_ROE_BITS))
    {
        picoutil_log(LOG_ERROR, "A read occurred when the FIFO was empty, or a write occurred when the FIFO was full");
        multicore_fifo_clear_irq();
    }

    uint32_t interrupt_state = picoutil_disable_interrupts();
    while (multicore_fifo_rvalid())
    {
        uint32_t magic = multicore_fifo_pop_blocking();
        switch (magic)
        {
            case PICOUTIL_INTERCORE_LOCKOUT_MAGIC_START:
                multicore_fifo_push_blocking(PICOUTIL_INTERCORE_LOCKOUT_MAGIC_START); // Not "PICOUTIL_INTERCORE_OK_MAGIC" so we can distinguish between the two
                lockout_state.granted = true;
                atomic_thread_fence(memory_order_release);
                while (multicore_fifo_pop_blocking() != PICOUTIL_INTERCORE_LOCKOUT_MAGIC_END)
                    __wfe();
                multicore_fifo_push_blocking(PICOUTIL_INTERCORE_LOCKOUT_MAGIC_END);
                lockout_state.granted = false;
                atomic_thread_fence(memory_order_release);
                __sev();
                break;
            case PICOUTIL_INTERCORE_LOCKOUT_MAGIC_END:
                // Should not happen
                __sev();
                break;
            case PICOUTIL_INTERCORE_TASK_RDY_MAGIC:
                // An incoming task from core 0 is ready to be treated
                char buf[sizeof(core1_task_units_t)] = { 0 };
                core1_task_t task = { 0 };
                uint8_t count = 0;
                while (!queue_is_empty(&incoming_task_queue))
                {
                    bzero(buf, sizeof(buf));
                    queue_remove_blocking(&incoming_task_queue, buf);
                    core1_task_units_t unit = *reinterpret_cast(core1_task_units_t*, DECAY(buf));
                    switch (unit.tag)
                    {
                        case CORE1_TASK_FUNC:
                            task.func = unit.payload.func;
                            ++count;
                            break;
                        case CORE1_TASK_ARGS:
                            task.args = unit.payload.args;
                            ++count;
                            break;
                        case CORE1_TASK_RET_MEM:
                            task.ret_mem = unit.payload.ret_mem;
                            ++count;
                            break;
                        case CORE1_TASK_RET_SIZE:
                            task.ret_size = unit.payload.ret_size;
                            ++count;
                            break;
                        default:
                            picoutil_log(LOG_ERROR, "Invalid tag in core1_task_units_t: %d", unit.tag);
                            break;
                    }
                    // Since we empty the queue each time we get an interrupt, the queue is supposed to have only one task in it, so only 4 units
                    // Hence, if we have all 4 units but still have elems in the queue, that's an error
                    if (count == 4 && !queue_is_empty(&incoming_task_queue))
                    {
                        picoutil_restore_interrupts(interrupt_state);
                        picoutil_log(LOG_FATAL, "The incoming task queue is not supposed to have more than one task in it, or have more \"units\" than the 4 required ones");                    
                    }
                }
                if (count != 4)
                {
                    picoutil_restore_interrupts(interrupt_state);
                    picoutil_log(LOG_FATAL, "A task is composed of 4 \"units\". Only %d provided", (task.func ? 1 : 0) + (task.args ? 1 : 0) + (task.ret_mem ? 1 : 0) + (task.ret_size ? 1 : 0));
                }
                // Add `task` to `core1_task_queue`
                queue_add_blocking(&core1_task_queue, &task);
                uint32_t out;
                while (true)
                {
                    while (!mutex_try_enter(&core0_state_mutex, &out))
                    {
                        if (out == 1)
                            picoutil_log(LOG_FATAL, "Core 1 is already owning a mutex it should not own at this point");
                        tight_loop_contents();
                    }
                    if (!core0_in_isr)
                        break;
                    mutex_exit(&core0_state_mutex);
                }
                multicore_fifo_push_blocking(PICOUTIL_INTERCORE_OK_MAGIC); // We received everything 5/5
                mutex_exit(&core0_state_mutex);
                __sev();
                break;
            case PICOUTIL_INTERCORE_RES_RDY_MAGIC:
                __sev();
                break; // Should not happen
            case PICOUTIL_INTERCORE_OK_MAGIC:
                __sev();
                break; // We don't care
            default:
                picoutil_log(LOG_ERROR, "Invalid magic number in intercore FIFO: %" PRIx32 "\n", magic);
                __sev();
                break;
        }
    }
    
    picoutil_restore_interrupts(interrupt_state);
    mutex_enter_blocking(&core1_state_mutex);
    core1_in_isr = false;
    mutex_exit(&core1_state_mutex);
    __sev();
}

__interrupt
static void picoutil_isr_intercore_fifo_core0(void)
{
    ASSERT_IN_CORE(0);

    if (UNLIKELY(!mutex_is_initialized(&core0_state_mutex)))
        mutex_init(&core0_state_mutex);
    mutex_enter_blocking(&core0_state_mutex);
    core0_in_isr = true;
    mutex_exit(&core0_state_mutex);

    uint32_t fifo_st = multicore_fifo_get_status();
    if (UNLIKELY(fifo_st & SIO_FIFO_ST_WOF_BITS || fifo_st & SIO_FIFO_ST_ROE_BITS))
    {
        picoutil_log(LOG_ERROR, "A read occurred when the FIFO was empty, or a write occurred when the FIFO was full");
        multicore_fifo_clear_irq();
    }

    uint32_t interrupt_state = picoutil_disable_interrupts();
    while (multicore_fifo_rvalid())
    {
        uint32_t magic = multicore_fifo_pop_blocking();
        switch (magic)
        {
            case PICOUTIL_INTERCORE_LOCKOUT_MAGIC_START:
                // Should not happen because we specificaly disable FIFO interrupts when we send a lockout request
                // and because core 1 cannot ask core 0 to lockout
                __sev();
                break;
            case PICOUTIL_INTERCORE_LOCKOUT_MAGIC_END:
                // Should not happen (same as above)
                __sev();
                break;
            case PICOUTIL_INTERCORE_TASK_RDY_MAGIC:
                // Should not happen + we don't care
                __sev();
                break;
            case PICOUTIL_INTERCORE_RES_RDY_MAGIC:
                // A result from core 1 is ready to be treated
                process_result();
                __sev();
                break;
            case PICOUTIL_INTERCORE_OK_MAGIC:
                core1_ok = true;
                __sev();
                break;
            default:
                picoutil_log(LOG_ERROR, "Invalid magic number in intercore FIFO: %" PRIx32 "\n", magic);
                __sev();
                break;
        }
    }

    picoutil_restore_interrupts(interrupt_state);
    mutex_enter_blocking(&core0_state_mutex);
    core0_in_isr = false;
    mutex_exit(&core0_state_mutex);
    __sev();
}

__flatten
static inline void ensure_core1_ok(void)
{
    ASSERT_IN_CORE(0);
    while (!picoutil_core1_ok())
        tight_loop_contents(); // ... At least for now
}

void picoutil_async_exec(picoutil_task_func_t func, void* args, void* ret_buf, size_t ret_size)
{
    ASSERT_IN_CORE(0);
    
    if (UNLIKELY(!func))
    {
        picoutil_log(LOG_ERROR, "Invalid function pointer");
        return;
    }

    if (!core0_initialized)
        prepare_core0();
    if (!core1_launched)
        picoutil_launch_core1();
    
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
    
    uint32_t out;
    while (true)
    {
        while (!mutex_try_enter(&core1_state_mutex, &out))
        {
            if (out == 0)
                picoutil_log(LOG_FATAL, "Core 0 is already owning a mutex it should not own at this point");
            tight_loop_contents();
        }
        if (!core1_in_isr)
            break;
        mutex_exit(&core1_state_mutex);
    }

    if (UNLIKELY(!multicore_fifo_wready()))
        picoutil_log(LOG_FATAL, "Multicore FIFO is not ready to write when it should be");
    multicore_fifo_push_blocking(PICOUTIL_INTERCORE_TASK_RDY_MAGIC);
    mutex_exit(&core1_state_mutex);
    
    __sev();
    ensure_core1_ok();
}

bool picoutil_has_result(void)
{
    ASSERT_IN_CORE(0);
    return !queue_is_empty(&core0_result_queue);
}

void picoutil_wait_result(void* ret_buf, size_t ret_size)
{
    ASSERT_IN_CORE(0);
    picoutil_log(LOG_DEBUG, "Waiting for result");
    while (queue_is_empty(&core0_result_queue))
        __wfi();
    char buf[sizeof(core1_result_t)] = { 0 };
    queue_remove_blocking(&core0_result_queue, buf);
    core1_result_t result = *reinterpret_cast(core1_result_t*, DECAY(buf));
    if (result.ret_size > 0 && result.ret_mem != NULL && ret_buf != NULL)
        memcpy(ret_buf, result.ret_mem, result.ret_size);
    if (ret_size != result.ret_size)
        picoutil_log(LOG_WARNING, "The size of the result buffer (%d) is different from the size of the result (%d)", ret_size, result.ret_size);
}

bool picoutil_lockout_start(void)
{
    ASSERT_IN_CORE(0);

    uint irq_num = SIO_IRQ_PROC0;
    bool enabled = irq_is_enabled(irq_num);
    if (enabled) irq_set_enabled(irq_num, false);

    mutex_enter_blocking(&core0_state_mutex);
    core0_in_isr = true; // We're not in isr, but we want to prevent core 1 from sending a result while we're in the lockout
    mutex_exit(&core0_state_mutex);

    multicore_fifo_push_blocking(PICOUTIL_INTERCORE_LOCKOUT_MAGIC_START);

    lockout_state.requested = true;
    atomic_thread_fence(memory_order_release);
    while (!atomic_get(lockout_state.granted))
        __wfe();

    uint32_t magic;
    // Verify that core 1 has effectively answered
    while (multicore_fifo_rvalid())
    {
        magic = multicore_fifo_pop_blocking();
        if (magic == PICOUTIL_INTERCORE_RES_RDY_MAGIC)
            process_result();
        else if (magic == PICOUTIL_INTERCORE_LOCKOUT_MAGIC_START) // Core 1 answered and started its lockout
            break;
    }
    if (magic != PICOUTIL_INTERCORE_LOCKOUT_MAGIC_START)
        picoutil_log(LOG_FATAL, "Core 1 did not answer to lockout request");

    return enabled;
}

void picoutil_lockout_end(const volatile bool state)
{
    ASSERT_IN_CORE(0);

    multicore_fifo_push_blocking(PICOUTIL_INTERCORE_LOCKOUT_MAGIC_END);

    lockout_state.requested = false;
    atomic_thread_fence(memory_order_release);
    while (atomic_get(lockout_state.granted))
        __wfe();

    // Verify that core 1 has effectively answered
    // Double check
    uint32_t magic = multicore_fifo_pop_blocking(); // We can block, because it is supposed to answer nearly immediately
    if (magic != PICOUTIL_INTERCORE_LOCKOUT_MAGIC_END)
        picoutil_log(LOG_FATAL, "Core 1 did not answer to lockout end");


    if (state)
        irq_set_enabled(SIO_IRQ_PROC0, state);

    mutex_enter_blocking(&core0_state_mutex);
    core0_in_isr = false;
    mutex_exit(&core0_state_mutex);
}

__restore_warnings