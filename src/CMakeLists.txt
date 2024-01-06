# Include subdirectories
add_subdirectory(drivers/uart)

# Compile time definitions to configure the scheduler's uart drivers
add_definitions(
    -DSCHEDULER_UART_BAUD_RATE=115200
)


# Generate a library from the scheduler source 
add_avr_library (
    scheduler
    scheduler.cpp
    scheduler.h
)

# Link libraries
link_libraries(
    scheduler-${AVR_MCU}
    libAVR-UART-LIB-${AVR_MCU} # Probably not needed to manually link.
)