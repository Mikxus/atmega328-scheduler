# Get all AVR source files
file(GLOB AVR_SRC_FILES *.cpp)

set(CMAKE_CXX_FLAGS "-Wno-c++20-extensionsi ${AVR_CXX_FLAGS}")

foreach(FILE ${AVR_SRC_FILES})
    get_filename_component(NAME ${FILE} NAME_WE)
    message(STATUS "Adding AVR source file ${NAME}")
    message(STATUS "Adding AVR source file ${FILE}")

    # Add the AVR executable
    add_avr_executable(${NAME} ${FILE})

    # Set the compile flags
    set_target_properties(
        ${NAME}
        PROPERTIES
        COMPILE_FLAGS "${AVR_COMPILE_FLAGS}"
    )

    # Link necessary libraries
    avr_target_link_libraries(${NAME} scheduler-${AVR_MCU})


endforeach()