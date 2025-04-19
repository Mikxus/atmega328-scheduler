/**
 * @file clock.h
 * @brief Scheduler's timekeeping driver
 *      This driver is responsible for keeping track of uptime
 *      uses timer0's compare a interrupt to increment the uptime
 */
#ifndef _CLOCK_H_
#define _CLOCK_H_

#include <avr/interrupt.h>
#include <avr/io.h>
#include "../timer/timer.h"



/**
 * @brief Initializes timers needed for the scehduler  
 * @note   
 * @retval None
 */
void initialize_clock(void);
    
/**
 * @brief Returns the uptime in milliseconds
 * @note   
 * @retval 
 */
uint32_t get_ms(void);


#endif