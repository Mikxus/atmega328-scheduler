/*
 *  test-context-save.c
 *  Test if saving CPU context on task switch works correctly
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#include "tools/cpu.h"
#include "tools/debug.h"
#include "tools/test.h"

#include "sim_elf.h"
#include "sim_avr.h"
#include "sim_gdb.h"
#include "avr_timer.h"

void timer0_compb_cb(struct avr_irq_t *irq, uint32_t value, void *param)
{
    (void) value;
    if (strcmp(irq->name, ">avr.timer0.compb") != 0) {
        WARN("Incorrect ISR registered: %s should be: >avr.timer0.compb", irq->name);
        return;
    }

    *(bool *) param = 1;
}

int main(int argc, char *argv[])
{
    avr_t *avr = NULL;
    time_t time_seed = time(NULL);
    bool interrupt_fired = 0;
    uint8_t register_values[32] = {0};
    uint16_t register_array_offset = 0;
    
    if (validate_args(argc, argv)){
        return 1;
    }

    avr = init_avr(argv[1], argv[2], atoi(argv[3]));
   
    /*
     * Register callback for timer0 compb interrupt in avr
     * allows us to detect when the context interrupt has run 
     * TIMER_IRQ_OUT_COMP+1 get us compb interrupt... why?
     */
    avr_irq_register_notify(
        avr_io_getirq(avr, AVR_IOCTL_TIMER_GETIRQ('0'), TIMER_IRQ_OUT_COMP+1),
        timer0_compb_cb,
        &interrupt_fired
    );
    
    if (run_avr_until_interrupt(avr, 20000, &interrupt_fired)) 
        return 1;

    /* Now avr is in the timer compb interrupt */
    /* Save current cpu register state */
    for (int i = 0; i < 32; i++)
    {
        register_values[i] = read_register(avr, i);
    }

    // Execute until context save is done
    run_avr_cycles(avr, 91, 0);
    
    // Find task.cpu_state.regs[] array in avr memory
    register_array_offset = find_offset_off(avr, register_values, 32);

    if (!register_array_offset) {
        ERROR("Register values incorrectly saved");
        dump_avr_core(avr);
        return 1;
    }

    srand(time_seed);
    INFO("Filling task.cpu_state.regs[] with random data\nSeed: %ld", time_seed);
    for (int i = 0; i < 32; i++)
    {
        avr->data[register_array_offset + i] = (uint8_t)(rand() % 256); 
        register_values[i] = avr->data[register_array_offset + i];
        INFO("r%d = 0x%02x ", i, register_values[i]);
    }

    // Execute until context restore is done
    run_avr_us(avr, 100, 0);

    // compare if the ISR restored context successfully
    for (int i = 0; i < 32; i++)
    {
        uint8_t reg_value = read_register(avr, i);
        if (reg_value != register_values[i]) {
            ERROR("Register %d value incorrect after context restore: expected: 0x%02x got: 0x%02x",
                i, register_values[i], reg_value);
            dump_avr_core(avr);
            return 1;
        }
    }
    return 0;
}