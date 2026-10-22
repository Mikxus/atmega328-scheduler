/**
 * @file clock.h
 * @brief Scheduler's timekeeping driver
 *      This driver is responsible for keeping track of uptime
 *      uses timer0's compare a interrupt to increment the uptime
 */
#ifndef _CLOCK_H_
#define _CLOCK_H_

#include <inttypes.h>

/**
 * @brief Initializes timers needed for the scehduler  
 * @note  Uses timer0's overflow interrupt
 * @return None
 */
void clock_init(void);
    
/**
 * @brief Returns the uptime in milliseconds.
 * @return ms
 */
uint32_t get_ms(void);

/**
 * @brief Returns the uptime in microseconds.
 * @return us 
 */
uint32_t get_us(void);

#endif