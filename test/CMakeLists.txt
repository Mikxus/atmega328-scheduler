
include_directories(${CMAKE_SOURCE_DIR}/submodules/simavr/simavr/sim)
include_directories(${CMAKE_SOURCE_DIR}/submodules/simavr/include)
include_directories(${CMAKE_SOURCE_DIR}/submodules/simavr/simavr)

# add subdirectories
add_subdirectory(tools)

# Get all .cpp files in the current directory
file(GLOB ALL_CPP_FILES *.cpp *.c)

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


    # Add an executable target
    add_executable(${NAME} ${FILE})

    set_target_properties(${NAME} PROPERTIES COMPILE_OPTIONS "-Wno-pedantic")
    
    # ensure simavr is built.
    add_dependencies(${NAME} test-runner)

endforeach()
