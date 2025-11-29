# build atmega328p scheduler's uart drivers eq. abstractions from the AVR-UART-LIB's functions
add_avr_library(
    clock-lib
    clock.cpp
    clock.h
)

avr_target_link_libraries(
   clock-lib-${AVR_MCU}
   timer-lib-${AVR_MCU} 
)