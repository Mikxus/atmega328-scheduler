/**
 * @file test-get-us-stability.c
 * @brief Test to get the stability of get_us() function over time
 *  mcu code generates writes pb4 to high & low each ms
 * Testing on real hardware indicates that get_us returns spuriously incorrect values
 * leading to generating pulses mostly at 1 ms but also at lower times
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
#include "avr_ioport.h"

#define SAMPLE_SIZE 1000


static void pb4_state_change_cb(struct avr_irq_t *irq, uint32_t value, void *param)
{
    (void) value;

    /* Aknowledge interrupt only on the rising edge */
    if (irq->value == 1)
        *(bool *) param = 1;
}

int main(int argc, char *argv[]) {

    avr_t *avr = NULL;
    avr_irq_t *base_irq = NULL; 
    bool interrupt_state = 0;
    struct math_timings_data timings_data = {0};
    timings_data.size = SAMPLE_SIZE;
    timings_data.buffer = malloc(sizeof(float) * SAMPLE_SIZE);

    if (!timings_data.buffer) {
        ERROR("Failed to allocate memory for timings data");
        return 1;
    }

    if (validate_args(argc, argv))
        return 1;

    avr = init_avr(argv[1], argv[2], atoi(argv[3]));

    /* Get the start of DDRB port's irq list*/
    base_irq = avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('B'), 0);

    /*
     * Register interrupt when mcu changes pb4 pin's state
     */
    avr_irq_register_notify(
        base_irq + IOPORT_IRQ_PIN4,
        pb4_state_change_cb,
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
#ifdef RELEASE_BUILD
    return verify_timing(&timings_data,
        976.4f,
        973.50f,
        977.5f,
        1.3f,
        5.0f );
#endif

#ifdef DEBUG_BUILD
    return verify_timing(&timings_data,
        976.0f,
        973.50f,
        977.0f,
        1.6f,
        6.0f );
#endif

    // If no build type defined, fail the test
    return 1;
}