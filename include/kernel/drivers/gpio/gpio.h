#ifndef _GPIO_
#define _GPIO_
#include <avr/io.h>
#include <stdbool.h>
#include <kernel/drivers/synchronization/atomic.h>

typedef enum io_port {
    IO_PORTB,
    IO_PORTC,
    IO_PORTD
} io_port;

typedef enum io_data_direction {
    INPUT = 0,
    OUTPUT,
    INPUT_PULLUP,
} io_data_direction;

/**
 * @brief Set the io mode object
 * 
 * @param port 
 * @param pin 
 */
void set_gpio_mode(io_port port, uint8_t pin, io_data_direction dir);

/**
 * @brief Write specified bit to pin  
 * @note doesn't check if pin is specified as input
 * @param bit 
 */
void write_gpio(io_port port, uint8_t pin, bool value);

/**
 * @brief Read pin state
 * 
 * @param port 
 * @param pin 
 * @param value 
 */
bool read_gpio(io_port port, uint8_t pin);

#endif // _GPIO_