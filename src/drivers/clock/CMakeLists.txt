# build atmega328p scheduler's uart drivers eq. abstractions from the AVR-UART-LIB's functions
add_avr_library(
    clock-lib
    clock.cpp
    clock.h
)

set_target_properties(
    clock-lib
    PROPERTIES
    COMPILE_FLAGS "${AVR_COMPILE_FLAGS}"
)

avr_target_link_libraries(
   clock-lib-${AVR_MCU}
   timer-lib-${AVR_MCU} 
)