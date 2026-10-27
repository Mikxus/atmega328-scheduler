add_library(
    sim-util
    elf.c
    test.c
    debug.c
    math.c
    cpu.c
)

# check if math.h needs libm to be linked
set(POW_LIBS "")
include(CheckLibraryExists)
check_library_exists(m pow "" LIBM)
if(LIBM)
    list(APPEND POW_LIBS "m")
endif()

# Exporting the parent keeps math.h & elf.h from shadowing system libraries.
# To use sim util "#include sim_util/elf.h" 
target_include_directories(sim-util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_compile_options(sim-util PRIVATE -g)
target_link_libraries(sim-util PUBLIC simavr ${POW_LIBS} elf)
add_dependencies(sim-util build-simavr)