file(GLOB_RECURSE picoflash_SOURCES "*.cpp" "*.hpp" "*.c" "*.h")
add_library(picoflash STATIC ${picoflash_SOURCES})

target_include_directories(picoflash PUBLIC "../picoflash/" "../picoutil/")
target_link_libraries(
    picoflash 
    ${pico_all_modules}
    picoutil
)
target_compile_options(picoflash PRIVATE "-Wall" "-Wextra" "-Werror" "-ftrack-macro-expansion=0" "-Wno-error=cpp")