# Compile time definitions to configure the scheduler's uart drivers
add_definitions(
    -DSCHEDULER_UART_BAUD_RATE=115200
)

message(STATUS "Scheduler's UART drivers")

# build atmega328p scheduler's uart drivers eq. abstractions from the AVR-UART-LIB's functions
add_avr_library(
    scheduler-uart-lib
    uart.cpp
    uart.h
)

# Link needed libraries
link_libraries(
    scheduler-uart-lib-${AVR_MCU}
    libAVR-UART-LIB-${AVR_MCU}.a
)