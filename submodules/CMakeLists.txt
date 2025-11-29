
# some trickery to get simavr's libsimavr.a to be recognized by CMake
add_library(simavr STATIC IMPORTED )

set_target_properties(simavr PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/submodules/simavr/simavr/obj-x86_64-pc-linux-gnu/libsimavr.so.1
)

# Build simavr
add_custom_target(build-simavr ALL 
    COMMAND make 
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/submodules/simavr/
    COMMENT "Building simavr"
)

# Build simavr's tests
add_custom_target(
    simavr-tests
    COMMAND make build-tests
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/submodules/simavr/
)

# Clean simavr
add_custom_target(
    clean-simavr
    COMMAND make clean
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/submodules/simavr/
)

# Install simavr's emulator to host
add_custom_target(
    install-simavr
    DEPENDS build-simavr
    COMMAND make install
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/submodules/simavr/
)
