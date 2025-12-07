#ifndef _CONTEXT_SWITCH_H_
#define _CONTEXT_SWITCH_H_

#include <inttypes.h>
#include <stddef.h>
#include <avr/interrupt.h>
#include "drivers/timer/timer.h"
#include "cpu.h"
#include "task.h"
#include "scheduler.h"

/**
 * @brief Converts frequency to timer compare value
 * @note  result may overflow if targeted frequency isn't possible with given prescaler 
 */
#define FREQ_TO_TIMER_VALUE(freq, prescaler) (uint8_t)((F_CPU / (prescaler * freq)) - 1)

/**
 * @brief sets timer2's comp a unit to interrupt at defined intervals
 * 
 */
void initialize_context_switch_timer(uint8_t burst_lenght);

void simple_schedule_next_task(void);

uint8_t get_context_switch_burst_length(uint8_t ms);

#endif // _CONTEXT_SWITCH_H_