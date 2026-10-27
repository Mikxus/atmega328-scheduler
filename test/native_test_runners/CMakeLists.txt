cmake_minimum_required(VERSION 3.22)

include(${CMAKE_SOURCE_DIR}/cmake/native.cmake)

project(native_test_runners)

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


    target_link_libraries(${NAME} PRIVATE sim-util)
    target_compile_options(${NAME} PRIVATE -Wall -Werror -W -Wno-error=unused-parameter -DCONFIF_PULL_UP=1)

    add_test(NAME ${NAME} COMMAND ${OUTPUT_FILE} ${CMAKE_BINARY_DIR}/test/avr/${AVR_TEST_NAME}-${AVR_MCU}.elf ${AVR_MCU} ${MCU_SPEED})
    set_tests_properties(${NAME} PROPERTIES SKIP_RETURN_CODE 77)
endforeach()