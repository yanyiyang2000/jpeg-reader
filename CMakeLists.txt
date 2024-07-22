cmake_minimum_required(VERSION 3.22)

set(CMAKE_C_COMPILER "/usr/bin/gcc-12")
set(CMAKE_CXX_STANDARD 23)

project(JPEG_Reader LANGUAGES C)

add_subdirectory(${PROJECT_SOURCE_DIR}/source)
add_subdirectory(${PROJECT_SOURCE_DIR}/example)
