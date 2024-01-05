# Generate a library from the scheduler source 
add_avr_library (
    scheduler
    main.cpp
)

# Link libraries
link_libraries(
    scheduler-${AVR_MCU}
    scheduler-uart-lib-${AVR_MCU} # Probably not needed to manually link.
)