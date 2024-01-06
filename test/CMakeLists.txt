# include simavr's avr headers
include_directories(
    ${CMAKE_SOURCE_DIRl}/submodules/simavr/simavr/sim/avr
)

# Create AVR executables for each test
add_avr_executable(
    test-uart
    test-uart.cpp
)

# Link test executables to the library
link_libraries(
    test-uart-${AVR_MCU}
    scheduler-uart-lib-${AVR_MCU}
)