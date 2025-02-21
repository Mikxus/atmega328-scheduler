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
 * @brief  Initialize the clock
 * @note   
 * @retval None
 */
void initialize_clock(void);
    

#endif