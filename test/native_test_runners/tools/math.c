#include "math.h"

float calculate_standard_deviation(float *data, const uint64_t size)
{
    float avg = 0.0f;
    float values = 0.0f;

    for (uint64_t i = 0; i < size; i++)
    {
        avg += data[i];
    }

    avg /= (float) size;

    for (uint64_t i = 0; i < size; i++)
    {
        values += pow(data[i] - avg, 2.0f);
    }

    return sqrt(values / size);
}

float fequal(float a, float b, float precision)
{
    return fabs(a - b) < precision;
}


float convert_cycles_to_frequency(avr_t *avr, float cycles_delta)
{
    float cycles_per_μs = (1.0f / (float) avr->frequency) * 1000000000.0f;
    return 1.0f / (cycles_delta * cycles_per_μs / 1000000000.0f);
}