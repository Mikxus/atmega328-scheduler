# add compile options
add_compile_options(
    ${AVR_DEFAULT_FLAGS}
)

# Generate a library from the scheduler source 
add_avr_library (
    scheduler
    drivers/uart.cpp
    drivers/timer.cpp
    drivers/clock.cpp
    drivers/gpio.cpp
    mutex.cpp
    drivers/scheduling/sched.cpp
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