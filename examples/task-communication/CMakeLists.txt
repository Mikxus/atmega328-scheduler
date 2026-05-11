
add_avr_executable(
    task-communication
    main.cpp
)

add_compile_options(${AVR_DEFAULT_FLAGS})

avr_target_link_libraries(task-communication scheduler-${AVR_MCU})