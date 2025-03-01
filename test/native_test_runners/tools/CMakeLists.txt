add_library(
    test-runner-lib
    test.c
    test.h
    debug.c
)
target_compile_options(test-runner-lib PRIVATE -g)
link_libraries(test-runner-lib PRIVATE simavr)