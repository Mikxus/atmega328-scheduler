add_compile_options(${AVR_DEFAULT_FLAGS})

add_avr_executable(
    task-communication
    main.cpp
)

avr_target_link_libraries(task-communication scheduler-${AVR_MCU})