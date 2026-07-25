/**
 * @file gpio.h  
 * @brief Simple gpio library
 */
#ifndef _GPIO_H_
#define _GPIO_H_

#include <stdint.h>

typedef enum io_port {
    IO_PORTB,
    IO_PORTC,
    IO_PORTD
} io_port_t;

typedef enum io_data_direction {
    INPUT = 0,
    OUTPUT,
    INPUT_PULLUP,
} io_data_direction_t;

/**
 * @brief Set gpio's data direction 
 * 
 * @param port Port to be used: 
 *                  - IO_PORTB 
 *                  - IO_PORTC 
 *                  - IO_PORTD
 * 
 * @param pin  Pin number in range: 1 - 8
 * 
 * @param dir  Pin data direction:
 *                  - INPUT
 *                  - INPUT_PULLUP 
 *                  - OUTPUT
 */
void set_gpio_mode(io_port_t port, uint8_t pin, io_data_direction_t dir);

/**
 * @brief Write specified bit to pin  
 *
 * @note Doesn't check if pin is specified as input
 *
 * @param port Port to be used: 
 *                  - IO_PORTB 
 *                  - IO_PORTC 
 *                  - IO_PORTD
 *
 * @param pin  Pin number in range: 1 - 8
 *
 */
void write_gpio(io_port_t port, uint8_t pin, bool value);

/**
 * @brief Read pin's state
 * 
 * @param port Port to be used: 
 *                  - IO_PORTB 
 *                  - IO_PORTC 
 *                  - IO_PORTD
 *
 * @param pin  Pin number in range: 1 - 8
 */
bool read_gpio(io_port_t port, uint8_t pin);

#endif // _GPIO_H_