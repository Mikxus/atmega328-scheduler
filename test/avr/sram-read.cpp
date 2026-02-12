#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

const uint8_t identifier[] = {0xDE, 0xAD, 0xBE, 0xEF}; 

int main(void)
{
    /* Copy identifier to start of sram */
    memcpy((void *) 0x0, &identifier, sizeof(identifier));

    /* Copy identifier to end of sram */
    memcpy((uint8_t *)RAMEND - sizeof(identifier), &identifier, sizeof(identifier));

    cli();
    sleep_cpu();
}