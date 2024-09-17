# Generate simavr
add_custom_target(
    simavr
    COMMAND make build-simavr
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/submodules/simavr/
)

# Clean simavr
add_custom_target(
    simavr-clean
    COMMAND make clean
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/submodules/simavr/
)

# Install simavr's emulator to host
add_custom_target(
    simavr-install
    COMMAND make install
    DEPENDS simavr
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/submodules/simavr/
)