# Native compiler setup
# also include simavr's headers
set(CMAKE_C_COMPILER    "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER  "/usr/bin/g++")
set(CMAKE_AR            "/usr/bin/ar")
set(CMAKE_RANLIB        "/usr/bin/ranlib")
set(CMAKE_STRIP         "/usr/bin/strip")
set(CMAKE_LINKER        "/usr/bin/ld")
set(CMAKE_ASM_COMPILER  "/usr/bin/as")

include_directories(
  ${CMAKE_SOURCE_DIR}/submodules/simavr/simavr/sim
  ${CMAKE_SOURCE_DIR}/submodules/simavr/include
  ${CMAKE_SOURCE_DIR}/submodules/simavr/simavr
)
