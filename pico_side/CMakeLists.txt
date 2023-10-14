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

if (NOT(${CMAKE_GENERATOR} STREQUAL "Ninja Multi-Config"))
    if (NOT DEFINED CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release")
    endif()
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

add_compile_definitions("PARAM_ASSERTIONS_ENABLE_ALL=1")
add_compile_definitions("PICO_USE_ORIGINAL_EXIT=1")
add_compile_options("-masm-syntax-unified")

# Ask for four configs: Debug, Release, RelWithDebInfo and MinSizeRel
if (${CMAKE_GENERATOR} STREQUAL "Ninja Multi-Config")
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel" CACHE STRING "" FORCE)
endif()

check_ipo_supported(RESULT ipo_supported OUTPUT ipo_supported_output LANGUAGES C CXX)

if (ipo_supported)
    if (NOT(${CMAKE_GENERATOR} STREQUAL "Ninja Multi-Config") AND ${CMAKE_BUILD_TYPE} STREQUAL "Release")
        message(STATUS "IPO / LTO enabled for ${PROJECT_NAME}")
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
        add_compile_definitions(PICO_LTO=1)
    elseif (${CMAKE_GENERATOR} STREQUAL "Ninja Multi-Config")
        # Enable LTO for Release builds only (not the three other configs)
        # Also, add the `PICO_LTO` macro definition only when LTO is enabled (i.e. only for Release builds)
        message(STATUS "IPO / LTO enabled for ${PROJECT_NAME}")
        set_property(GLOBAL PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
        set_property(GLOBAL PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO FALSE)
        set_property(GLOBAL PROPERTY INTERPROCEDURAL_OPTIMIZATION_DEBUG FALSE)
        set_property(GLOBAL PROPERTY INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL FALSE)
        add_compile_definitions("$<IF:$<CONFIG:Release>,PICO_LTO=1,PICO_LTO=0>")
    else()
        message(STATUS "IPO / LTO disabled for ${PROJECT_NAME}")
        add_compile_definitions(PICO_LTO=0)
    endif()
else()
    message(FATAL_ERROR "IPO / LTO not supported: <${ipo_supported_output}>")
endif()

add_compile_options("-I/home/axel/Documents/programmation/mcu/raspi/pico/pw-vault/")
add_compile_options("-I/home/axel/Documents/programmation/mcu/raspi/pico/pw-vault/third_party/chaos-pp/")

function(add_gcc_param_if_release param)
    add_compile_options($<IF:$<CONFIG:Release>,${param},$<IF:$<CONFIG:MinSizeRel>,${param},>>)
endfunction()

function(add_gcc_param_if_debug param)
    add_compile_options($<IF:$<CONFIG:Debug>,${param},$<IF:$<CONFIG:RelWithDebInfo>,${param},>>)
endfunction()

#add_compile_options($<IF:$<CONFIG:Release>,-fgcse-las -fgcse-sm -fgraphite -fgraphite-identity -fipa-pta -flive-range-shrinkage -fmodulo-sched,>)
add_gcc_param_if_release("-fgcse-las")
add_gcc_param_if_release("-fgcse-sm")
add_gcc_param_if_release("-fgraphite")
add_gcc_param_if_release("-fgraphite-identity")
add_gcc_param_if_release("-fipa-pta")
add_gcc_param_if_release("-flive-range-shrinkage")
add_gcc_param_if_release("-fmodulo-sched")
add_gcc_param_if_release("-fmodulo-sched-allow-regmoves")
add_gcc_param_if_release("-free")
add_gcc_param_if_release("-frename-registers")
add_gcc_param_if_release("-fschedule-fusion")
add_gcc_param_if_release("-ftracer")
add_gcc_param_if_release("-fschedule-insns")
add_gcc_param_if_release("-fschedule-insns2")
add_gcc_param_if_release("-fsplit-wide-types-early")
add_gcc_param_if_release("-ftree-cselim")
add_gcc_param_if_release("-ftree-vectorize")
add_gcc_param_if_release("-fweb")

add_gcc_param_if_debug("-ggdb3")

add_compile_options("-fno-exceptions")

#[[ add_compile_definitions(PICO_BOARD=${PICO_BOARD}) ]]
add_compile_definitions("PICO_GCC_TRIPLE=${PICO_GCC_TRIPLE}")
add_compile_definitions("PICO_PLATFORM=${PICO_PLATFORM}")
add_compile_definitions("PICO_USE_STACK_GUARDS=1")
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

    target_include_directories(pwv_pico_side PUBLIC "./picoflash/" "./picoutil/" "./src/" "./third_party/chaos-pp/" "./")
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

get_target_property(picoutil_compile_options picoutil COMPILE_OPTIONS)

set(
    compile_command
    
    /toolchains/raspi/pico/bin/pico-gcc -DLIB_PICO_BIT_OPS=1 -DLIB_PICO_BIT_OPS_PICO=1 -DLIB_PICO_DIVIDER=1 -DLIB_PICO_DIVIDER_HARDWARE=1 -DLIB_PICO_DOUBLE=1 -DLIB_PICO_DOUBLE_PICO=1 -DLIB_PICO_FLASH=1 -DLIB_PICO_FLOAT=1 -DLIB_PICO_FLOAT_PICO=1 -DLIB_PICO_I2C_SLAVE=1 -DLIB_PICO_INT64_OPS=1 -DLIB_PICO_INT64_OPS_PICO=1 -DLIB_PICO_MALLOC=1 -DLIB_PICO_MEM_OPS=1 -DLIB_PICO_MEM_OPS_PICO=1 -DLIB_PICO_MULTICORE=1 -DLIB_PICO_PLATFORM=1 -DLIB_PICO_PRINTF=1 -DLIB_PICO_PRINTF_PICO=1 -DLIB_PICO_RAND=1 -DLIB_PICO_RUNTIME=1 -DLIB_PICO_STANDARD_LINK=1 -DLIB_PICO_STDIO=1 -DLIB_PICO_STDIO_UART=1 -DLIB_PICO_STDLIB=1 -DLIB_PICO_SYNC=1 -DLIB_PICO_SYNC_CRITICAL_SECTION=1 -DLIB_PICO_SYNC_MUTEX=1 -DLIB_PICO_SYNC_SEM=1 -DLIB_PICO_TIME=1 -DLIB_PICO_UNIQUE_ID=1 -DLIB_PICO_UTIL=1 -DPARAM_ASSERTIONS_ENABLE_ALL=1 -DPICOUTIL_DEBUG -DPICO_BOARD=\"pico\" -DPICO_BUILD=1 -DPICO_CMAKE_BUILD_TYPE=\"Release\" -DPICO_COPY_TO_RAM=0 -DPICO_CXX_ENABLE_EXCEPTIONS=0 -DPICO_ENTER_USB_BOOT_ON_EXIT=1 -DPICO_GCC_TRIPLE=arm-none-eabi -DPICO_LTO=1 -DPICO_NO_FLASH=0 -DPICO_NO_HARDWARE=0 -DPICO_ON_DEVICE=1 -DPICO_PLATFORM=rp2040 -DPICO_USE_BLOCKED_RAM=0 -DPICO_USE_ORIGINAL_EXIT=1 -DPICO_USE_STACK_GUARDS=1 -D_FORTIFY_SOURCE=2 -D_GNU_SOURCE=1 -I/home/axel/Documents/programmation/mcu/raspi/pico/pw-vault/pico_side/picoutil -I/home/axel/Documents/programmation/mcu/raspi/pico/pw-vault/pico_side/picoutil/.. -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_adc/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_base/include -I/home/axel/Documents/programmation/mcu/raspi/pico/pw-vault/pico_side/build/generated/pico_base -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/boards/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_platform/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2040/hardware_regs/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_base/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2040/hardware_structs/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_claim/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_sync/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_gpio/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_irq/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_sync/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_time/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_timer/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_util/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_resets/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_clocks/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_pll/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_vreg/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_watchdog/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_xosc/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_divider/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_dma/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_exception/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_flash/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_bootrom/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_i2c/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_interp/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_pio/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_pwm/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_rtc/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_spi/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_uart/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_flash/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_multicore/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_i2c_slave/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_rand/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_unique_id/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_stdlib/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_runtime/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_printf/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_bit_ops/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_divider/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_double/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_float/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_malloc/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_binary_info/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_stdio/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_stdio_uart/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_int64_ops/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_mem_ops/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/boot_stage2/include -mcpu=cortex-m0plus -mthumb -mcpu=cortex-m0plus -mfp16-format=ieee -O3 -DNDEBUG -flto=auto -fno-fat-lto-objects -masm-syntax-unified -I/home/axel/Documents/programmation/mcu/raspi/pico/pw-vault/ -I/home/axel/Documents/programmation/mcu/raspi/pico/pw-vault/third_party/chaos-pp/ -fgcse-las -fgcse-sm -fgraphite -fgraphite-identity -static -ftrack-macro-expansion=0 -Wall -Wextra -Werror -ffunction-sections -fdata-sections -Wno-error=strict-aliasing -std=gnu2x
)

add_custom_target(
    recap_gcc_params ALL

    COMMAND ${compile_command} -Q --help=optimizers > ${CMAKE_CURRENT_SOURCE_DIR}/optimization_flags_${CMAKE_BUILD_TYPE}.txt
    COMMAND ${compile_command} -Q --help=warnings > ${CMAKE_CURRENT_SOURCE_DIR}/warning_flags_${CMAKE_BUILD_TYPE}.txt
    COMMAND ${compile_command} -Q --help=target > ${CMAKE_CURRENT_SOURCE_DIR}/target_flags_${CMAKE_BUILD_TYPE}.txt
    COMMAND ${compile_command} -Q --help=common > ${CMAKE_CURRENT_SOURCE_DIR}/common_flags_${CMAKE_BUILD_TYPE}.txt

    DEPENDS pwv_pico_side picoutil picoflash aes_test

    BYPRODUCTS 
        ${CMAKE_CURRENT_SOURCE_DIR}/optimization_flags_${CMAKE_BUILD_TYPE}.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/warning_flags_${CMAKE_BUILD_TYPE}.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/target_flags_${CMAKE_BUILD_TYPE}.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/common_flags_${CMAKE_BUILD_TYPE}.txt
)

message(STATUS "Building for board: ${PICO_BOARD}, with build type: ${CMAKE_BUILD_TYPE}")