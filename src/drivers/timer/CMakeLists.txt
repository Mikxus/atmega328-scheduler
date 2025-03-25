# build atmega328p scheduler's uart drivers eq. abstractions from the AVR-UART-LIB's functions
add_avr_library(
    timer-lib
    timer.cpp
    timer.h
)