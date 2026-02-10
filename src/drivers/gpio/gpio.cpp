#include "gpio.h"

void set_gpio_mode(io_port port, uint8_t pin, io_data_direction dir)
{
    volatile uint8_t *ddrx;
    volatile uint8_t *portx;

    switch (port)
    {
    case IO_PORTB:
        ddrx    = &DDRB;
        portx   = &PORTB;
        break;

    case IO_PORTC:
        ddrx    = &DDRC;
        portx   = &PORTC;
        break;

    case IO_PORTD:
        ddrx    = &DDRD;
        portx   = &PORTD;
        break;
    default:
        return;
    }

    ATOMIC_GUARD();
    *ddrx &= ~(1 << pin);
    *ddrx |= ((dir & 1) << pin); // (dir & 1) == 1 when dir == OUTPUT
    *portx &= ~(1 << pin);

    if (dir == INPUT_PULLUP)
        *portx |= (1 << pin);
    return;
}

void write_gpio(io_port port, uint8_t pin, bool value)
{
    volatile uint8_t *portx;

    switch (port)
    {
    case IO_PORTB:
        portx   = &PORTB;
        break;

    case IO_PORTC:
        portx   = &PORTC;
        break;

    case IO_PORTD:
        portx   = &PORTD;
        break;
    default:
        return;
    }

    ATOMIC_GUARD();
    if (value)
        *portx |= (1 << pin);
    else
        *portx &= ~(1 << pin);
    return;
}

bool read_gpio(io_port port, uint8_t pin)
{
    volatile uint8_t *pinx;
    bool value = 0;

    switch (port)
    {
    case IO_PORTB:
        pinx    = &PINB;
        break;

    case IO_PORTC:
        pinx    = &PINC;
        break;

    case IO_PORTD:
        pinx    = &PIND;
        break;
    default:
        return false;
    }

    ATOMIC_BLOCK() {
        value = (*pinx & (1 << pin)) != 0;
    };

    return value;
}