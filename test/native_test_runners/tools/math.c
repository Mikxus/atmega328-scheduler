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

void calculate_timings(avr_t *avr, struct math_timings_data *data)
{
    float avg_sum = 0.0f; 
    float *temp_buff = malloc(sizeof(float) * data->size);
    data->min = FLT_MAX;
    data->max = FLT_MIN;

    if (!temp_buff) {
        ERROR("Failed to allocate memory for timing calculations");
        return;
    }

    /* Calculate differences between readings */
    for (uint64_t i = 0; i < data->size; i++)
    {
        if (i == 0) 
        {
            /* First reading is inaccurate */
            continue;
        }
        
        temp_buff[i] = convert_cycles_to_frequency(avr,
            data->buffer[i] - data->buffer[i - 1]);

        if (temp_buff[i] < data->min) data->min = temp_buff[i];
        if (temp_buff[i] > data->max) data->max = temp_buff[i];
        avg_sum += temp_buff[i];
    }
    /* move our results back */
    memcpy(&data->buffer[0], &temp_buff[0], sizeof(float) * data->size);

    data->avg = avg_sum / (data->size - 1);

    /* skip the first reading since it's inaccurate */
    data->stddev = calculate_standard_deviation(&data->buffer[1], data->size - 1);
}

bool verify_timing(struct math_timings_data *data,
        float target_avg,
        float target_min,
        float target_max,
        float target_stddev,
        float accuracy)
{
    bool state = 0;

    if (!fequal(data->avg, target_avg, accuracy))
    {
        ERROR("AVG: %0.10f | target: %0.10f | accuracy %0.10f differs too much!", data->avg, target_avg, accuracy);
        state = 1;
    } else {
        INFO("AVG: %0.10f | target: %0.10f | accuracy %0.10f  PASS", data->avg, target_avg, accuracy);
    }

    if (!fequal(data->min, target_min, accuracy))
    {
        ERROR("MIN: %0.10f | target: %0.10f | accuracy %0.10f differs too much!", data->min, target_min, accuracy);
        state = 1;
    } else {
        INFO("MIN: %0.10f | target: %0.10f | accuracy %0.10f  PASS", data->min, target_min, accuracy);
    }

    if (!fequal(data->max, target_max, accuracy))
    {
        ERROR("MAX: %0.10f | target: %0.10f | accuracy %0.10f differs too much!", data->max, target_max, accuracy);
        state = 1;
    } else {
        INFO("MAX: %0.10f | target: %0.10f | accuracy %0.10f  PASS", data->max, target_max, accuracy);
    }

    if (!fequal(data->stddev, target_stddev, accuracy))
    {
        ERROR("STDDEV: %0.10f | target: %0.10f | accuracy %0.10f differs too much!", data->stddev, target_stddev, accuracy);
        state = 1;
    } else {
        INFO("STDDEV: %0.10f | target: %0.10f | accuracy %0.10f  PASS", data->stddev, target_stddev, accuracy);
    }

    return state;
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