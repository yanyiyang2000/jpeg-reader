cmake_minimum_required(VERSION 3.22)

set(CMAKE_C_COMPILER "/usr/bin/gcc-12")

project(Test LANGUAGES C)

add_subdirectory(source)

add_executable(test main.c)

target_link_libraries(test PUBLIC jpeg-reader)

target_include_directories(test PUBLIC "${CMAKE_SOURCE_DIR}/include")

target_compile_options(
    test
    PUBLIC
    -O0
    -g3
    -Wall
)