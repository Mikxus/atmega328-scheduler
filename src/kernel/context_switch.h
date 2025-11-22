#ifndef _CONTEXT_SWITCH_H_
#define _CONTEXT_SWITCH_H_

#include <inttypes.h>
#include <avr/interrupt.h>
#include "../drivers/timer/timer.h"
#include "cpu.h"
#include "task.h"
#include "../scheduler.h"

/**
 * @brief sets timer2's comp a unit to interrupt at defined intervals
 * 
 */
void initialize_context_switch_timer(uint8_t burst_lenght);

__attribute__((always_inline)) inline void _save_context(void);

#endif // _CONTEXT_SWITCH_H_