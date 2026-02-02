#include "gpio.h"

void set_gpio_mode(io_port port, io_data_direction dir, uint8_t pin)
{
    uint8_t *ddrx;
    uint8_t *portx;

    switch (port)
    {
    case IO_PORTB:
        ddrx    = (uint8_t *) DDRB;
        portx   = (uint8_t *) PORTB;
        break;

    case IO_PORTC:
        ddrx    = (uint8_t *) DDRC;
        portx   = (uint8_t *) PORTC;
        break;

    case IO_PORTD:
        ddrx    = (uint8_t*) DDRD;
        portx   = (uint8_t*) PORTD;
        break;
    default:
        return;
    }

    *ddrx &= ~(1 << pin);
    *ddrx |= ((dir & 1) << pin); // (dir & 1) == 1 when dir == OUTPUT
    *portx &= ~(1 << pin);

    if (dir == INPUT_PULLUP)
        *portx |= (1 << pin);
    return;
}

void write_gpio(io_port port, uint8_t pin, bool value)
{
    uint8_t *portx;

    switch (port)
    {
    case IO_PORTB:
        portx   = (uint8_t *) PORTB;
        break;

    case IO_PORTC:
        portx   = (uint8_t *) PORTC;
        break;

    case IO_PORTD:
        portx   = (uint8_t*) PORTD;
        break;
    default:
        return;
    }

    if (value)
        *portx |= (1 << pin);
    else
        *portx &= ~(1 << pin);
    return;
}

bool read_gpio(io_port port, uint8_t pin)
{
    uint8_t *pinx;

    switch (port)
    {
    case IO_PORTB:
        pinx    = (uint8_t *) PINB;
        break;

    case IO_PORTC:
        pinx    = (uint8_t *) PINC;
        break;

    case IO_PORTD:
        pinx    = (uint8_t *) PIND;
        break;
    default:
        return false;
    }

    return ((*pinx & (1 << pin)) != 0);
}