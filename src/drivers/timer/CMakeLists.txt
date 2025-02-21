# build atmega328p scheduler's uart drivers eq. abstractions from the AVR-UART-LIB's functions
add_avr_library(
    timer-lib
    timer.cpp
    timer.h
)

set_target_properties(
    timer-lib
    PROPERTIES
    COMPILE_FLAGS "${AVR_COMPILE_FLAGS}"
)