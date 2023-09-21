#include <picoutil.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <pico/bootrom.h>

#include <stdio.h>

#define MAX_COUNT 10000

static volatile uint64_t counter = 0;

static volatile typeof(counter) core0_counter = 0;
static volatile typeof(counter) core1_counter = 0;

static volatile bool core1_finished = false;

static void test_atomic_core0(void)
{
    printf("core0 started (corenum = %u)\n", get_core_num());
    volatile typeof(counter) curr_val = 0;
    core0_counter = 0;
    picoutil_atomic_op(curr_val = counter, memory_order_acquire);
    while (curr_val < MAX_COUNT)
    {
        ++core0_counter;
        picoutil_atomic_op(curr_val = ++counter, memory_order_acq_rel);
    }
}
static void test_atomic_core1(void)
{
    printf("core1 started (corenum = %u)\n", get_core_num());
    volatile typeof(counter) curr_val = 0;
    core1_counter = 0;
    core1_finished = false;
    picoutil_atomic_op(curr_val = counter, memory_order_acquire);
    while (curr_val < MAX_COUNT)
    {
        ++core1_counter;
        picoutil_atomic_op(curr_val = ++counter, memory_order_acq_rel);
    }
    core1_finished = true;
    __sev();
    while (1)
    {
        __wfe();
        __sev();
    }
}

static void test_atomic_unsafe_core0(void)
{
    printf("core0 started (corenum = %u)\n", get_core_num());
    volatile typeof(counter) curr_val = 0;
    core0_counter = 0;
    curr_val = counter;
    while (curr_val < MAX_COUNT)
    {
        ++core0_counter;
        curr_val = ++counter;
    }
}
static void test_atomic_unsafe_core1(void)
{
    printf("core1 started (corenum = %u)\n", get_core_num());
    volatile typeof(counter) curr_val = 0;
    core1_counter = 0;
    core1_finished = false;
    curr_val = counter;
    while (curr_val < MAX_COUNT)
    {
        ++core1_counter;
        curr_val = ++counter;
    }
    core1_finished = true;
    __sev();
    while (1)
    {
        __wfe();
        __sev();
    }
}

int main(void)
{
    stdio_init_all();
    volatile typeof(counter) counter_results[2] = {0};
    volatile typeof(counter) core0_counter_results[2] = {0};
    volatile typeof(counter) core1_counter_results[2] = {0};
    // Run here instead, and then print results in the loop
    multicore_launch_core1(test_atomic_core1);
    test_atomic_core0();
    while (!core1_finished)
    {
        __wfe();
    }
    multicore_reset_core1();
    counter_results[0] = counter;
    core0_counter_results[0] = core0_counter;
    core1_counter_results[0] = core1_counter;

    printf("counter_results[0] = %u\n", counter_results[0]);

    counter = 0;
    core0_counter = 0;
    core1_counter = 0;
    core1_finished = false;

    multicore_launch_core1(test_atomic_unsafe_core1);
    test_atomic_unsafe_core0();
    while (!core1_finished)
    {
        __wfe();
    }
    multicore_reset_core1();
    counter_results[1] = counter;
    core0_counter_results[1] = core0_counter;
    core1_counter_results[1] = core1_counter;

    printf("counter_results[1] = %u\n", counter_results[1]);

    counter = 0;
    core0_counter = 0;
    core1_counter = 0;
    core1_finished = false;

    while (1)
    {
        char c = getchar();
        if (c == 'q')
            reset_usb_boot(0, 0);

#if 0
        multicore_launch_core1(test_atomic_core1);
        test_atomic_core0();
        while (!core1_finished)
        {
            __wfe();
        }
        multicore_reset_core1();
        printf("[safe] counter = %u\n\n", counter);
        
        counter = 0;
        core0_counter = 0;
        core1_counter = 0;
        core1_finished = false;
        
        multicore_launch_core1(test_atomic_unsafe_core1);
        test_atomic_unsafe_core0();
        while (!core1_finished)
        {
            __wfe();
        }
        multicore_reset_core1();
        printf("[unsafe] counter = %u\n\n\n", counter);
        
        counter = 0;
        core0_counter = 0;
        core1_counter = 0;
        core1_finished = false;
#else
        printf("counter_results[0] = %u\n", counter_results[0]);
        printf("core0_counter_results[0] = %u\n", core0_counter_results[0]);
        printf("core1_counter_results[0] = %u\n", core1_counter_results[0]);
        printf("counter_results[1] = %u\n", counter_results[1]);
        printf("core0_counter_results[1] = %u\n", core0_counter_results[1]);
        printf("core1_counter_results[1] = %u\n", core1_counter_results[1]);
        printf("\n");
#endif
    }
    
}