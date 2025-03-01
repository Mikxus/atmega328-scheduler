# Ignore array bouds warnings for AVR-UART-LIB
add_definitions("-Wno-array-bounds")

# build atmega328p scheduler's uart drivers eq. abstractions from the AVR-UART-LIB's functions
add_avr_library(
    scheduler-uart-lib
    uart.cpp
    uart.h
)

# Link needed libraries
avr_target_link_libraries(
    scheduler-uart-lib-${AVR_MCU}
    AVR-UART-LIBRARY-${AVR_MCU}
)