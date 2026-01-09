#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <stdio.h>
#include <avr/cpufunc.h>

#include "task.h"
#include "kernel.h"
#include "drivers/uart/uart.h" 
#include "drivers/clock/clock.h"

/**
 * @brief Initializes the scheduler
 */
void init_scheduler(void);

/**
 * @brief Starts the scheduler
 * @note Forces context switch to the first task this doesn't return
 */
void __attribute__((noreturn)) start_scheduler(void);

/**
 * @brief Adds a new task to the scheduler
 * 
 * @param task 
 * @param name task name
 * @param priority task priority (0 - 255)
 * @param slice_ms lenght of time slice in milliseconds
 * @param entry Task entry function ptr
 * @return true 
 * @return false 
 */
bool create_task(
    task_data_t &task,
    volatile uint8_t *stack_array,
    const uint16_t stack_size,
    const char *name,
    const uint8_t priority,
    const uint8_t slice_ms,
    void (entry)(void));

/* 
* Setup register & stack for Args... 
* avr-libc call-used registers (r18-27, r30-r31)
* Arguments allocated left to right r25-r8
*      All arguments aligned to start in even-numbered registers
*      odd-sized arguments have one free register above them
*/
template <typename T>
void _process_arg(
    volatile uint8_t *stack_array,
    const uint16_t stack_size,
    volatile uint8_t *arg_ptr,
    uint8_t &bytes_used,
    T arg)
{
    uint8_t arg_size = sizeof(T);
    bool padding = arg_size % 2;

    printf("size of arg %d\n", arg_size);
    uart0_flush();

    /* Check if current arg would go over the call registers */
    if (bytes_used + arg_size + padding > 17 && bytes_used <= 17) {
        printf("arguments doesn't fit in regs: %d\n", arg_size);
        uart0_flush();
        /* 
        * set arg_ptr to stack's SP 
        * Sp is set at stack_size - 3
        */
        arg_ptr = (uint8_t*) &stack_array[stack_size - 3];
    }

    arg_ptr -= arg_size + padding;
    bytes_used =+ arg_size + padding;
    memcpy((void*) arg_ptr, &arg, arg_size);
    printf("Arg base addr %p size %d\n",arg_ptr , arg_size);
    uart0_flush();
    return;
}

template <typename... Args>
bool create_task(
    task_data_t &task,
    volatile uint8_t *stack_array,
    const uint16_t stack_size,
    const char *name,
    const uint8_t priority,
    const uint8_t slice_ms,
    void (entry)(Args...),
    Args... args)
{
    volatile uint8_t *arg_ptr;
    uint8_t regs_usage = 0;

    if (stack_array == nullptr)
        return 1;

    /* Initialize arg ptr to task's registers */
    arg_ptr = &task.cpu_state.regs[26];

    /* 
     *  Unpack the variadic args using initializer list
     *  This should call process_arg with each given argument
     */
    int dummy[] = {0, (_process_arg(stack_array, stack_size, arg_ptr, regs_usage, args), 0)...}; 
    (void)dummy; // silence unused warning

    return create_task(
        task,
        stack_array,
        stack_size,
        name,
        priority,
        slice_ms,
        (void (*)(void)) entry);
}
#endif // _SCHEDULER_H_