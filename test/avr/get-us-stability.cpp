#include <stdio.h>
#include <string.h>
#include <avr/sleep.h>
#include <avr/io.h>
#include "src/scheduler.h"

int main(void)
{
    init_scheduler();
   
    // Set PB4 as output
    DDRB = 0b00010000;
    uint32_t last_us = get_us();           
    while(true){
        if (get_us() - last_us >= 1000){
            PORTB = 0b00010000;
            last_us = get_us();
            PORTB ^= 0b00010000;
        }
    }
}