# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/opt/MCU_development/raspi/pico/pico-sdk/repo/tools/pioasm"
  "/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/pico_side/build/pioasm"
  "/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/pico_side/build/pico-sdk/src/rp2_common/tinyusb/pioasm"
  "/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/pico_side/build/pico-sdk/src/rp2_common/tinyusb/pioasm/tmp"
  "/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/pico_side/build/pico-sdk/src/rp2_common/tinyusb/pioasm/src/PioasmBuild-stamp"
  "/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/pico_side/build/pico-sdk/src/rp2_common/tinyusb/pioasm/src"
  "/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/pico_side/build/pico-sdk/src/rp2_common/tinyusb/pioasm/src/PioasmBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/pico_side/build/pico-sdk/src/rp2_common/tinyusb/pioasm/src/PioasmBuild-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/axel/Documents/programmation/mcu/raspi/pico/store_pw/pico_side/build/pico-sdk/src/rp2_common/tinyusb/pioasm/src/PioasmBuild-stamp${cfgdir}") # cfgdir has leading slash
endif()
