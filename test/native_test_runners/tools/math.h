#ifndef _MATH_H_
#define _MATH_H_

#include <inttypes.h>
#include <stdbool.h>
#include <math.h>

#include "debug.h"
#include "sim/sim_avr.h"

/**
 * @brief calculates the standard deviation for float array
 * 
 * @param data pointer to float array
 * @param size array size
 * @return float standard deviation
 */
float calculate_standard_deviation(float *data, const uint64_t size);

/**
 * @brief compare two floating point numbers to within a precision
 * 
 * @param a 
 * @param b 
 * @param precision 
 * @return float non-zero value if a-b equal within the precision, zero if not equal
 */
float fequal(float a, float b, float precision);

/**
 * converts a delta of clock cycles to a frequency value in Hz.
 *
 * @param avr pointer to an initialized avr_t structure 
 * @param cycles_delta the number of clock cycles to convert.
 * @return the calculated frequency in Hz corresponding to the given cycles_delta.
 */
float convert_cycles_to_frequency(avr_t *avr, float cycles_delta);

#endif