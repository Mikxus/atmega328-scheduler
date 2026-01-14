#include <stdio.h>
#include <string.h>
#include <avr/sleep.h>
#include <avr/io.h>
#include "src/scheduler.h"

int main(void)
{
    initialize_clock();

    // Set PB4 as output
    DDRB = 0b00010000;
    uint32_t last_us = get_us();           
    while(true){
        if (get_us() - last_us >= 1000){
            PORTB = 0b00010000;
            // Busy wait for a bit to ensure get_us returns
            // different value next time
            for (volatile uint32_t i = 0; i < 10; i++)
                _NOP();
            PORTB = 0b00000000;
            last_us = get_us();
        }
    }
}