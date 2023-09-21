cmake_minimum_required(VERSION 3.12)

project(storepw C CXX ASM)

# Set build time (from `date` shell command if Linux, etc...), author and version variables
exec_program("date '+%Y-%m-%d at %H:%M:%S'" OUTPUT_VARIABLE BUILD_DATE)
set(BUILD_AUTHOR "Axel PASCON")
set(BUILD_VERSION "0.1.0")

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.hpp" "src/*.c" "src/*.h")
add_executable(storepw SOURCES)
