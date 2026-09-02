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

/*
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
//void soft_yield(void);

/**
 * @brief yields the currently running task immediately
 * @note
 */
void __attribute__((naked, noinline)) yield(void);

/**
 * @brief  Delay task specified ms
 * @note   If there isn't any task's remaining in READY state
 *         the scheduler will automatically enter the mcu in sleep state
 * @param  ms: 
 * @retval None
 */
void delay_ms(uint32_t ms);

#endif // _KERNEL_H_