# include simavr's avr headers
include_directories(
    "${CMAKE_SOURCE_DIRl}/submodules/simavr/simavr/sim/"
)

# Create AVR executables for each test
add_avr_executable(
    test-uart
    test-uart.cpp
)

# Make sure simavr's source is compiled for the tests later on
add_dependencies(
    test-uart-${AVR_MCU}.elf
    simavr
)

# Link test executables to the library
link_libraries(
    test-uart-${AVR_MCU}
    libscheduler-uart-lib-${AVR_MCU}
)

# Add test
add_test(
    NAME test-uart
    COMMAND simavr test-uart-${AVR_MCU}.elf)