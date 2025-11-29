cmake_minimum_required(VERSION 3.22)

# Explicitly set the Native compilers and tools for x86_64
set(CMAKE_C_COMPILER "/usr/bin/gcc" )
set(CMAKE_CXX_COMPILER "/usr/bin/g++" )
set(CMAKE_AR "/usr/bin/ar")
set(CMAKE_RANLIB "/usr/bin/ranlib")
set(CMAKE_STRIP "/usr/bin/strip")
set(CMAKE_LINKER "/usr/bin/ld")
set(CMAKE_ASM_COMPILER "/usr/bin/as")

# Overwrite the global C/C++ flags explicitly
project(native_test_runners)

# C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/submodules/simavr/simavr/sim)
include_directories(${CMAKE_SOURCE_DIR}/submodules/simavr/include)
include_directories(${CMAKE_SOURCE_DIR}/submodules/simavr/simavr)

# Add tools subdirectory
add_subdirectory(tools)

# Add native test runner's source files
file(GLOB NATIVE_TEST_FILES *.c)

# For each native test file, create an executable and add it to the tests
foreach(FILE ${NATIVE_TEST_FILES})
    get_filename_component(NAME ${FILE} NAME_WE)

    # Generate the avr hex file's name 
    string(REPLACE "test-" "" AVR_TEST_NAME ${NAME})

    message(STATUS "Adding native test ${NAME} ${FILE}")

    # Define the output file
    set(OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/${NAME}")

    # Add an executable target
    add_executable(${NAME} ${FILE})

    add_dependencies(${NAME} build-simavr ${AVR_TEST_NAME} scheduler-${AVR_MCU})

    # check if math.h needs libm to be linked
    set(POW_LIBS "")
    include(CheckLibraryExists)
    check_library_exists(m pow "" LIBM)
    if(LIBM)
        list(APPEND POW_LIBS "m")
    endif()

    target_link_libraries(${NAME} PRIVATE simavr test-runner-lib ${POW_LIBS})
    
    target_compile_options(${NAME} PRIVATE -Wall -Werror -W -Wno-error=unused-parameter -DCONFIF_PULL_UP=1)

    add_test(NAME ${NAME} COMMAND ${OUTPUT_FILE} ${CMAKE_BINARY_DIR}/test/avr/${AVR_TEST_NAME}-${AVR_MCU}.hex ${AVR_MCU} ${MCU_SPEED})
endforeach()