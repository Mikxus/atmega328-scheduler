#ifndef _KERNEL_H_
#define _KERNEL_H_ 

#include <inttypes.h>
#include <stddef.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include "cpu.h"
#include "task.h"
#include "drivers/clock/clock.h"
#include "drivers/timer/timer.h"
#include "drivers/scheduling/sched.h"
#include "drivers/synchronization/atomic.h"


/**
 * @brief sets timer0's comp b unit to interrupt at defined intervals
 * 
 */
void kernel_init_timer(void);

/**
 * @brief Starts the kernel and performs first context switch
 * @note This function does not return
 */
void __attribute__((noreturn)) kernel_start(void);

/**
 * @brief yields the currently running task by triggering timer0 COMPB match
 * @note It is not quaranteed that the task will yield immediately,
 *      soft_yield will always enable interrupts upon return
 */
void soft_yield(void);

/**
 * @brief yields the currently running task immediately
 * @note Yield will always enable interrupts upon return
 */
void __attribute__((hot, flatten, naked)) yield(void);

#endif // _KERNEL_H_