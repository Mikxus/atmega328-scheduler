# Ignore array bounds warnings for AVR-UART-LIBRARY
add_definitions("-w")

add_compile_options(
    ${AVR_DEFAULT_FLAGS}
)

# CMakeLists.txt for the uart library
add_avr_library(
    AVR-UART-LIBRARY
    usart.c
    usart.h
)