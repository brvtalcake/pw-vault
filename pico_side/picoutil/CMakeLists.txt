#[[ set(PICOUTIL_VERSION_MAJOR 0) ]]
#[[ set(PICOUTIL_VERSION_MINOR 1) ]]
#[[ set(PICOUTIL_VERSION_PATCH 0) ]]
#[[ set(PICOUTIL_VERSION_MAJOR_DEFINED 1) ]]
#[[ set(PICOUTIL_VERSION_MINOR_DEFINED 1) ]]
#[[ set(PICOUTIL_VERSION_PATCH_DEFINED 1) ]]

# Configure picoutil_stdtypes.h.in to picoutil_stdtypes.h
#     1) Get size of _Float16
#     2) Get size of float
#     3) Get size of double
#     4) Get size of long double
#     5) Configure picoutil_stdtypes.h.in to picoutil_stdtypes.h
set(PICOUTIL_STDTYPES_H_IN "${CMAKE_CURRENT_LIST_DIR}/picoutil_stdtypes.h.in")
set(PICOUTIL_STDTYPES_H "${CMAKE_CURRENT_LIST_DIR}/picoutil_stdtypes.h")

# 1) Get size of _Float16
include(CheckTypeSize)
check_type_size("__fp16" MEASURED_SIZEOF_FLOAT16 LANGUAGE C)
if(NOT MEASURED_SIZEOF_FLOAT16)
    message(FATAL_ERROR "Could not determine size of _Float16")
endif()
message(STATUS "Size of _Float16: ${MEASURED_SIZEOF_FLOAT16}")
set(PICOUTIL_SIZEOF_FLOAT16 ${MEASURED_SIZEOF_FLOAT16})

# 2) Get size of float
check_type_size("float" MEASURED_SIZEOF_FLOAT LANGUAGE C)
if(NOT MEASURED_SIZEOF_FLOAT)
    message(FATAL_ERROR "Could not determine size of float")
endif()
message(STATUS "Size of float: ${MEASURED_SIZEOF_FLOAT}")
set(PICOUTIL_SIZEOF_FLOAT ${MEASURED_SIZEOF_FLOAT})

# 3) Get size of double
check_type_size("double" MEASURED_SIZEOF_DOUBLE LANGUAGE C)
if(NOT MEASURED_SIZEOF_DOUBLE)
    message(FATAL_ERROR "Could not determine size of double")
endif()
message(STATUS "Size of double: ${MEASURED_SIZEOF_DOUBLE}")
set(PICOUTIL_SIZEOF_DOUBLE ${MEASURED_SIZEOF_DOUBLE})

# 4) Get size of long double
check_type_size("long double" MEASURED_SIZEOF_LONG_DOUBLE LANGUAGE C)
if(NOT MEASURED_SIZEOF_LONG_DOUBLE)
    message(FATAL_ERROR "Could not determine size of long double")
endif()
message(STATUS "Size of long double: ${MEASURED_SIZEOF_LONG_DOUBLE}")
set(PICOUTIL_SIZEOF_LONG_DOUBLE ${MEASURED_SIZEOF_LONG_DOUBLE})

# 5) Configure picoutil_stdtypes.h.in to picoutil_stdtypes.h
configure_file(${PICOUTIL_STDTYPES_H_IN} ${PICOUTIL_STDTYPES_H} @ONLY)



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