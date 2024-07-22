add_executable(
    demo 
    demo.c
)

target_link_libraries(
    demo
    PRIVATE
    jpeg-reader
)

target_include_directories(
    demo
    PRIVATE
    "${PROJECT_SOURCE_DIR}/include/public"
)

target_compile_options(
    demo
    PRIVATE
    -O0
    -g3
    -Wall
)

install(
    TARGETS
    demo
    DESTINATION
    ${PROJECT_SOURCE_DIR}/example
)
