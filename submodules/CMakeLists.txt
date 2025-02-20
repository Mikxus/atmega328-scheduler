
add_library(simavr STATIC IMPORTED )

set_target_properties(simavr PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/submodules/simavr/simavr/obj-x86_64-pc-linux-gnu/libsimavr.a
)

add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/submodules/simavr/simavr/obj-x86_64-pc-linux-gnu/libsimavr.a
    COMMAND make build-simavr
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/submodules/simavr/
    COMMENT "Building simavr"
)

add_custom_target(build-simavr ALL 
    DEPENDS ${CMAKE_SOURCE_DIR}/submodules/simavr/simavr/obj-x86_64-pc-linux-gnu/libsimavr.a
)


add_custom_target(
    simavr-tests
    COMMAND make build-tests
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
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/submodules/simavr/
)