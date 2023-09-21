file(GLOB_RECURSE picoutil_SOURCES "*.h" "*.c" "*.hpp" "*.cpp" "*.s" "*.S")
add_library(picoutil STATIC ${picoutil_SOURCES})

target_link_libraries(
    picoutil
    ${pico_all_modules}
)
target_include_directories(picoutil PUBLIC ${CMAKE_CURRENT_LIST_DIR})
#[[ target_compile_definitions(picoutil PUBLIC _FORTIFY_SOURCE=2) ]]