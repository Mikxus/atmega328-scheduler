# dev utilities

include(${CMAKE_SOURCE_DIR}/cmake/native.cmake)

project(util C)

# Simavr helper libary used with tests and dev utils
add_subdirectory(sim_util)