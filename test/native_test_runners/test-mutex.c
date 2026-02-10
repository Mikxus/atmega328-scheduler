/* 
 * test-fifo.c
 * Test that two tasks can't acquire mutex at the same time
 * task 1 sets PB4 high when it has mutex
 * task 2 sets PB3 high when it has mutex
 * If both pins are high at the same time, the test fails
 */
#include <stddef.h>
#include <stdio.h> 
#include <stdlib.h>
#include <math.h>

#include "tools/test.h"
#include "avr_ioport.h"
#include "sim_elf.h"
#include "sim_avr.h"
#include "sim_core.h"

struct pin_state_change
{
    int pin_number;
    unsigned int value;
    bool state_changed;
    int change_count;
};

void pb4_state_change_cb(struct avr_irq_t *irq,
    uint32_t value,
    void *param)
{
    (void) value;
    struct pin_state_change *ptr = (struct pin_state_change *) param;

    ptr->pin_number = 4;
    ptr->value = !irq->value;
    ptr->state_changed = 1;
    ptr->change_count += 1;
}

void pb3_state_change_cb(struct avr_irq_t *irq,
    uint32_t value,
    void *param)
{
    (void) value;
    struct pin_state_change *ptr = (struct pin_state_change *) param;

    ptr->pin_number = 3;

    /* Invert irq->value as it seems that simavr returns inverted value?
    *  i have no idea wtf. Atleast on the scope the pb4 % pb3 signals aren't inverted
    *  Maybe its due to some kind of pullup resistor simulation?  
    *  I don't think i've enabled any pullups
    */
    ptr->value = !irq->value; 
    ptr->state_changed = 1;
    ptr->change_count += 1;
}

bool run_avr_until_pin_change(avr_t *avr,
    struct pin_state_change pb_change[],
    size_t pin_count,
    uint64_t timeout_ms)
{
    avr_cycle_count_t timeout_cycles = avr->cycle + (avr->frequency / 1000) * timeout_ms;

    while (avr->cycle < timeout_cycles)
    {
        avr_run(avr);

        for (size_t i = 0; i < pin_count; i++) {
            if (pb_change[i].state_changed == 1) {
                pb_change[i].state_changed = 0;
                return 0;
            }
        }
    }

    WARN("%s: did not acquire mutex once in %llu ms", __func__, timeout_ms);
    dump_avr_core(avr);
    return 1;
}

bool test_mutex_violation(avr_t *avr,
    struct pin_state_change pb_change[],
    size_t pin_count,
    uint64_t test_time_ms,
    uint64_t pin_change_timeout_ms)
{
    avr_cycle_count_t test_timeout_cycles = avr->cycle + (avr->frequency / 1000) * test_time_ms;
    size_t high_count = 0;

    while (avr->cycle < test_timeout_cycles)
    {
        high_count = 0;
        if (run_avr_until_pin_change(avr, pb_change, pin_count, pin_change_timeout_ms))
            return 1;

        // Count high pins
        for (size_t i = 0; i < pin_count; i++)
        {
            if (pb_change[i].value)
                high_count += 1;
        }

        // Check for mutex violation
        if (high_count > 1) {
            ERROR("Mutex violation high count: %d", high_count);
            for (size_t i = 0; i < pin_count; i++)
            {
                INFO("PIN%d: value: %d change count: %d",
                    pb_change[i].pin_number,
                    pb_change[i].value,
                    pb_change[i].change_count);
            }
            dump_avr_core(avr);
            //enter_gdb_debug(avr, 1234);
            return 1;
        }

    }

    // Check if all tasks acuired mutex atleast once
    for (size_t i = 0; i < pin_count; i++)
    {
        if (pb_change[i].change_count == 0) {
            ERROR("PIN %d: Did not acquire mutex even once", pb_change[i].pin_number);
            return 1;
        }
    }

    return 0;
}

int main(int argc, char *argv[]) {

    avr_irq_t *base_irq = NULL;
    avr_t *avr = NULL;
    struct pin_state_change pb_state[2]= {0};

    if (validate_args(argc, argv))
        return 1;

    avr = init_avr(argv[1], argv[2], atoi(argv[3]));

    base_irq = avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('B'), 0);

    // Register callback for pb4 pin state change
    avr_irq_register_notify(
        base_irq + IOPORT_IRQ_PIN4,
        pb4_state_change_cb,
        &pb_state[0]);

    // Register callback for pb3 pin state change
    avr_irq_register_notify(
        base_irq + IOPORT_IRQ_PIN3,
        pb3_state_change_cb,
        &pb_state[1]);

    return test_mutex_violation(avr,
        pb_state,
        sizeof(pb_state) / sizeof(pb_state[0]),
        10000,
        1);
}