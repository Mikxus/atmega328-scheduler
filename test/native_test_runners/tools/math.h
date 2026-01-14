#ifndef _MATH_H_
#define _MATH_H_

#include <stdlib.h>
#include <stdbool.h>
#include <float.h>
#include <inttypes.h>
#include <string.h>
#include <math.h>

#include "debug.h"
#include "sim/sim_avr.h"

struct math_timings_data
{
    float min;
    float max;
    float avg;
    float stddev;
    uint64_t size;
    float *buffer;
};

/**
 * @brief calculates the standard deviation for float array
 * 
 * @param data pointer to float array
 * @param size array size
 * @return float standard deviation
 */
float calculate_standard_deviation(float *data, const uint64_t size);

/**
 * @brief Calculate freqeuncy timings from cycle readings
 * 
 * @param avr 
 * @param data current cycles between "changes"
 */
void calculate_timings(avr_t *avr, struct math_timings_data *data);

/**
 * @brief Verify timing data agains target values
 * 
 * @param data 
 * @param target_avg 
 * @param target_min 
 * @param target_max 
 * @param target_stddev 
 * @param accuracy 
 * @return true if all verifications pass 
 * @return false 
 */
bool verify_timing(struct math_timings_data *data,
        float target_avg,
        float target_min,
        float target_max,
        float target_stddev,
        float accuracy);
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