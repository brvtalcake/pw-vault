cmake_minimum_required(VERSION 3.21)

# If `PWVAULT_MAINTAINER_BUILD` environment variable is set to `1`, then set `IS_MAINTAINER_BUILD` to `ON`, else set it to `OFF`
if (DEFINED ENV{PWVAULT_MAINTAINER_BUILD})
    if ($ENV{PWVAULT_MAINTAINER_BUILD} STREQUAL "1")
        set(IS_MAINTAINER_BUILD ON)
    else()
        set(IS_MAINTAINER_BUILD OFF)
    endif()
else()
    set(IS_MAINTAINER_BUILD OFF)
endif()

if (${IS_MAINTAINER_BUILD})
    set(CMAKE_C_COMPILER "/toolchains/raspi/pico/bin/pico-gcc")
    set(CMAKE_CXX_COMPILER "/toolchains/raspi/pico/bin/pico-g++")
    set(CMAKE_ASM_COMPILER "/toolchains/raspi/pico/bin/pico-gcc")
    set(CMAKE_OBJCOPY "/toolchains/raspi/pico/bin/pico-objcopy")
    set(CMAKE_OBJDUMP "/toolchains/raspi/pico/bin/pico-objdump")
    set(CMAKE_SIZE "/toolchains/raspi/pico/bin/pico-size")
    set(CMAKE_NM "/toolchains/raspi/pico/bin/pico-gcc-nm")
    set(CMAKE_RANLIB "/toolchains/raspi/pico/bin/pico-gcc-ranlib")
    set(CMAKE_AR "/toolchains/raspi/pico/bin/pico-gcc-ar")
    set(CMAKE_LINKER "/toolchains/raspi/pico/bin/pico-ld")
    set(CMAKE_STRIP "/toolchains/raspi/pico/bin/pico-strip")
    set(PICO_TOOLCHAIN_PATH "/toolchains/raspi/pico")
endif()

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()


set(__pico_sdk_path__ $ENV{PICO_SDK_PATH})

if (
    DEFINED pico_hardware_modules OR
    DEFINED pico_high_level_modules OR
    DEFINED pico_tinyusb_modules OR
    DEFINED pico_networking_modules OR
    DEFINED pico_runtime_modules OR
    DEFINED pico_all_modules
)
    message(FATAL_ERROR "One or more of the following variables are already defined: pico_hardware_modules, pico_high_level_modules, pico_tinyusb_modules, pico_networking_modules, pico_runtime_modules, pico_all_modules")
endif()

set(
    pico_hardware_modules

    hardware_adc hardware_base hardware_claim hardware_clocks hardware_divider hardware_dma hardware_exception hardware_flash hardware_gpio hardware_i2c hardware_interp hardware_irq hardware_pio hardware_pll hardware_pwm hardware_resets hardware_rtc hardware_spi hardware_sync hardware_timer hardware_uart hardware_vreg hardware_watchdog hardware_xosc
)

set(
    pico_high_level_modules

    pico_flash pico_i2c_slave pico_multicore pico_rand pico_stdlib pico_sync pico_time pico_unique_id pico_util
    #[[ pico_async_context ]]
)

set(
    pico_tinyusb_modules

    #[[ tinyusb_device ]] #[[ tinyusb_host ]]
)

set(
    pico_networking_modules

    #[[ pico_btstack pico_btstack_ble pico_btstack_classic pico_btstack_cyw43 pico_cyw43_arch_lwip_threadsafe_background ]]
        #[[ pico_btstack_sbc_encoder pico_btstack_sbc_decoder pico_btstack_bnep_lwip pico_btstack_bnep_lwip_sys_freertos ]]
    #[[ pico_lwip pico_lwip_nosys pico_lwip_arch pico_cyw43_arch_lwip_threadsafe_background ]]
        #[[ pico_lwip_snmp pico_lwip_http pico_lwip_makefsdata pico_lwip_iperf pico_lwip_smtp pico_lwip_sntp pico_lwip_mdns pico_lwip_netbios pico_lwip_tftp pico_lwip_mbedtls pico_lwip_mqtt ]]
    pico_cyw43_driver pico_cyw43_arch
)

set(
    pico_runtime_modules

    pico_base pico_binary_info pico_bit_ops pico_bootrom pico_divider pico_double pico_float pico_int64_ops pico_malloc pico_mem_ops pico_platform pico_printf pico_runtime pico_stdio pico_standard_link
    #[[ boot_stage2 ]] #[[ pico_bootsel_via_double_reset ]] #[[ pico_cxx_options ]]
)

set(
    pico_all_modules

    ${pico_hardware_modules}

    ${pico_high_level_modules}

    #[[ ${pico_tinyusb_modules} ]]

    #[[ ${pico_networking_modules} ]]

    ${pico_runtime_modules}
)

if ((NOT DEFINED PICO_BOARD OR NOT PICO_BOARD) AND (NOT DEFINED ENV{PICO_BOARD}))
    set(PICO_BOARD "pico_w")
endif()

if ((NOT DEFINED PICO_GCC_TRIPLE) OR (NOT DEFINED ENV{PICO_GCC_TRIPLE}))
    set(PICO_GCC_TRIPLE "arm-none-eabi")
endif()

if ((NOT DEFINED PICO_PLATFORM) OR (NOT DEFINED ENV{PICO_PLATFORM}))
    set(PICO_PLATFORM "rp2040")
endif()

if ((NOT DEFINED PICO_COMPILER) OR (NOT DEFINED ENV{PICO_COMPILER}))
    set(PICO_COMPILER "pico_arm_gcc")
endif()

include(${__pico_sdk_path__}/cmake/Platform/PICO.cmake)
include("pico_sdk_import.cmake")
include(CheckIPOSupported)
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)
#[[ set_property(GLOBAL PROPERTY POSITION_INDEPENDENT_CODE OFF) ]]
set_property(GLOBAL PROPERTY C_STANDARD_REQUIRED ON)
set_property(GLOBAL PROPERTY CXX_STANDARD_REQUIRED ON)

project(pw_vault C CXX ASM)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_supported_output LANGUAGES C CXX)

if (ipo_supported)
    message(STATUS "IPO / LTO enabled for ${PROJECT_NAME}")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    add_compile_definitions(PICO_LTO=1)
else()
    message(FATAL_ERROR "IPO / LTO not supported: <${ipo_supported_output}>")
endif()

add_compile_options("-I/home/axel/Documents/programmation/mcu/raspi/pico/pw-vault/")
add_compile_options("-I/home/axel/Documents/programmation/mcu/raspi/pico/pw-vault/third_party/chaos-pp/")

#[[ add_compile_definitions(PICO_BOARD=${PICO_BOARD}) ]]
add_compile_definitions("PICO_GCC_TRIPLE=${PICO_GCC_TRIPLE}")
add_compile_definitions("PICO_PLATFORM=${PICO_PLATFORM}")
add_compile_definitions("PICO_USE_STACK_GUARDS=0")
add_compile_definitions("PICO_ENTER_USB_BOOT_ON_EXIT=1")

add_compile_definitions("_FORTIFY_SOURCE=2")
add_compile_definitions("_GNU_SOURCE=1")
add_compile_definitions("PICOUTIL_STATIC_BYTES_SIZE=(1024 * 2 * 2 * 2 * 2)")

add_compile_options(-static)

# Add `-mcpu='cortex-m0plus'` CFLAGS
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=cortex-m0plus -mfp16-format=ieee")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=cortex-m0plus -mfp16-format=ieee")

# Set build time (from `date` shell command if Linux, etc...), author and version variables
exec_program(date ARGS "+%Y-%m-%d at %H:%M:%S" OUTPUT_VARIABLE BUILD_DATE)
set(BUILD_AUTHOR "Axel PASCON")
set(BUILD_VERSION "0.1.0")
pico_sdk_init()

file(GLOB_RECURSE pico_side_SOURCES "src/*.cpp" "src/*.hpp" "src/*.c" "src/*.h")
add_subdirectory("./picoflash/")
add_subdirectory("./picoutil/")
add_subdirectory("./aes_test/")

if (TARGET tinyusb_device)
    add_executable(pwv_pico_side ${pico_side_SOURCES})
    add_dependencies(pwv_pico_side picoflash)

    target_include_directories(pwv_pico_side PUBLIC "./picoflash/" "./picoutil/" "./src/" "./third_party/chaos-pp/")
    target_link_libraries(
        pwv_pico_side
        ${pico_all_modules}
        picoflash picoutil aes_test
    )
    # Add -Wall -Wextra
    target_compile_options(pwv_pico_side PRIVATE "-Wall" "-Wextra" "-Werror" "-ftrack-macro-expansion=0")
    target_compile_options(pico_stdio INTERFACE "-Wno-error=strict-aliasing")

    pico_enable_stdio_usb(pwv_pico_side 1)
    pico_enable_stdio_uart(pwv_pico_side 0)

    pico_add_extra_outputs(pwv_pico_side)
elseif(PICO_ON_DEVICE)
    message(FATAL_ERROR "Not building storepw because TinyUSB submodule is not initialized in the SDK")
endif()

message(STATUS "Building for board: ${PICO_BOARD}, with build type: ${CMAKE_BUILD_TYPE}")