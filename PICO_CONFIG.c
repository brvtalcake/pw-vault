// PICO_CONFIG: PARAM_ASSERTIONS_ENABLED_LOCK_CORE, Enable/disable assertions in the lock core, type=bool, default=0, group=pico_sync
// PICO_CONFIG: PICO_DEFAULT_LED_PIN, Optionally define a pin that drives a regular LED on the board, group=pico_stdlib
// PICO_CONFIG: PICO_DEFAULT_LED_PIN_INVERTED, 1 if LED is inverted or 0 if not, type=int, default=0, group=pico_stdlib
// PICO_CONFIG: PICO_DEFAULT_WS2812_PIN, Optionally define a pin that controls data to a WS2812 compatible LED on the board, group=pico_stdlib
// PICO_CONFIG: PICO_DEFAULT_WS2812_POWER_PIN, Optionally define a pin that controls power to a WS2812 compatible LED on the board, group=pico_stdlib
// PICO_CONFIG: PARAM_ASSERTIONS_ENABLED_TIME, Enable/disable assertions in the time module, type=bool, default=0, group=pico_time
// PICO_CONFIG: PICO_TIME_SLEEP_OVERHEAD_ADJUST_US, How many microseconds to wake up early (and then busy_wait) to account for timer overhead when sleeping in low power mode, type=int, default=6, group=pico_time
--
// PICO_CONFIG: PICO_TIME_DEFAULT_ALARM_POOL_DISABLED, Disable the default alarm pool, type=bool, default=0, advanced=true, group=pico_time
--
// PICO_CONFIG: PICO_TIME_DEFAULT_ALARM_POOL_HARDWARE_ALARM_NUM, Select which HW alarm is used for the default alarm pool, min=0, max=3, default=3, advanced=true, group=pico_time
// PICO_CONFIG: PICO_TIME_DEFAULT_ALARM_POOL_MAX_TIMERS, Selects the maximum number of concurrent timers in the default alarm pool, min=0, max=255, default=16, advanced=true, group=pico_time
// PICO_CONFIG: PARAM_ASSERTIONS_ENABLE_ALL, Global assert enable, type=bool, default=0, group=pico_base
// PICO_CONFIG: PARAM_ASSERTIONS_DISABLE_ALL, Global assert disable, type=bool, default=0, group=pico_base
// -----------------------------------------------------
// NOTE: THIS HEADER IS ALSO INCLUDED BY ASSEMBLY CODE SO
//       SHOULD ONLY CONSIST OF PREPROCESSOR DIRECTIVES
//       OR USE #ifndef __ASSEMBLER__ guards
// -------------
// PICO_CONFIG_HEADER_FILES and then PICO_SDK_<PLATFORM>_CONFIG_INCLUDE_FILES
// entries are dumped in order at build time into this generated header
// PICO_CONFIG: PICO_CONFIG_RTOS_ADAPTER_HEADER, unquoted path to header include in the default pico/config.h for RTOS integration defines that must be included in all sources, group=pico_base
// PICO_CONFIG: PICO_CONFIG_HEADER, unquoted path to header include in place of the default pico/config.h which may be desirable for build systems which can't easily generate the config_autogen header, group=pico_base
// PICO_CONFIG: PICO_QUEUE_MAX_LEVEL, Maintain a field for the highest level that has been reached by a queue, type=bool, default=0, advanced=true, group=queue
// PICO_CONFIG: PARAM_ASSERTIONS_ENABLED_PHEAP, Enable/disable assertions in the pheap module, type=bool, default=0, group=pico_util
--
// PICO_CONFIG: PICO_PHEAP_MAX_ENTRIES, Maximum number of entries in the pheap, min=1, max=65534, default=255, group=pico_util
// public heap_node ids are numbered from 1 (0 means none)
// PICO_CONFIG: PARAM_ASSERTIONS_ENABLED_UART, Enable/disable assertions in the UART module, type=bool, default=0, group=hardware_uart
// PICO_CONFIG: PICO_UART_ENABLE_CRLF_SUPPORT, Enable/disable CR/LF translation support, type=bool, default=1, group=hardware_uart
// PICO_CONFIG: PICO_UART_DEFAULT_CRLF, Enable/disable CR/LF translation on UART, type=bool, default=0, depends=PICO_UART_ENABLE_CRLF_SUPPORT, group=hardware_uart
// PICO_CONFIG: PICO_DEFAULT_UART, Define the default UART used for printf etc, min=0, max=1, group=hardware_uart
// PICO_CONFIG: PICO_DEFAULT_UART_TX_PIN, Define the default UART TX pin, min=0, max=29, group=hardware_uart
// PICO_CONFIG: PICO_DEFAULT_UART_RX_PIN, Define the default UART RX pin, min=0, max=29, group=hardware_uart
// PICO_CONFIG: PICO_DEFAULT_UART_BAUD_RATE, Define the default UART baudrate, max=921600, default=115200, group=hardware_uart
// These two config items are also used by assembler, so keeping separate
// PICO_CONFIG: PICO_MAX_SHARED_IRQ_HANDLERS, Maximum number of shared IRQ handlers, default=4, advanced=true, group=hardware_irq
// PICO_CONFIG: PICO_DISABLE_SHARED_IRQ_HANDLERS, Disable shared IRQ handlers, type=bool, default=0, group=hardware_irq
// PICO_CONFIG: PICO_VTABLE_PER_CORE, user is using separate vector tables per core, type=bool, default=0, group=hardware_irq
--
// PICO_CONFIG: PICO_DEFAULT_IRQ_PRIORITY, Define the default IRQ priority, default=0x80, group=hardware_irq
// PICO_CONFIG: PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY, Set default shared IRQ order priority, default=0x80, group=hardware_irq
// PICO_CONFIG: PARAM_ASSERTIONS_ENABLED_IRQ, Enable/disable assertions in the IRQ module, type=bool, default=0, group=hardware_irq
// PICO_CONFIG: PLL_COMMON_REFDIV, PLL reference divider setting - used for both PLLs, type=int, default=1, advanced=true, group=hardware_clocks
// PLL settings for standard 125 MHz system clock.
// PICO_CONFIG: PLL_SYS_VCO_FREQ_KHZ, System clock PLL frequency, type=int, default=1500 * KHZ, advanced=true, group=hardware_clocks
// PICO_CONFIG: PLL_SYS_POSTDIV1, System clock PLL post divider 1 setting, type=int, default=6, advanced=true, group=hardware_clocks
// PICO_CONFIG: PLL_SYS_POSTDIV2, System clock PLL post divider 2 setting, type=int, default=2, advanced=true, group=hardware_clocks
// PLL settings for a USB clock of 48MHz.
// PICO_CONFIG: PLL_USB_VCO_FREQ_KHZ, USB clock PLL frequency, type=int, default=1200 * KHZ, advanced=true, group=hardware_clocks
// PICO_CONFIG: PLL_USB_POSTDIV1, USB clock PLL post divider 1 setting, type=int, default=5, advanced=true, group=hardware_clocks
// PICO_CONFIG: PLL_USB_POSTDIV2, USB clock PLL post divider 2 setting, type=int, default=5, advanced=true, group=hardware_clocks
// PICO_CONFIG: PARAM_ASSERTIONS_ENABLED_CLOCKS, Enable/disable assertions in the clocks module, type=bool, default=0, group=hardware_clocks
// PICO_CONFIG: PARAM_ASSERTIONS_ENABLED_INTERP, Enable/disable assertions in the interpolation module, type=bool, default=0, group=hardware_interp
// PICO_CONFIG: PARAM_ASSERTIONS_ENABLED_SYNC, Enable/disable assertions in the HW sync module, type=bool, default=0, group=hardware_sync
// PICO_CONFIG: PICO_SPINLOCK_ID_IRQ, Spinlock ID for IRQ protection, min=0, max=31, default=9, group=hardware_sync
// PICO_CONFIG: PICO_SPINLOCK_ID_TIMER, Spinlock ID for Timer protection, min=0, max=31, default=10, group=hardware_sync
// PICO_CONFIG: PICO_SPINLOCK_ID_HARDWARE_CLAIM, Spinlock ID for Hardware claim protection, min=0, max=31, default=11, group=hardware_sync
// PICO_CONFIG: PICO_SPINLOCK_ID_RAND, Spinlock ID for Random Number Generator, min=0, max=31, default=12, group=hardware_sync
// PICO_CONFIG: PICO_SPINLOCK_ID_OS1, First Spinlock ID reserved for use by low level OS style software, min=0, max=31, default=14, group=hardware_sync
// PICO_CONFIG: PICO_SPINLOCK_ID_OS2, Second Spinlock ID reserved for use by low level OS style software, min=0, max=31, default=15, group=hardware_sync
// PICO_CONFIG: PICO_SPINLOCK_ID_STRIPED_FIRST, Lowest Spinlock ID in the 'striped' range, min=0, max=31, default=16, group=hardware_sync
// PICO_CONFIG: PICO_SPINLOCK_ID_STRIPED_LAST, Highest Spinlock ID in the 'striped' range, min=0, max=31, default=23, group=hardware_sync
// PICO_CONFIG: PICO_SPINLOCK_ID_CLAIM_FREE_FIRST, Lowest Spinlock ID in the 'claim free' range, min=0, max=31, default=24, group=hardware_sync
// PICO_CONFIG: PICO_SPINLOCK_ID_CLAIM_FREE_LAST, Highest Spinlock ID in the 'claim free' range, min=0, max=31, default=31, group=hardware_sync
// PICO_CONFIG: PARAM_ASSERTIONS_ENABLED_FLASH, Enable/disable assertions in the flash module, type=bool, default=0, group=hardware_flash
// PICO_CONFIG: PICO_FLASH_SIZE_BYTES, size of primary flash in bytes, type=int, group=hardware_flash
// PICO_CONFIG: PICO_STDIO_USB_DEFAULT_CRLF, Default state of CR/LF translation for USB output, type=bool, default=PICO_STDIO_DEFAULT_CRLF, group=pico_stdio_usb
// PICO_CONFIG: PICO_STDIO_USB_STDOUT_TIMEOUT_US, Number of microseconds to be blocked trying to write USB output before assuming the host has disappeared and discarding data, default=500000, group=pico_stdio_usb
// todo perhaps unnecessarily frequent?
// PICO_CONFIG: PICO_STDIO_USB_TASK_INTERVAL_US, Period of microseconds between calling tud_task in the background, default=1000, advanced=true, group=pico_stdio_usb
// PICO_CONFIG: PICO_STDIO_USB_LOW_PRIORITY_IRQ, Explicit User IRQ number to claim for tud_task() background execution instead of letting the implementation pick a free one dynamically (deprecated), advanced=true, group=pico_stdio_usb
// this variable is no longer set by default (one is claimed dynamically), but will be respected if specified
// PICO_CONFIG: PICO_STDIO_USB_ENABLE_RESET_VIA_BAUD_RATE, Enable/disable resetting into BOOTSEL mode if the host sets the baud rate to a magic value (PICO_STDIO_USB_RESET_MAGIC_BAUD_RATE), type=bool, default=1, group=pico_stdio_usb
// PICO_CONFIG: PICO_STDIO_USB_RESET_MAGIC_BAUD_RATE, baud rate that if selected causes a reset into BOOTSEL mode (if PICO_STDIO_USB_ENABLE_RESET_VIA_BAUD_RATE is set), default=1200, group=pico_stdio_usb
// PICO_CONFIG: PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS, Maximum number of milliseconds to wait during initialization for a CDC connection from the host (negative means indefinite) during initialization, default=0, group=pico_stdio_usb
// PICO_CONFIG: PICO_STDIO_USB_POST_CONNECT_WAIT_DELAY_MS, Number of extra milliseconds to wait when using PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS after a host CDC connection is detected (some host terminals seem to sometimes lose transmissions sent right after connection), default=50, group=pico_stdio_usb
// PICO_CONFIG: PICO_STDIO_USB_RESET_BOOTSEL_ACTIVITY_LED, Optionally define a pin to use as bootloader activity LED when BOOTSEL mode is entered via USB (either VIA_BAUD_RATE or VIA_VENDOR_INTERFACE), type=int, min=0, max=29, group=pico_stdio_usb
// PICO_CONFIG: PICO_STDIO_USB_RESET_BOOTSEL_FIXED_ACTIVITY_LED, Whether the pin specified by PICO_STDIO_USB_RESET_BOOTSEL_ACTIVITY_LED is fixed or can be modified by picotool over the VENDOR USB interface, type=bool, default=0, group=pico_stdio_usb
// Any modes disabled here can't be re-enabled by picotool via VENDOR_INTERFACE.
// PICO_CONFIG: PICO_STDIO_USB_RESET_BOOTSEL_INTERFACE_DISABLE_MASK, Optionally disable either the mass storage interface (bit 0) or the PICOBOOT interface (bit 1) when entering BOOTSEL mode via USB (either VIA_BAUD_RATE or VIA_VENDOR_INTERFACE), type=int, min=0, max=3, default=0, group=pico_stdio_usb
// PICO_CONFIG: PICO_STDIO_USB_ENABLE_RESET_VIA_VENDOR_INTERFACE, Enable/disable resetting into BOOTSEL mode via an additional VENDOR USB interface - enables picotool based reset, type=bool, default=1, group=pico_stdio_usb
// PICO_CONFIG: PICO_STDIO_USB_RESET_INTERFACE_SUPPORT_RESET_TO_BOOTSEL, If vendor reset interface is included allow rebooting to BOOTSEL mode, type=bool, default=1, group=pico_stdio_usb
// PICO_CONFIG: PICO_STDIO_USB_RESET_INTERFACE_SUPPORT_RESET_TO_FLASH_BOOT, If vendor reset interface is included allow rebooting with regular flash boot, type=bool, default=1, group=pico_stdio_usb
// PICO_CONFIG: PICO_STDIO_USB_RESET_RESET_TO_FLASH_DELAY_MS, delays in ms before rebooting via regular flash boot, default=100, group=pico_stdio_usb
// PICO_CONFIG: PICO_STDIO_USB_CONNECTION_WITHOUT_DTR, Disable use of DTR for connection checking meaning connection is assumed to be valid, type=bool, default=0, group=pico_stdio_usb
// PICO_CONFIG: PICO_STDIO_USB_DEVICE_SELF_POWERED, Set USB device as self powered device, type=bool, default=0, group=pico_stdio_usb
// PICO_CONFIG: PICO_STDIO_USB_SUPPORT_CHARS_AVAILABLE_CALLBACK, Enable USB STDIO support for stdio_set_chars_available_callback. Can be disabled to make use of USB CDC RX callback elsewhere, type=bool, default=1, group=pico_stdio_usb
// PICO_CONFIG: PICO_RAND_ENTROPY_SRC_ROSC, Enable/disable use of ROSC as an entropy source, type=bool, default=1, group=pico_rand
// PICO_CONFIG: PICO_RAND_ENTROPY_SRC_TIME, Enable/disable use of hardware timestamp as an entropy source, type=bool, default=1, group=pico_rand
// PICO_CONFIG: PICO_RAND_ENTROPY_SRC_BUS_PERF_COUNTER, Enable/disable use of a bus performance counter as an entropy source, type=bool, default=1, group=pico_rand
// --------------------
// SEED ENTROPY SOURCES
// --------------------
// PICO_CONFIG: PICO_RAND_SEED_ENTROPY_SRC_ROSC, Enable/disable use of ROSC as an entropy source for the random seed, type=bool, default=1, group=pico_rand
// PICO_CONFIG: PICO_RAND_SEED_ENTROPY_SRC_TIME, Enable/disable use of hardware timestamp as an entropy source for the random seed, type=bool, default=1, group=pico_rand
// PICO_CONFIG: PICO_RAND_SEED_ENTROPY_SRC_BOARD_ID, Enable/disable use of board id as part of the random seed, type=bool, default=1, group=pico_rand
// PICO_CONFIG: PICO_RAND_SEED_ENTROPY_SRC_RAM_HASH, Enable/disable use of a RAM hash as an entropy source for the random seed, type=bool, default=1, group=pico_rand
// ---------------------------------
// PICO_RAND_ENTROPY_SRC_ROSC CONFIG
// ---------------------------------
// PICO_CONFIG: PICO_RAND_ROSC_BIT_SAMPLE_COUNT, Number of samples to take of the ROSC random bit per random number generation , min=1, max=64, default=1, group=pico_rand
// PICO_CONFIG: PICO_RAND_MIN_ROSC_BIT_SAMPLE_TIME_US, Define a default minimum time between sampling the ROSC random bit, min=5, max=20, default=10, group=pico_rand
// (Arbitrary / tested) minimum time between sampling the ROSC random bit
// ---------------------------------------------
// PICO_RAND_ENTROPY_SRC_BUS_PERF_COUNTER CONFIG
// ---------------------------------------------
// PICO_CONFIG: PICO_RAND_BUS_PERF_COUNTER_INDEX, Bus performance counter index to use for sourcing entropy, min=0, max=3, group=pico_rand
// this is deliberately undefined by default, meaning the code will pick that appears unused
//#define PICO_RAND_BUS_PERF_COUNTER_INDEX 0
// PICO_CONFIG: PICO_RAND_BUS_PERF_COUNTER_EVENT, Bus performance counter event to use for sourcing entropy, default=arbiter_sram5_perf_event_access, group=pico_rand
// ------------------------------------------
// PICO_RAND_SEED_ENTROPY_SRC_RAM_HASH CONFIG
// ------------------------------------------
// PICO_CONFIG: PICO_RAND_RAM_HASH_END, end of address in RAM (non-inclusive) to hash during pico_rand seed initialization, default=SRAM_END, group=pico_rand
// PICO_CONFIG: PICO_RAND_RAM_HASH_START, start of address in RAM (inclusive) to hash during pico_rand seed initialization, default=PICO_RAND_RAM_HASH_END - 1024, group=pico_rand
// We provide a maximum of 128 bits entropy in one go
// PICO_CONFIG: PICO_STDOUT_MUTEX, Enable/disable mutex around stdout, type=bool, default=1, group=pico_stdio
// PICO_CONFIG: PICO_STDIO_ENABLE_CRLF_SUPPORT, Enable/disable CR/LF output conversion support, type=bool, default=1, group=pico_stdio
// PICO_CONFIG: PICO_STDIO_DEFAULT_CRLF, Default for CR/LF conversion enabled on all stdio outputs, type=bool, default=1, depends=PICO_STDIO_ENABLE_CRLF_SUPPORT, group=pico_stdio
// PICO_CONFIG: PICO_STDIO_STACK_BUFFER_SIZE, Define printf buffer size (on stack)... this is just a working buffer not a max output size, min=0, max=512, default=128, group=pico_stdio
// PICO_CONFIG: PICO_STDIO_DEADLOCK_TIMEOUT_MS, Time after which to assume stdio_usb is deadlocked by use in IRQ and give up, type=int, default=1000, group=pico_stdio
// PICO_CONFIG: PICO_FLASH_BANK_TOTAL_SIZE, Total size of the Bluetooth flash storage. Must be an even multiple of FLASH_SECTOR_SIZE, type=int, default=FLASH_SECTOR_SIZE * 2, group=pico_btstack
// PICO_CONFIG: PICO_FLASH_BANK_STORAGE_OFFSET, Offset in flash of the Bluetooth flash storage, type=int, default=PICO_FLASH_SIZE_BYTES - PICO_FLASH_BANK_TOTAL_SIZE, group=pico_btstack
// PICO_CONFIG: PARAM_ASSERTIONS_ENABLED_ADC, Enable/disable assertions in the ADC module, type=bool, default=0, group=hardware_adc
// PICO_CONFIG: PARAM_ASSERTIONS_ENABLED_DMA, Enable/disable DMA assertions, type=bool, default=0, group=hardware_dma
    // this method is used a lot by inline functions so avoid code bloat by deferring to function
// PICO_CONFIG: PARAM_ASSERTIONS_ENABLED_ADDRESS_ALIAS, Enable/disable assertions in memory address aliasing macros, type=bool, default=0, group=hardware_base
// PICO_CONFIG: PARAM_ASSERTIONS_ENABLED_GPIO, Enable/disable assertions in the GPIO module, type=bool, default=0, group=hardware_gpio
--
// PICO_CONFIG: GPIO_IRQ_CALLBACK_ORDER_PRIORITY, the irq priority order of the default IRQ callback, min=0, max=255, default=PICO_SHARED_IRQ_HANDLER_LOWEST_ORDER_PRIORITY, group=hardware_gpio
// PICO_CONFIG: GPIO_RAW_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY, the irq priority order of raw IRQ handlers if the priortiy is not specified, min=0, max=255, default=PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY, group=hardware_gpio
--
// PICO_CONFIG: PICO_DEBUG_PIN_BASE, First pin to use for debug output (if enabled), min=0, max=28, default=19, group=hardware_gpio
// PICO_CONFIG: PICO_DEBUG_PIN_COUNT, Number of pins to use for debug output (if enabled), min=1, max=28, default=3, group=hardware_gpio
// note these two macros may only be used once per and only apply per compilation unit (hence the CU_)
// PICO_CONFIG: PICO_STACK_SIZE, Stack Size, min=0x100, default=0x800, advanced=true, group=pico_platform
// PICO_CONFIG: PICO_HEAP_SIZE, Heap size to reserve, min=0x100, default=0x800, advanced=true, group=pico_platform
// PICO_CONFIG: PICO_NO_RAM_VECTOR_TABLE, Enable/disable the RAM vector table, type=bool, default=0, advanced=true, group=pico_platform
// PICO_CONFIG: PICO_RP2040_B0_SUPPORTED, Whether to include any specific software support for RP2040 B0 revision, type=bool, default=1, advanced=true, group=pico_platform
// PICO_CONFIG: PICO_FLOAT_SUPPORT_ROM_V1, Include float support code for RP2040 B0 when that chip revision is supported , type=bool, default=1, advanced=true, group=pico_platform
// PICO_CONFIG: PICO_DOUBLE_SUPPORT_ROM_V1, Include double support code for RP2040 B0 when that chip revision is supported , type=bool, default=1, advanced=true, group=pico_platform
// PICO_CONFIG: PICO_RP2040_B1_SUPPORTED, Whether to include any specific software support for RP2040 B1 revision, type=bool, default=1, advanced=true, group=pico_platform
// PICO_CONFIG: PICO_RP2040_B2_SUPPORTED, Whether to include any specific software support for RP2040 B2 revision, type=bool, default=1, advanced=true, group=pico_platform
// --- remainder of file is not included by assembly code ---
--
// PICO_CONFIG: PICO_NO_FPGA_CHECK, Remove the FPGA platform check for small code size reduction, type=bool, default=0, advanced=true, group=pico_runtime
// PICO_CONFIG: PARAM_ASSERTIONS_ENABLED_I2C, Enable/disable assertions in the I2C module, type=bool, default=0, group=hardware_i2c
--
// PICO_CONFIG: PICO_DEFAULT_I2C, Define the default I2C for a board, min=0, max=1, group=hardware_i2c
// PICO_CONFIG: PICO_DEFAULT_I2C_SDA_PIN, Define the default I2C SDA pin, min=0, max=29, group=hardware_i2c
// PICO_CONFIG: PICO_DEFAULT_I2C_SCL_PIN, Define the default I2C SCL pin, min=0, max=29, group=hardware_i2c
// PICO_CONFIG: PICO_PRINTF_ALWAYS_INCLUDED, Whether to always include printf code even if only called weakly (by panic), type=bool, default=1 in debug build 0 otherwise, group=pico_printf
// weak raw printf may be a puts if printf has not been called,
// PICO_CONFIG: CYW43_TASK_STACK_SIZE, Stack size for the CYW43 FreeRTOS task in 4-byte words, type=int, default=1024, group=pico_cyw43_arch
// PICO_CONFIG: CYW43_TASK_PRIORITY, Priority for the CYW43 FreeRTOS task, type=int, default=tskIDLE_PRIORITY + 4, group=pico_cyw43_arch
// PICO_CONFIG: PARAM_ASSERTIONS_ENABLED_CYW43_ARCH, Enable/disable assertions in the pico_cyw43_arch module, type=bool, default=0, group=pico_cyw43_arch
// PICO_CONFIG: PICO_CYW43_ARCH_DEBUG_ENABLED, Enable/disable some debugging output in the pico_cyw43_arch module, type=bool, default=1 in debug builds, group=pico_cyw43_arch
// PICO_CONFIG: PICO_CYW43_ARCH_DEFAULT_COUNTRY_CODE, Default country code for the cyw43 wireless driver, default=CYW43_COUNTRY_WORLDWIDE, group=pico_cyw43_arch
// PICO_CONFIG: PARAM_ASSERTIONS_ENABLED_TIMER, Enable/disable assertions in the timer module, type=bool, default=0, group=hardware_timer
// PICO_CONFIG: PICO_CMSIS_RENAME_EXCEPTIONS, Whether to rename SDK exceptions such as isr_nmi to their CMSIS equivalent i.e. NMI_Handler, type=bool, default=1, group=cmsis_core
// Note that since this header is included at the config stage, if you wish to override this you should do so via build compiler define
// PICO_CONFIG: PICO_BUILD_BOOT_STAGE2_NAME, The name of the boot stage 2 if selected by the build, group=boot_stage2
    // check that multiple boot stage 2 options haven't been set...
// PICO_CONFIG: PICO_BOOT_STAGE2_CHOOSE_IS25LP080, Select boot2_is25lp080 as the boot stage 2 when no boot stage 2 selection is made by the CMake build, type=bool, default=0, group=boot_stage2
// PICO_CONFIG: PICO_BOOT_STAGE2_CHOOSE_W25Q080, Select boot2_w25q080 as the boot stage 2 when no boot stage 2 selection is made by the CMake build, type=bool, default=0, group=boot_stage2
// PICO_CONFIG: PICO_BOOT_STAGE2_CHOOSE_W25X10CL, Select boot2_w25x10cl as the boot stage 2 when no boot stage 2 selection is made by the CMake build, type=bool, default=0, group=boot_stage2
// PICO_CONFIG: PICO_BOOT_STAGE2_CHOOSE_AT25SF128A, Select boot2_at25sf128a as the boot stage 2 when no boot stage 2 selection is made by the CMake build, type=bool, default=0, group=boot_stage2
// PICO_CONFIG: PICO_BOOT_STAGE2_CHOOSE_GENERIC_03H, Select boot2_generic_03h as the boot stage 2 when no boot stage 2 selection is made by the CMake build, type=bool, default=1, group=boot_stage2
// PICO_CONFIG: PARAM_ASSERTIONS_ENABLED_PWM, Enable/disable assertions in the PWM module, type=bool, default=0, group=hardware_pwm
// PICO_CONFIG: PARAM_ASSERTIONS_ENABLED_EXCEPTION, Enable/disable assertions in the exception module, type=bool, default=0, group=hardware_exception
// PICO_CONFIG: PICO_STDIO_UART_DEFAULT_CRLF, Default state of CR/LF translation for UART output, type=bool, default=PICO_STDIO_DEFAULT_CRLF, group=pico_stdio_uart
// PICO_CONFIG: PICO_STDIO_UART_SUPPORT_CHARS_AVAILABLE_CALLBACK, Enable UART STDIO support for stdio_set_chars_available_callback. Can be disabled to make use of the uart elsewhere, type=bool, default=1, group=pico_stdio_uart
// PICO_CONFIG: PICO_USE_MALLOC_MUTEX, Whether to protect malloc etc with a mutex, type=bool, default=1 with pico_multicore, 0 otherwise, group=pico_malloc
// PICO_CONFIG: PICO_MALLOC_PANIC, Enable/disable panic when an allocation failure occurs, type=bool, default=1, group=pico_malloc
// PICO_CONFIG: PICO_DEBUG_MALLOC, Enable/disable debug printf from malloc, type=bool, default=0, group=pico_malloc
// PICO_CONFIG: PICO_DEBUG_MALLOC_LOW_WATER, Define the lower bound for allocation addresses to be printed by PICO_DEBUG_MALLOC, min=0, default=0, group=pico_malloc
// PICO_CONFIG: PICO_CORE1_STACK_SIZE, Stack size for core 1, min=0x100, max=0x10000, default=PICO_STACK_SIZE (0x800), group=pico_multicore
// PICO_CONFIG: PARAM_ASSERTIONS_ENABLED_PIO_INSTRUCTIONS, Enable/disable assertions in the PIO instructions, type=bool, default=0, group=pio_instructions
// PICO_CONFIG: PARAM_ASSERTIONS_ENABLED_PIO, Enable/disable assertions in the PIO module, type=bool, default=0, group=hardware_pio
// PICO_CONFIG: PARAM_ASSERTIONS_ENABLED_SPI, Enable/disable assertions in the SPI module, type=bool, default=0, group=hardware_spi
--
// PICO_CONFIG: PICO_DEFAULT_SPI, Define the default SPI for a board, min=0, max=1, group=hardware_spi
// PICO_CONFIG: PICO_DEFAULT_SPI_SCK_PIN, Define the default SPI SCK pin, min=0, max=29, group=hardware_spi
// PICO_CONFIG: PICO_DEFAULT_SPI_TX_PIN, Define the default SPI TX pin, min=0, max=29, group=hardware_spi
// PICO_CONFIG: PICO_DEFAULT_SPI_RX_PIN, Define the default SPI RX pin, min=0, max=29, group=hardware_spi
// PICO_CONFIG: PICO_DEFAULT_SPI_CSN_PIN, Define the default SPI CSN pin, min=0, max=29, group=hardware_spi
// PICO_CONFIG: PICO_STDIO_SEMIHOSTING_DEFAULT_CRLF, Default state of CR/LF translation for semihosting output, type=bool, default=PICO_STDIO_DEFAULT_CRLF, group=pico_stdio_semihosting
// PICO_CONFIG: PICO_FLASH_ASSERT_ON_UNSAFE, Assert in debug mode rather than returning an error if flash_safe_execute cannot guarantee safety to catch bugs early, type=bool, default=1, group=pico_flash
// PICO_CONFIG: PICO_FLASH_ASSUME_CORE0_SAFE, Assume that core 0 will never be accessing flash and so doesn't need to be considered during flash_safe_execute, type=bool, default=0, group=pico_flash
// PICO_CONFIG: PICO_FLASH_ASSUME_CORE1_SAFE, Assume that core 1 will never be accessing flash and so doesn't need to be considered during flash_safe_execute, type=bool, default=0, group=pico_flash
// PICO_CONFIG: PICO_FLASH_SAFE_EXECUTE_SUPPORT_FREERTOS_SMP, Support using FreeRTOS SMP to make the other core safe during flash_safe_execute, type=bool, default=1 when using FreeRTOS SMP, group=pico_flash
// PICO_CONFIG: PICO_FLASH_SAFE_EXECUTE_PICO_SUPPORT_MULTICORE_LOCKOUT, Support using multicore_lockout functions to make the other core safe during flash_safe_execute, type=bool, default=1 when using pico_multicore, group=pico_flash
// PICO_CONFIG: PICO_XOSC_STARTUP_DELAY_MULTIPLIER, Multiplier to lengthen xosc startup delay to accommodate slow-starting oscillators, type=int, min=1, default=1, group=hardware_xosc
// PICO_CONFIG: USB_DPRAM_MAX, Set amount of USB RAM used by USB system, min=0, max=4096, default=4096, group=hardware_usb
// Allow user to claim some of the USB RAM for themselves
// Define maximum packet sizes
// PICO_CONFIG: XOSC_KHZ, The crystal oscillator frequency in kHz, type=int, default=12000, advanced=true, group=hardware_base
// NOTE:  The system and USB clocks are generated from the frequency using two PLLs.
// If you override this define, or SYS_CLK_KHZ/USB_CLK_KHZ below, you will *also* need to add your own adjusted PLL set-up defines to
// override the defaults which live in src/rp2_common/hardware_clocks/include/hardware/clocks.h
// Please see the comments there about calculating the new PLL setting values.
// PICO_CONFIG: SYS_CLK_KHZ, The system operating frequency in kHz, type=int, default=125000, advanced=true, group=hardware_base
// PICO_CONFIG: USB_CLK_KHZ, USB clock frequency. Must be 48MHz for the USB interface to operate correctly, type=int, default=48000, advanced=true, group=hardware_base
// For backwards compatibility define XOSC_MHZ if the frequency is indeed an integer number of Mhz.
// For backwards compatibility define SYS_CLK_MHZ if the frequency is indeed an integer number of Mhz.
