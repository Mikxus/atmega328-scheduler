#include <stdio.h>
#include <avr/sleep.h>
#include "submodules/simavr/simavr/sim/avr/avr_mcu_section.h"
#include "src/scheduler.h"

#define STRINGIZE(x) #x
#define STRINGIZE_VALUE_OF(x) STRINGIZE(x)

/* 
 * Embed mcu data in the generated ELF file.
 * Useful for the simavr simulator. 
 */
AVR_MCU(F_CPU, STRINGIZE(SCHEDULER_AVR_MCU));

int main(void)
{
    _scheduler_init();
    sei();
    printf("Hellou!\n");

    sleep_cpu();
    return 0;
}