add_library(
    test-runner-lib
    test.c
    test.h
    debug.c
    math.c
    cpu.c
)

# check if math.h needs libm to be linked
set(POW_LIBS "")
include(CheckLibraryExists)
check_library_exists(m pow "" LIBM)
if(LIBM)
    list(APPEND POW_LIBS "m")
endif()

target_compile_options(test-runner-lib PRIVATE -g)
link_libraries(test-runner-lib PRIVATE simavr ${POW_LIBS})