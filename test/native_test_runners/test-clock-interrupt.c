/* 
 * test-clock-interrupt.c
 * Tests that the clock interrupt is setup correctly
 */
#include <stddef.h>
#include <stdio.h> 
#include <stdlib.h>
#include <float.h>
#include <stdbool.h>

/* internal tools */
#include "tools/debug.h"
#include "tools/test.h"
#include "tools/math.h"

/* Simavr */
#include "sim_elf.h"
#include "sim_avr.h"
#include "sim_core.h"
#include "sim_hex.h"
#include "avr_timer.h"

#define SAMPLE_SIZE 500

volatile bool interrupt_state = 0;

struct timings_data
{
    float min;
    float max;
    float avg;
    float stddev;
    uint64_t size;
    float buffer[SAMPLE_SIZE];
};

static void timer0_comp_ovf_cb(struct avr_irq_t *irq, uint32_t value, void *param)
{
    /*
     * Check if TIMER0 overflow triggered the interrupt
     */
    if (value > 1) return;

    interrupt_state = 1;
}

void calculate_timings(avr_t *avr, struct timings_data *data)
{
    float avg_sum = 0.0f; 
    float temp_buff[SAMPLE_SIZE];
    data->min = FLT_MAX;
    data->max = FLT_MIN;

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
    memcpy(&data->buffer[0], &temp_buff[0], sizeof(float) * SAMPLE_SIZE);

    data->avg = avg_sum / (data->size - 1);

    /* skip the first reading since it's inaccurate */
    data->stddev = calculate_standard_deviation(&data->buffer[1], sizeof(data->buffer) / sizeof(float) - 1);
}

bool verify_timing(struct timings_data *data,
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

int main(int argc, char *argv[]) {

    avr_t *avr = NULL;
    struct timings_data timings_data;
    timings_data.size = SAMPLE_SIZE;
    timings_data.min = 0.0F;
    timings_data.max = 0.0F;
    timings_data.avg = 0.0F;

    if (validate_args(argc, argv))
    {
        return 1;
    }

    avr = init_avr(argv[1], argv[2], atoi(argv[3]));

    /* Register interrupt for TIMER0 COMPARE overflow interrupt
     * our callback function then checks if it was triggered by the timer0 overflow interrupt 
     */
    avr_irq_register_notify(
        avr_io_getirq(avr, AVR_IOCTL_TIMER_GETIRQ('0'), TIMER_IRQ_OUT_COMP),
        timer0_comp_ovf_cb,
        NULL
    );

    /* collect timing samples */
    for (unsigned int i = 0; i < timings_data.size; i++)
    {
        if (run_avr_until_interrupt(avr, 2000, &interrupt_state))
        {
            ERROR("Test failed avr did not interrupt in time");
            return 1;
        }

        interrupt_state = 0;
        timings_data.buffer[i] = avr->cycle;
    }

    calculate_timings(avr, &timings_data);

    /* Verify outputs */
    return verify_timing(&timings_data,
        976.57f,
        976.50f,
        976.62f,
        0.02f,
        0.021f );
}