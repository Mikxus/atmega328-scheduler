# Generate simavr
add_custom_target(
    simavr
    COMMAND make build-simavr
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/submodules/simavr/
)