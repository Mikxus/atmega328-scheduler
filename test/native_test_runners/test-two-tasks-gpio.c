/* 
 * test-two-tasks-gpio.c
 * Test preemptive multitasking with two tasks toggling GPIO pins 
 */
#include <stddef.h>
#include <stdio.h> 
#include <stdlib.h>

#include "tools/test.h"
#include "sim_elf.h"
#include "sim_avr.h"
#include "sim_core.h"
#include "sim_hex.h"

int main(int argc, char *argv[]) {

    avr_t *avr = NULL;

    if (validate_args(argc, argv))
    {
        return 1;
    }

    avr = init_avr(argv[1], argv[2], atoi(argv[3]));
    return run_avr_ms(avr, 2*1000, 0);
}