#include <picoutil.h>

__restore_macro(__always_inline)

// Put includes here
#include <hardware/exception.h>

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <inttypes.h>

#include <picoutil_fix_macros.h>

static void nmi_except_handler(void)
{
    picoutil_log(LOG_FATAL, "NMI exception caught!");
}

static void hardfault_except_handler(void)
{
    picoutil_log(LOG_FATAL, "HardFault exception caught!");
}

static void svcall_except_handler(void)
{
    picoutil_log(LOG_FATAL, "SVCall exception caught!");
}

static void pend_sv_except_handler(void)
{
    picoutil_log(LOG_FATAL, "PendSV exception caught!");
}

static void systick_except_handler(void)
{
    picoutil_log(LOG_FATAL, "SysTick exception caught!");
}

void picoutil_install_exception_handlers(void)
{
    picoutil_log(LOG_DEBUG, "Installing exception handlers for core %d...", get_core_num());

    exception_set_exclusive_handler(NMI_EXCEPTION, nmi_except_handler);
    exception_set_exclusive_handler(HARDFAULT_EXCEPTION, hardfault_except_handler);
    exception_set_exclusive_handler(SVCALL_EXCEPTION, svcall_except_handler);
    exception_set_exclusive_handler(PENDSV_EXCEPTION, pend_sv_except_handler);
    exception_set_exclusive_handler(SYSTICK_EXCEPTION, systick_except_handler);

    picoutil_log(LOG_DEBUG, "Exception handlers installed for core %d", get_core_num());
}