/* 
 * test-clock-interrupt.c
 * Tests that the clock interrupt is setup correctly
 */
#include <stddef.h>
#include <stdio.h> 
#include <stdlib.h>

#include "tools/test.h"
#include "sim_elf.h"
#include "sim_avr.h"
#include "sim_core.h"
#include "sim_hex.h"
#include "avr_timer.h"

volatile bool interrupt_state = 0;

uint64_t timings_buffer[2900] = {0};

static void timer0_comp_cb(struct avr_irq_t *irq, uint32_t value, void *param)
{
    /*
     * Check if COMPA triggered the interrupt
     */
    if (value > 0) return;

    interrupt_state = 1;
}

double convert_cycles_to_frequency(avr_t *avr, uint64_t cycles_delta)
{
    double cycles_per_μs = (1.0 / (double) avr->frequency) * 1000000000.0;
    return 1.0 / ((double) cycles_delta * cycles_per_μs / 1000000000.0);
}

void analyze_timings(avr_t *avr, uint64_t *timings_buffer, size_t size)
{
    uint64_t min = 0;
    uint64_t max = 0;
    float avg = 0.0;
    uint64_t avg_temp = 0; 

    /* Calculate differences between readings */
    for (size_t i = 0; i < size - 1; i++)
    {
        if (i == 0) 
        {
            min = timings_buffer[i];
            continue;
        }

        timings_buffer[i] = timings_buffer[i] - timings_buffer[i - 1];

        avg_temp += timings_buffer[i];

        if (timings_buffer[i] < min) min = timings_buffer[i];
        if (timings_buffer[i] > max) max = timings_buffer[i];
    }

    avg = (float) avg_temp / size;

    INFO("Timings result: ");
    INFO("MIN: %10.6f", convert_cycles_to_frequency(avr, min));
    INFO("MAX: %10.6f", convert_cycles_to_frequency(avr, max));
    INFO("AVG: %10.6f", convert_cycles_to_frequency(avr, avg));
}

int main(int argc, char *argv[]) {

    avr_t *avr = NULL;

    if (validate_args(argc, argv))
    {
        return 1;
    }

    avr = init_avr(argv[1], argv[2], atoi(argv[3]));

    /* Register interrupt for TIMER0 COMPARE interrupt
     * our callback function then checks if it was triggered by the timer0 overflow interrupt 
     */
    avr_irq_register_notify(
        avr_io_getirq(avr, AVR_IOCTL_TIMER_GETIRQ('0'), TIMER_IRQ_OUT_COMP),
        timer0_comp_cb,
        NULL
    );

    for (unsigned int i = 0; i < sizeof(timings_buffer) / sizeof(uint64_t); i++)
    {
        if (!run_avr_until_interrupt(avr, 2000, &interrupt_state))
        {
            interrupt_state = 0;
            timings_buffer[i] = avr->cycle;
        }
    }
    INFO("Analyzing timings");
    analyze_timings(avr, &timings_buffer[0], sizeof(timings_buffer) / sizeof(uint64_t));
    return 1;
}