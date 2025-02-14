# Include subdirectories
add_subdirectory(drivers/uart)

# Generate a library from the scheduler source 
add_avr_library (
    scheduler
    scheduler.cpp
    scheduler.h
)

# Link libraries
avr_target_link_libraries(
    scheduler-${AVR_MCU}
    scheduler-uart-lib-${AVR_MCU}
)