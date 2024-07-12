cmake_minimum_required(VERSION 3.22)

set(CMAKE_C_COMPILER "/usr/bin/gcc-12")
set(CMAKE_CXX_STANDARD 23)

project(Test LANGUAGES C)

add_subdirectory(${PROJECT_SOURCE_DIR}/source)

add_executable(test main.c)

target_link_libraries(
    test
    PUBLIC
    jpeg-reader
)

target_include_directories(
    test
    PUBLIC
    "${PROJECT_SOURCE_DIR}/include"
)

target_compile_options(
    test
    PUBLIC
    -O0
    -g3
    -Wall
)