cmake_minimum_required(VERSION 3.22)

set(CMAKE_C_COMPILER "/usr/bin/gcc-12")
set(CMAKE_CXX_STANDARD 23)

project(JPEG_Reader_Demo LANGUAGES C)

add_subdirectory(${PROJECT_SOURCE_DIR}/source)

add_executable(demo demo.c)

target_link_libraries(
    demo
    PUBLIC
    jpeg-reader
)

target_include_directories(
    demo
    PUBLIC
    "${PROJECT_SOURCE_DIR}/include"
)

target_compile_options(
    demo
    PUBLIC
    -O0
    -g3
    -Wall
)