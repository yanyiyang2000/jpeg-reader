cmake_minimum_required(VERSION 3.22)

set(CMAKE_C_COMPILER "/usr/bin/gcc-12")

project(Test LANGUAGES C)

add_subdirectory(source)

add_executable(test main.c)

target_link_libraries(test PUBLIC exif_reader)

target_include_directories(test PUBLIC "${CMAKE_SOURCE_DIR}/include")