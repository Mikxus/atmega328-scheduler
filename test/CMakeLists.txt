# Include simavr source files
include_directories(${CMAKE_SOURCE_DIR}/submodules/simavr/simavr/)

# Get all .cpp files in the current directory
file(GLOB ALL_CPP_FILES *.cpp)

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
    add_avr_executable(
        ${NAME} ${FILE}
        )

    set_target_properties(${NAME} PROPERTIES DEPENDS test-${NAME})
    
    avr_target_link_libraries(
        ${NAME}
        scheduler-${AVR_MCU}
    )

    add_dependencies(${NAME} test-${NAME})

endforeach()

# For each test file, create an executable and add it to the tests
foreach(FILE ${TEST_FILES})
    get_filename_component(NAME ${FILE} NAME_WE)

    # Define the output file
    set(OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/${NAME})

    add_custom_target(
        ${NAME}
        COMMAND make -C ${CMAKE_SOURCE_DIR}/test OUTFILE=${OUTPUT_FILE} CC=gcc CXX=g++ FILE=${FILE} 
        DEPENDS ${FILE} simavr
        COMMENT "Building ${NAME} with Makefile"
    )

    set_target_properties(${NAME} PROPERTIES IMPORTED_LOCATION ${OUTPUT_FILE})

    # Make the test depend on the custom target
    add_test(NAME ${NAME} COMMAND ${NAME})

endforeach()
