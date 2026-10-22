# add compile options
add_compile_options(
    ${AVR_DEFAULT_FLAGS}
)

# Generate a library from the scheduler source 
add_avr_library (
    scheduler
    drivers/adc.cpp
    drivers/scheduling/sched.cpp
    drivers/clock.cpp
    drivers/gpio.cpp
    drivers/timer.cpp
    drivers/uart.cpp
    drivers/ws2812b.cpp
    mutex.cpp
    kernel.cpp
    errno.cpp
    event.cpp
    task.cpp
    task_utils.cpp
)

# Link libraries
avr_target_link_libraries(
    scheduler-${AVR_MCU}
    AVR-UART-LIBRARY-${AVR_MCU}
)