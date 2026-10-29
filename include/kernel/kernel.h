/*
 * @file kernel.h
 * @brief 
 */
#ifndef _KERNEL_H_
#define _KERNEL_H_ 

#include <inttypes.h>

/**
 * @brief sets timer0's comp b unit to interrupt at defined intervals
 * 
 */
void kernel_init_timer(void);

/**
 * @brief Initializes subsystems
 * @retval None
 */
void kernel_init(void);

/**
 * @brief Starts the kernel and performs first context switch
 * @note This function does not return
 */
void __attribute__((noreturn)) kernel_start(void);

/**
 * @brief yields the currently running task by triggering timer0 COMPB match
 * @note Task will yield at the next timer tick
 *       soft_yield will most likely exit before the current task yields
 */
void soft_yield(void);

/**
 * @brief yields the currently running task immediately.
 * @note  Do not use inside interrupts to yield currently running task.
 *      
 */
void __attribute__((naked, noinline)) yield(void);

/**
 * @brief yields the interrupted task immediately. Use this instead of yield()
 *        at the end of an isr.
 * @note  Does nothing if no task is running (scheduler is idle or kernel
 *        hasn't started).
 */
void yield_isr(void);

/**
 * @brief  Delay task specified ms
 * @param  ms: 
 * @retval None
 */
void delay_ms(uint16_t ms);

#endif // _KERNEL_H_