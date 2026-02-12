#ifndef _TASK_H_
#define _TASK_H_

#include <stdint.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <kernel/drivers/synchronization/atomic.h>
#include <kernel/drivers/scheduling/sched.h>

typedef struct task_data_t task_data_t;

/* Currently running task */
extern task_data_t* volatile c_task;

/**
 * @brief Enum for task's possible states
 */
typedef enum 
{
    READY,
    RUNNING,
    BLOCKED,
    SLEEP,
    UNDEFINED
} task_state_t;

/**
 * @brief Atmega328p's cpu registers' state for context switching
*/
struct __attribute__((packed)) cpu_registers
{
    uint8_t regs[32]; // r0 - r31
    uint16_t sp;     // stack pointer
    uint8_t sreg;    // status register
};

/**
 * @brief Task memory data structure
 */
typedef struct
{
    volatile uint8_t *memory_ptr;
    uint16_t size;
} task_stack_t;

/**
 * @brief Task data structure
 * @note: No initializers, or methods
 *      This structure must be standard layout for offsetof to work correctly
 */
typedef struct task_data_t
{
    /* singly linked list node for the current task */
    task_data_t* volatile next_node;

    char name[CONF_TASK_NAME_MAX_LENGTH];

    volatile task_state_t state;
    volatile task_stack_t stack;
    volatile cpu_registers cpu_state;

    #if SCHEDULER_HAS_PRIORITIES == 1
    uint8_t priority;
    #endif

    #if CONF_TRACK_TASK_CPU_TIME == 1
    /*
     * Task execution time represented in 38 bits
     * Overflows in ~50.9 days
     */
    volatile uint32_t exec_time_us;
    volatile uint8_t  exec_time_overflow_count;

    /* Start time of current execution slice */
    volatile uint32_t exec_start_time_us;
    #endif
    uint8_t time_slice_ms;
};

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


template<typename T>
void _calc_sizes(
    task_data_t &task,
    bool &args_to_stack,
    uint16_t &regs_used,
    uint16_t &stack_used,
    T arg)
{
    uint16_t arg_size = sizeof(T);
    bool padding = arg_size % 2;

    /* if arg fits in registers */
    if (regs_used + arg_size + padding < 17 
        && args_to_stack == 0) {
        regs_used += arg_size + padding;
        return;
    }

    /* rest goes to stack & everything after it */
    args_to_stack = 1;
    stack_used += arg_size;
    return;
}

/* 
 * Setup register & stack for Args
 * Rules: 
 *    - Arguments allocated left to right r25-r8
 *    - All arguments are aligned to start in even-numbered registers
 *    - odd-sized arguments have one free register above them
 * 
 *    - once one argument is written to stack, all following args go to stack
 *    - arguments written to stack are not aligned to two byte boundary
 *    - odd-sized arguments on stack do not have padding
 *    - Arguments are placed on stack from low -> high address
 */
template <typename T>
void _process_arg(
    task_data_t &task,
    bool &args_to_stack,
    uint16_t &regs_used,
    uint16_t &stack_used,
    T arg)
{
    uint16_t arg_size = sizeof(T);
    bool padding = arg_size % 2;

    /* Determine if arg is placed in registers */
    if (regs_used + arg_size + padding < 17 && args_to_stack == 0) {
        uint8_t arg_base = 25 - regs_used - arg_size - padding + 1; 
        memcpy((void*) &task.cpu_state.regs[arg_base],
            &arg, arg_size);

        regs_used += arg_size + padding;
        /*
        printf("%s: Arg written to R%02u-R%02u bytes %u\n",
            task.name,  arg_base, arg_base + arg_size + padding - 1, arg_size + padding);
        */
        return;
    }

    /* If one arg doesn't fit in registers, rest goes to stack */
    args_to_stack = 1;
    uint16_t arg_base = task.stack.size - stack_used - 1; 

    /*
    printf("%s: arg written to stack [%02u-%02u] [0x%04x-0x%04x] bytes: %u\n",
        task.name,
        arg_base, arg_base + arg_size,
        (uint16_t) &task.stack.memory_ptr[arg_base],
        (uint16_t) &task.stack.memory_ptr[arg_base + arg_size],
        arg_size);
    */
    memcpy((void*) &task.stack.memory_ptr[arg_base],
        &arg,arg_size);

    stack_used -= arg_size;
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
    bool args_to_stack = 0;
    uint16_t regs_used = 0;
    uint16_t stack_used = 0;
    uint16_t stack_base = 0;
    ATOMIC_GUARD();

    if (create_task(
        task, stack_array, stack_size,
        name, priority, slice_ms,
        (void (*)(void)) entry)) {
        return 1;
    }

    /*
     * We need first calculate sizes of how much stack & registers are needed
     * since we have to place the left most stack arguments at the lowest stack address
     */
    int dummy_size[] = {0, (_calc_sizes(task, args_to_stack, regs_used, stack_used, args), 0)...}; 
    (void)dummy_size; // silence unused warning
    /* No need to keep track of regs used
     * since they're placed from r25 downwards
     */
    stack_base = stack_used;
    regs_used = 0;
    args_to_stack = 0;

    /*
     *  Unpack the variadic args using initializer list
     *  This should call process_arg with each given argument
     */
    int dummy[] = {0, (_process_arg(task, args_to_stack, regs_used, stack_used, args), 0)...}; 
    (void)dummy; // silence unused warning

    /*
     * Prepare stack pointer & set entry point 
     * Stack should look like this: (without the PC H & L)
     * * Low address:
     *  [PC H] <-- task.cpu_state.sp should point to this
     *  [PC L]  <-- Gets popped the first time task is run by the ISR
     *  [function args which don't fit in regs]
     *  [---stack high---] end of task's stack
     *  High address:
     */
    if (stack_base != 0) {
        uint16_t sp_index = task.stack.size - stack_base - 6;
        task.cpu_state.sp = (uint16_t) &task.stack.memory_ptr[sp_index];
        task.stack.memory_ptr[sp_index + 2] = (uint8_t) ((uint16_t) entry & 0xFF);          // pc l 
        task.stack.memory_ptr[sp_index + 1] = (uint8_t) (((uint16_t) entry >> 8) & 0xFF);   // pc h
        /*
        printf("SP:  0x%04x\n", task.cpu_state.sp);
        printf("ENTRY:  0x%04x\n", (uint16_t) entry);
        printf("ENTRY L:  0x%04x is: 0x%04x\n", task.cpu_state.sp +2,  *((uint8_t*) task.cpu_state.sp+2));
        printf("ENTRY h:  0x%04x is: 0x%04x\n\n", task.cpu_state.sp +1,  *((uint8_t*) task.cpu_state.sp+1));
        */
    }

    return 0;
}


/**
 * @brief Returns the task currently running  
 * @note   
 * @retval 
 */
task_data_t* get_current_task();

/**
 * @brief Remove task from the scheduler  
 * @note  The task will continue running until it's tick ends
 * @param  *task: 
 * @retval
 */
bool remove_task(task_data_t *task = c_task);

#endif