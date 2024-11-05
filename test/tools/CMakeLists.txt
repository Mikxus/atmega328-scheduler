add_executable(
    test-runner
    test.c
    test.h
)
target_compile_options(test-runner PRIVATE "-Wno-pedantic")
link_libraries(test-runner simavr)