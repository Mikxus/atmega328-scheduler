# include simavr's avr headers
include_directories(
    ${CMAKE_SOURCE_DIRl}/submodules/simavr/simavr/sim/avr
)

# Create AVR executables for each test
add_avr_executable(
    test-uart
    test-uart.cpp
)

<<<<<<< Updated upstream
# Link test executables to the library
link_libraries(
    test-uart-${AVR_MCU}
    scheduler-uart-lib-${AVR_MCU}
)
=======
# Lists to hold the AVR and test files
set(AVR_FILES "")
set(TEST_FILES "")

# Iterate over all .cpp files
foreach(FILE ${ALL_CPP_FILES})
    # Get the filename without the directory
    get_filename_component(NAME ${FILE} NAME)

    # Check if the filename starts with "test-"
    if(NAME MATCHES "^test-.*")
        # Add it to the list of test files
        list(APPEND TEST_FILES ${FILE})
    else()
        # Add it to the list of AVR files
        list(APPEND AVR_FILES ${FILE})
    endif()
endforeach()

# For each AVR file, create an AVR executable
foreach(FILE ${AVR_FILES})
    get_filename_component(NAME ${FILE} NAME_WE)
    add_avr_executable(${NAME} ${FILE})
    message(STATUS "Adding AVR test ${NAME}")
    message(STATUS "Adding AVR test ${FILE}")
    avr_target_link_libraries(
        ${NAME}
        scheduler-${AVR_MCU}
        )
endforeach()

# For each test file, create an executable and add it to the tests
foreach(FILE ${TEST_FILES})
    get_filename_component(NAME ${FILE} NAME_WE)
    message(STATUS "Adding test ${NAME}")
    message(STATUS "Adding test ${FILE}")

    # Define the output file
    set(OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/${NAME}")

    # Add a custom command that will be executed during the build phase
    add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        COMMAND CC=gcc CXX=g++ make -C ${CMAKE_SOURCE_DIR}/test FILE=${FILE}
        DEPENDS ${FILE}
        COMMENT "Building ${NAME} with Makefile"
    )

    # Add an executable target
    add_executable(${NAME} IMPORTED GLOBAL)
    set_target_properties(${NAME} PROPERTIES IMPORTED_LOCATION ${OUTPUT_FILE})

    # Add a custom target that depends on the custom command
    add_custom_target("HOST_TARGET_${NAME}" DEPENDS ${OUTPUT_FILE})

    # Make the test depend on the custom target
    add_test(NAME ${NAME} COMMAND ${NAME})
endforeach()
>>>>>>> Stashed changes
