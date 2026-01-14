/* 
 * test-clock-interrupt.c
 * Tests that the clock interrupt is setup correctly
 */
#include <stddef.h>
#include <stdio.h> 
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <float.h>

/* internal tools */
#include "tools/debug.h"
#include "tools/math.h"
#include "tools/test.h"

/* Simavr */
#include "sim_elf.h"
#include "sim_avr.h"
#include "sim_core.h"
#include "sim_hex.h"
#include "avr_timer.h"

#define SAMPLE_SIZE 500


static void timer0_comp_ovf_cb(struct avr_irq_t *irq, uint32_t value, void *param)
{
    (void) value;
    /*
     * Make sure it was the correct interrupt
     * It seems that simavr internally uses compa's interrupt for the overflow interrupt?
     * maybe a bug? or specified behavior on datasheet?
     */
    if (strcmp(irq->name, ">avr.timer0.compa") != 0)
        return;

    *(bool*) param = 1;
}

int main(int argc, char *argv[]) {

    avr_t *avr = NULL;
    bool interrupt_state = 0;
    struct math_timings_data timings_data = {0};
    timings_data.size = SAMPLE_SIZE;
    timings_data.buffer = malloc(sizeof(float) * timings_data.size);

    if(!timings_data.buffer) {
        ERROR("Failed to allocate memory for timings data");
        return 1;
    }

    if (validate_args(argc, argv))
        return 1;

    avr = init_avr(argv[1], argv[2], atoi(argv[3]));

    /* Register callback for TIMER0 COMPARE overflow interrupt in simavr
     * our callback function then checks if it was triggered by the correct interrupt 
     */
    avr_irq_register_notify(
        avr_io_getirq(avr, AVR_IOCTL_TIMER_GETIRQ('0'), TIMER_IRQ_OUT_COMP),
        timer0_comp_ovf_cb,
        &interrupt_state
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
        0.03f,
        0.021f );
}