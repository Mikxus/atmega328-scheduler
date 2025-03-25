#include <stdio.h>
#include <string.h>
#include <avr/sleep.h>
#include <avr/io.h>
#include "src/scheduler.h"

const uint8_t identifier[] = {0xDE, 0xAD, 0xBE, 0xEF}; 

int main(void)
{
    /* Copy identifier to start of sram */
    memcpy((void *) 0x0, &identifier, sizeof(identifier));

    /* Copy identifier to end of sram */
    memcpy((void *) RAMEND - sizeof(identifier), &identifier, sizeof(identifier));

    cli();
    sleep_cpu();
}