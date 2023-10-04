#include <picoutil.h>

__restore_macro(__always_inline)

#include <pico/bootrom.h>
#include <pico/stdlib.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <picoutil_fix_macros.h>

#ifdef BLUE
    #undef BLUE
#endif
#ifdef GREEN
    #undef GREEN
#endif
#ifdef RED
    #undef RED
#endif
#ifdef YELLOW
    #undef YELLOW
#endif
#ifdef WHITE
    #undef WHITE
#endif
#ifdef PURPLE
    #undef PURPLE
#endif
#ifdef BOLD
    #undef BOLD
#endif
#ifdef RESET
    #undef RESET
#endif

// All colors are bright
#define BLUE "\033[94m"
#define GREEN "\033[92m"
#define RED "\033[91m"
#define YELLOW "\033[93m"
#define WHITE "\033[97m"
#define PURPLE "\033[95m"
#define BOLD "\033[1m"
#define RESET "\033[0m"

static uint8_t log_threshold = LOG_WARNING;

uint8_t picoutil_set_log_threshold(log_level threshold)
{
    uint8_t old_threshold = log_threshold;
    log_threshold = threshold;
    return old_threshold;
}

static void log_va_list(const char* format, va_list args)
{
    vprintf(format, args);
    puts("");
}

__printflike(2, 3)
void picoutil_log(log_level level, const char* format, ...)
{
    if ((uint8_t)level < log_threshold)
        return;
    puts("");
    va_list args;
    va_start(args, format);
    switch (level)
    {
        case LOG_SUCCESS:
            printf(GREEN "[ SUCCESS ]  ");
            break;
        case LOG_INFO:
            printf(BLUE "[ INFO ]  ");
            break;
        case LOG_DEBUG:
            printf(WHITE "[ DEBUG ]  ");
            break;
        case LOG_WARNING:
            printf(YELLOW "[WARNING]  ");
            break;
        case LOG_ERROR:
            printf(RED "[ ERROR ]  ");
            break;
        case LOG_FATAL:
            printf(RED BOLD "[ FATAL ]  ");
            break;
        default:
            printf(PURPLE BOLD "[ UNKNOWN ]  ");
            break;
    }
    printf(RESET);
    log_va_list(format, args);
    va_end(args);
    if (level == LOG_FATAL)
#if defined(PICO_ENTER_USB_BOOT_ON_EXIT) && PICO_ENTER_USB_BOOT_ON_EXIT
        exit(EXIT_FAILURE);
#else
        reset_usb_boot(0, 0);
#endif
}

__printflike(1, 2)
void picoutil_log_raw(const char* format, ...)
{
    puts("");
    va_list args;
    va_start(args, format);
    log_va_list(format, args);
    va_end(args);
}

// TODO: Write `picoutil_panic` like so:
//     extern void __attribute__((noreturn)) __printflike(1, 0) PICO_PANIC_FUNCTION(__unused const char *fmt, ...);
// And define `PICO_PANIC_FUNCTION` to be `picoutil_panic` 