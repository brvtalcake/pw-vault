# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

# compile ASM with /usr/bin/arm-none-eabi-gcc
# compile C with /usr/bin/arm-none-eabi-gcc
# compile CXX with /usr/bin/arm-none-eabi-g++
ASM_DEFINES = -DCFG_TUSB_MCU=OPT_MCU_RP2040 -DCFG_TUSB_OS=OPT_OS_PICO -DLIB_PICO_BIT_OPS=1 -DLIB_PICO_BIT_OPS_PICO=1 -DLIB_PICO_DIVIDER=1 -DLIB_PICO_DIVIDER_HARDWARE=1 -DLIB_PICO_DOUBLE=1 -DLIB_PICO_DOUBLE_PICO=1 -DLIB_PICO_FIX_RP2040_USB_DEVICE_ENUMERATION=1 -DLIB_PICO_FLASH=1 -DLIB_PICO_FLOAT=1 -DLIB_PICO_FLOAT_PICO=1 -DLIB_PICO_I2C_SLAVE=1 -DLIB_PICO_INT64_OPS=1 -DLIB_PICO_INT64_OPS_PICO=1 -DLIB_PICO_MALLOC=1 -DLIB_PICO_MEM_OPS=1 -DLIB_PICO_MEM_OPS_PICO=1 -DLIB_PICO_MULTICORE=1 -DLIB_PICO_PLATFORM=1 -DLIB_PICO_PRINTF=1 -DLIB_PICO_PRINTF_PICO=1 -DLIB_PICO_RAND=1 -DLIB_PICO_RUNTIME=1 -DLIB_PICO_STANDARD_LINK=1 -DLIB_PICO_STDIO=1 -DLIB_PICO_STDIO_USB=1 -DLIB_PICO_STDLIB=1 -DLIB_PICO_SYNC=1 -DLIB_PICO_SYNC_CRITICAL_SECTION=1 -DLIB_PICO_SYNC_MUTEX=1 -DLIB_PICO_SYNC_SEM=1 -DLIB_PICO_TIME=1 -DLIB_PICO_UNIQUE_ID=1 -DLIB_PICO_UTIL=1 -DPICOUTIL_STATIC_BYTES_SIZE="(1024 * 2 * 2 * 2 * 2)" -DPICO_BOARD=\"pico_w\" -DPICO_BUILD=1 -DPICO_CMAKE_BUILD_TYPE=\"Release\" -DPICO_COPY_TO_RAM=0 -DPICO_CXX_ENABLE_EXCEPTIONS=0 -DPICO_GCC_TRIPLE=arm-none-eabi -DPICO_LTO=1 -DPICO_NO_FLASH=0 -DPICO_NO_HARDWARE=0 -DPICO_ON_DEVICE=1 -DPICO_PLATFORM=rp2040 -DPICO_RP2040_USB_DEVICE_UFRAME_FIX=1 -DPICO_TARGET_NAME=\"storepw_pico_side\" -DPICO_USE_BLOCKED_RAM=0 -D_FORTIFY_SOURCE=2

ASM_INCLUDES = -I/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/pico_side/./picoflash -I/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/pico_side/./picoutil -I/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/pico_side/./src -I/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/pico_side/./third_party/chaos-pp -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_adc/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_base/include -I/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/pico_side/build/generated/pico_base -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/boards/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_platform/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2040/hardware_regs/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_base/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2040/hardware_structs/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_claim/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_sync/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_gpio/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_irq/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_sync/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_time/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_timer/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_util/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_resets/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_clocks/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_pll/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_vreg/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_watchdog/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_xosc/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_divider/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_dma/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_exception/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_flash/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_bootrom/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_i2c/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_interp/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_pio/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_pwm/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_rtc/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_spi/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_uart/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_flash/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_multicore/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_i2c_slave/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_rand/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_unique_id/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_stdlib/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_runtime/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_printf/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_bit_ops/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_divider/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_double/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_float/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_malloc/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_binary_info/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_stdio/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_stdio_usb/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_usb_reset_interface/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_int64_ops/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_mem_ops/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/boot_stage2/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/lib/tinyusb/src -I/opt/MCU_development/raspi/pico/pico-sdk/repo/lib/tinyusb/src/common -I/opt/MCU_development/raspi/pico/pico-sdk/repo/lib/tinyusb/hw -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_fix/rp2040_usb_device_enumeration/include -I/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/pico_side/picoflash/../picoflash -I/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/pico_side/picoflash/../picoutil -I/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/pico_side/picoutil

ASM_FLAGS = -mcpu=cortex-m0plus -mthumb -O3 -DNDEBUG -I/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/ -I/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/third_party/chaos-pp/ -static -ffunction-sections -fdata-sections

C_DEFINES = -DCFG_TUSB_MCU=OPT_MCU_RP2040 -DCFG_TUSB_OS=OPT_OS_PICO -DLIB_PICO_BIT_OPS=1 -DLIB_PICO_BIT_OPS_PICO=1 -DLIB_PICO_DIVIDER=1 -DLIB_PICO_DIVIDER_HARDWARE=1 -DLIB_PICO_DOUBLE=1 -DLIB_PICO_DOUBLE_PICO=1 -DLIB_PICO_FIX_RP2040_USB_DEVICE_ENUMERATION=1 -DLIB_PICO_FLASH=1 -DLIB_PICO_FLOAT=1 -DLIB_PICO_FLOAT_PICO=1 -DLIB_PICO_I2C_SLAVE=1 -DLIB_PICO_INT64_OPS=1 -DLIB_PICO_INT64_OPS_PICO=1 -DLIB_PICO_MALLOC=1 -DLIB_PICO_MEM_OPS=1 -DLIB_PICO_MEM_OPS_PICO=1 -DLIB_PICO_MULTICORE=1 -DLIB_PICO_PLATFORM=1 -DLIB_PICO_PRINTF=1 -DLIB_PICO_PRINTF_PICO=1 -DLIB_PICO_RAND=1 -DLIB_PICO_RUNTIME=1 -DLIB_PICO_STANDARD_LINK=1 -DLIB_PICO_STDIO=1 -DLIB_PICO_STDIO_USB=1 -DLIB_PICO_STDLIB=1 -DLIB_PICO_SYNC=1 -DLIB_PICO_SYNC_CRITICAL_SECTION=1 -DLIB_PICO_SYNC_MUTEX=1 -DLIB_PICO_SYNC_SEM=1 -DLIB_PICO_TIME=1 -DLIB_PICO_UNIQUE_ID=1 -DLIB_PICO_UTIL=1 -DPICOUTIL_STATIC_BYTES_SIZE="(1024 * 2 * 2 * 2 * 2)" -DPICO_BOARD=\"pico_w\" -DPICO_BUILD=1 -DPICO_CMAKE_BUILD_TYPE=\"Release\" -DPICO_COPY_TO_RAM=0 -DPICO_CXX_ENABLE_EXCEPTIONS=0 -DPICO_GCC_TRIPLE=arm-none-eabi -DPICO_LTO=1 -DPICO_NO_FLASH=0 -DPICO_NO_HARDWARE=0 -DPICO_ON_DEVICE=1 -DPICO_PLATFORM=rp2040 -DPICO_RP2040_USB_DEVICE_UFRAME_FIX=1 -DPICO_TARGET_NAME=\"storepw_pico_side\" -DPICO_USE_BLOCKED_RAM=0 -D_FORTIFY_SOURCE=2

C_INCLUDES = -I/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/pico_side/./picoflash -I/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/pico_side/./picoutil -I/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/pico_side/./src -I/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/pico_side/./third_party/chaos-pp -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_adc/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_base/include -I/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/pico_side/build/generated/pico_base -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/boards/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_platform/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2040/hardware_regs/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_base/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2040/hardware_structs/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_claim/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_sync/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_gpio/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_irq/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_sync/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_time/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_timer/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_util/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_resets/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_clocks/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_pll/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_vreg/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_watchdog/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_xosc/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_divider/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_dma/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_exception/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_flash/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_bootrom/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_i2c/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_interp/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_pio/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_pwm/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_rtc/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_spi/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_uart/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_flash/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_multicore/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_i2c_slave/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_rand/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_unique_id/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_stdlib/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_runtime/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_printf/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_bit_ops/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_divider/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_double/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_float/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_malloc/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_binary_info/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_stdio/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_stdio_usb/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_usb_reset_interface/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_int64_ops/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_mem_ops/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/boot_stage2/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/lib/tinyusb/src -I/opt/MCU_development/raspi/pico/pico-sdk/repo/lib/tinyusb/src/common -I/opt/MCU_development/raspi/pico/pico-sdk/repo/lib/tinyusb/hw -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_fix/rp2040_usb_device_enumeration/include -I/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/pico_side/picoflash/../picoflash -I/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/pico_side/picoflash/../picoutil -I/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/pico_side/picoutil

C_FLAGS = -mcpu=cortex-m0plus -mthumb -mcpu=cortex-m0plus -O3 -DNDEBUG -flto=auto -fno-fat-lto-objects -I/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/ -I/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/third_party/chaos-pp/ -static -ffunction-sections -fdata-sections -std=gnu17

CXX_DEFINES = -DCFG_TUSB_MCU=OPT_MCU_RP2040 -DCFG_TUSB_OS=OPT_OS_PICO -DLIB_PICO_BIT_OPS=1 -DLIB_PICO_BIT_OPS_PICO=1 -DLIB_PICO_DIVIDER=1 -DLIB_PICO_DIVIDER_HARDWARE=1 -DLIB_PICO_DOUBLE=1 -DLIB_PICO_DOUBLE_PICO=1 -DLIB_PICO_FIX_RP2040_USB_DEVICE_ENUMERATION=1 -DLIB_PICO_FLASH=1 -DLIB_PICO_FLOAT=1 -DLIB_PICO_FLOAT_PICO=1 -DLIB_PICO_I2C_SLAVE=1 -DLIB_PICO_INT64_OPS=1 -DLIB_PICO_INT64_OPS_PICO=1 -DLIB_PICO_MALLOC=1 -DLIB_PICO_MEM_OPS=1 -DLIB_PICO_MEM_OPS_PICO=1 -DLIB_PICO_MULTICORE=1 -DLIB_PICO_PLATFORM=1 -DLIB_PICO_PRINTF=1 -DLIB_PICO_PRINTF_PICO=1 -DLIB_PICO_RAND=1 -DLIB_PICO_RUNTIME=1 -DLIB_PICO_STANDARD_LINK=1 -DLIB_PICO_STDIO=1 -DLIB_PICO_STDIO_USB=1 -DLIB_PICO_STDLIB=1 -DLIB_PICO_SYNC=1 -DLIB_PICO_SYNC_CRITICAL_SECTION=1 -DLIB_PICO_SYNC_MUTEX=1 -DLIB_PICO_SYNC_SEM=1 -DLIB_PICO_TIME=1 -DLIB_PICO_UNIQUE_ID=1 -DLIB_PICO_UTIL=1 -DPICOUTIL_STATIC_BYTES_SIZE="(1024 * 2 * 2 * 2 * 2)" -DPICO_BOARD=\"pico_w\" -DPICO_BUILD=1 -DPICO_CMAKE_BUILD_TYPE=\"Release\" -DPICO_COPY_TO_RAM=0 -DPICO_CXX_ENABLE_EXCEPTIONS=0 -DPICO_GCC_TRIPLE=arm-none-eabi -DPICO_LTO=1 -DPICO_NO_FLASH=0 -DPICO_NO_HARDWARE=0 -DPICO_ON_DEVICE=1 -DPICO_PLATFORM=rp2040 -DPICO_RP2040_USB_DEVICE_UFRAME_FIX=1 -DPICO_TARGET_NAME=\"storepw_pico_side\" -DPICO_USE_BLOCKED_RAM=0 -D_FORTIFY_SOURCE=2

CXX_INCLUDES = -I/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/pico_side/./picoflash -I/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/pico_side/./picoutil -I/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/pico_side/./src -I/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/pico_side/./third_party/chaos-pp -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_adc/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_base/include -I/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/pico_side/build/generated/pico_base -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/boards/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_platform/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2040/hardware_regs/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_base/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2040/hardware_structs/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_claim/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_sync/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_gpio/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_irq/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_sync/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_time/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_timer/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_util/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_resets/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_clocks/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_pll/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_vreg/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_watchdog/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_xosc/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_divider/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_dma/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_exception/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_flash/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_bootrom/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_i2c/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_interp/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_pio/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_pwm/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_rtc/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_spi/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/hardware_uart/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_flash/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_multicore/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_i2c_slave/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_rand/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_unique_id/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_stdlib/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_runtime/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_printf/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_bit_ops/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_divider/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_double/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_float/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_malloc/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_binary_info/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_stdio/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_stdio_usb/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/common/pico_usb_reset_interface/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_int64_ops/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_mem_ops/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/boot_stage2/include -I/opt/MCU_development/raspi/pico/pico-sdk/repo/lib/tinyusb/src -I/opt/MCU_development/raspi/pico/pico-sdk/repo/lib/tinyusb/src/common -I/opt/MCU_development/raspi/pico/pico-sdk/repo/lib/tinyusb/hw -I/opt/MCU_development/raspi/pico/pico-sdk/repo/src/rp2_common/pico_fix/rp2040_usb_device_enumeration/include -I/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/pico_side/picoflash/../picoflash -I/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/pico_side/picoflash/../picoutil -I/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/pico_side/picoutil

CXX_FLAGS = -mcpu=cortex-m0plus -mthumb -mcpu=cortex-m0plus -O3 -DNDEBUG -flto=auto -fno-fat-lto-objects -I/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/ -I/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/third_party/chaos-pp/ -static -ffunction-sections -fdata-sections -fno-exceptions -fno-unwind-tables -fno-rtti -fno-use-cxa-atexit -std=gnu++20

