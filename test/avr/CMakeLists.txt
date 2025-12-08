# Get all AVR source files
file(GLOB AVR_SRC_FILES *.cpp)

set(CMAKE_CXX_FLAGS "-Wno-c++20-extensions ${CMAKE_CXX_FLAGS}")

foreach(FILE ${AVR_SRC_FILES})
    get_filename_component(NAME ${FILE} NAME_WE)
    message(STATUS "Adding AVR test ${NAME} ${FILE}")

    # Add the AVR executable
    add_avr_executable(${NAME} ${FILE})

    # Set the compile flags
    add_compile_options(
       ${AVR_DEFAULT_FLAGS} 
    )

    # Link necessary libraries
    avr_target_link_libraries(${NAME} scheduler-${AVR_MCU})
endforeach()