# Include subdirectories
add_subdirectory(drivers/uart)
add_subdirectory(drivers/timer)
add_subdirectory(drivers/clock)

# Generate a library from the scheduler source 
add_avr_library (
    scheduler
    scheduler.cpp
    scheduler.h
)

set_target_properties(
    scheduler
    PROPERTIES
    COMPILE_FLAGS "${AVR_COMPILE_FLAGS}"
)

# Link libraries
avr_target_link_libraries(
    scheduler-${AVR_MCU}
    scheduler-uart-lib-${AVR_MCU}
    timer-lib-${AVR_MCU}
    clock-lib-${AVR_MCU}
)