cmake_minimum_required(VERSION 3.20)

project(DataStructureProject C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_BUILD_TYPE Debug)

include_directories(include)

file(GLOB_RECURSE SOURCES
    "src/*.c"
)

add_executable(app ${SOURCES})

target_compile_options(app PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)