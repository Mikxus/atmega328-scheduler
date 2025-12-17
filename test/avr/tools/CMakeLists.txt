MESSAGE(STATUS "tools included")
add_compile_options(
    ${AVR_DEFAULT_FLAGS}
)

add_avr_library(
    unittest
    unittest.cpp
)
