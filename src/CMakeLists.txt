# add compile options
add_compile_options(
    ${AVR_DEFAULT_FLAGS}
)

# Generate a library from the scheduler source 
add_avr_library (
    scheduler
    drivers/uart/uart.cpp
    drivers/timer/timer.cpp
    drivers/clock/clock.cpp
    drivers/synchronization/mutex.cpp
    drivers/scheduling/sched.cpp
    context_switch.cpp
    scheduler.cpp
    task.cpp
    scheduler.h
)

# Link libraries
avr_target_link_libraries(
    scheduler-${AVR_MCU}
    AVR-UART-LIBRARY-${AVR_MCU}
)