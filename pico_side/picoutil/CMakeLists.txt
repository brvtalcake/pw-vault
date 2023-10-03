#[[ set(PICOUTIL_VERSION_MAJOR 0) ]]
#[[ set(PICOUTIL_VERSION_MINOR 1) ]]
#[[ set(PICOUTIL_VERSION_PATCH 0) ]]
#[[ set(PICOUTIL_VERSION_MAJOR_DEFINED 1) ]]
#[[ set(PICOUTIL_VERSION_MINOR_DEFINED 1) ]]
#[[ set(PICOUTIL_VERSION_PATCH_DEFINED 1) ]]

file(GLOB_RECURSE picoutil_SOURCES "*.h" "*.c" "*.hpp" "*.cpp" "*.s" "*.S")
add_library(picoutil STATIC ${picoutil_SOURCES})
#[[ configure_file(picoutil_version.h.in picoutil_version.h @ONLY) ]]

target_link_libraries(
    picoutil
    ${pico_all_modules}
)
target_include_directories(picoutil PUBLIC ${CMAKE_CURRENT_LIST_DIR})
# Add -Wall -Wextra warnings
target_compile_options(picoutil PRIVATE "-ftrack-macro-expansion=0")
target_compile_options(picoutil PUBLIC "-Wall" "-Wextra" "-Werror")
#[[ target_compile_definitions(picoutil PUBLIC _FORTIFY_SOURCE=2) ]]