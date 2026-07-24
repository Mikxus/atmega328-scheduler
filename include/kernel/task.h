#ifndef _TASK_H_
#define _TASK_H_

#include <stdint.h>
#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include <kernel/errno.h>
#include <kernel/drivers/synchronization/atomic.h>
#include <kernel/drivers/data_types/intrusive_slinked_list.h>

struct event_t; 

/**
 * @brief Enum for task's possible states
 */
typedef enum 
{
    UNDEFINED = 0,
    READY,
    RUNNING,
    BLOCKED,
    STOPPED,
    SLEEP
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
    volatile uint8_t *ptr;
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
    intrusive_slinked_list_node<task_data_t> next_node;

    /* Flash ptr to a name */
    PGM_P name;

    volatile task_state_t state;
    volatile task_stack_t stack;
    volatile cpu_registers cpu_state;

    #if SCHEDULER_HAS_PRIORITIES == 1
    uint8_t priority;
    #endif

    #if CONF_TRACK_TASK_CPU_TIME == 1
    volatile uint32_t exec_time_us;
    volatile uint8_t  exec_time_overflow_count;

    /* Start time of current execution slice */
    volatile uint32_t exec_start_time_us;
    #endif
    uint8_t time_slice_ms;
} task_data_t;

/* Currently running task */
extern task_data_t* volatile c_task;

/**
 * @brief Initialize and add new task to the scheduler  
 * @note   
 * @param  &task: 
 * @param  *stack_array: 
 * @param  stack_size: 
 * @param  name:        Flash ptr to name 
 * @param  priority:    task priority (0 - 255)
 * @param  slice_ms:    Timeslice
 * @param  (entry: 
 * @retval 
 */
kernel_errno_t create_task(
    task_data_t &task,
    volatile uint8_t *stack_array,
    const uint16_t stack_size,
    PGM_P name,
    const uint8_t priority,
    const uint8_t slice_ms,
    void (entry)(void)
);

/**
 * @brief Internal function to calculate variadic function's size argument sizes
 * @note   
 * @param  &args_to_stack: 
 * @param  &regs_used: 
 * @param  &stack_used: 
 * @param  arg_size: 
 * @retval None
 */
void _calc_arg_stack_size(
    bool &args_to_stack,
    uint16_t &regs_used,
    uint16_t &stack_used,
    uint16_t arg_size
);

/**
 * @brief Initialize task's registers & stack with arguments.  
 * @details
 *      ABI rules:
 *          Registers: 
 *          - Arguments allocated left to right r25-r8
 *          - All arguments are aligned to start in even-numbered registers
 *          - odd-sized arguments have one free register above them
 *          - If one argument does not fit in registers all of the remaining args
 *            go to stack.
 * 
 *          Stack: 
 *          - odd-sized arguments on stack do not have padding
 *          - Arguments are placed on stack from low -> high address
 * 
 * @param  &task: 
 * @param  &args_to_stack: 
 * @param  &regs_used: 
 * @param  &stack_used: 
 * @param  arg: 
 * @retval None
 */
template <typename T>
void _process_arg(
    task_data_t &task,
    bool &args_to_stack,
    uint16_t &regs_used,
    uint16_t &stack_remaining,
    T arg
) {
    constexpr uint8_t MAX_REG_USAGE = 17;
    constexpr uint8_t START_REG     = 25;
    uint16_t arg_size               = sizeof(T);
    uint8_t padding                 = arg_size % 2;

    if (
        regs_used + arg_size + padding < MAX_REG_USAGE 
        && args_to_stack == false
    ) {
        uint8_t arg_base = START_REG - regs_used - arg_size - padding + 1; 

        memcpy(
            (void*) &task.cpu_state.regs[arg_base],
            &arg,
            arg_size
        );
        regs_used += arg_size + padding;
        /*
        printf("%s: Arg written to R%02u-R%02u bytes %u\n",
            task.name,  arg_base, arg_base + arg_size + padding - 1, arg_size + padding
        );
        */
        return;
    }

    args_to_stack = true;
    uint16_t arg_base = task.stack.size - stack_remaining - 1; 

    /*
    printf("%s: arg written to stack [%02u-%02u] [0x%04x-0x%04x] bytes: %u\n",
        task.name,
        arg_base, arg_base + arg_size,
        (uint16_t) &task.stack.memory_ptr[arg_base],
        (uint16_t) &task.stack.memory_ptr[arg_base + arg_size],
        arg_size
    );
    */
    memcpy((void*) &task.stack.ptr[arg_base],
        &arg,arg_size);

    stack_remaining -= arg_size;
    return;
}

template <typename... Args>
kernel_errno_t create_task(
    task_data_t &task,
    volatile uint8_t *stack_array,
    const uint16_t stack_size,
    PGM_P name,
    const uint8_t priority,
    const uint8_t slice_ms,
    void (entry)(Args...),
    Args... args
) {
    kernel_errno_t errno = KERNEL_OK;
    bool args_to_stack = 0;
    uint16_t regs_used = 0;
    uint16_t stack_used = 0;
    uint16_t stack_base = 0;
    ATOMIC_GUARD();

    errno = create_task(task, stack_array, stack_size,
        name, priority, slice_ms,
        (void (*)(void)) entry);

    if (errno != KERNEL_OK)
        return errno;

    /*
     * We first need to know how many bytes does the arguments occupy in stack.
     * Since we start placing function's left most argument at the lowest 
     * stack address.
     */
    int dummy_size[] = {
        0, 
        (
            _calc_arg_stack_size(
                args_to_stack,
                regs_used,
                stack_used,
                (uint16_t) sizeof(args)
            ),
            0
        )
        ...
    }; 
    (void)dummy_size;

    if (stack_used > stack_size - 1)
        return TASK_ERR_STACK_TOO_SMALL;

    /* 
     * No need to keep track of regs used
     * since they're placed from r25 downwards
     */
    stack_base = stack_used;
    regs_used = 0;
    args_to_stack = 0;

    int dummy[] = {
        0,
        (
            _process_arg(task, args_to_stack, regs_used, stack_used, args),
            0
        )
        ...
    }; 
    (void)dummy;

    /*
     * Prepare stack pointer & set entry point if we have args in stack.
     * Stack should look like this:
     *  [--- Low address ---]:
     *  [task's stack end   ]
     *  [       empty       ] <-- task's sp points to here 
     *  [       PC L        ]
     *  [       PC H        ]  
     *  [left-most func arg ]
     *  [      ||||||       ]
     *  [right-most func arg] <-- *which doesn't fit in registers
     *  [task's stack end   ]
     *  [---High address ---]
     *  High address:
     */
    if (stack_base != 0) {
        uint16_t sp_index = task.stack.size - stack_base - 6;
        task.cpu_state.sp = (uint16_t) &task.stack.ptr[sp_index];
        task.stack.ptr[sp_index + 2] = (uint8_t) ((uint16_t) entry & 0xFF);          // pc l 
        task.stack.ptr[sp_index + 1] = (uint8_t) (((uint16_t) entry >> 8) & 0xFF);   // pc h
        /*
        printf("SP:  0x%04x\n", task.cpu_state.sp);
        printf("ENTRY:  0x%04x\n", (uint16_t) entry);
        printf("ENTRY L:  0x%04x is: 0x%04x\n", task.cpu_state.sp +2,  *((uint8_t*) task.cpu_state.sp+2));
        printf("ENTRY h:  0x%04x is: 0x%04x\n\n", task.cpu_state.sp +1,  *((uint8_t*) task.cpu_state.sp+1));
        */
    }

    return KERNEL_OK;
}


/**
 * @brief Returns the task currently running  
 * @note   
 * @retval 
 */
task_data_t* get_current_task();

/**
 * @brief Get task's stack size in bytes
 * 
 * @param task 
 * @return 
 */
uint16_t get_task_stack_size(task_data_t* task);

/**
 * @brief get task's stack usage in bytes
 * @param task 
 * @return 
 */
uint16_t get_task_stack_usage(task_data_t* task);

/**
 * @brief Remove task from the scheduler  
 * @note  The task will continue running until it's tick ends
 * @param  *task: 
 * @retval      KERNEL_ERR_NOT_FOUND
 */
kernel_errno_t remove_task(task_data_t *task);

/**
 * @brief Get task's pc  
 * @note    - pc is only updated on context switch & points to 
 *              the next instruction to be ran when it's scheduled again
 *          - Returns 0 if task == get_current_task()
 *  
 * @param  *task: 
 * @retval  pc value
 */
uint16_t get_task_pc(task_data_t *task);

/**
 * @brief Stop task's execution  
 * @note   
 * @param  *task: 
 * @retval      KERNEL_ERR_INVALID_PARAMETER
 *              TASK_ERR_BLOCKED
 *              KERNEL_OK
 */
kernel_errno_t suspend_task(task_data_t *task);

/**
 * @brief Resume task's execution 
 * @note   
 * @param  *task: 
 * @retval      KERNEL_ERR_INVALID_PARAMETER:
 *              KERNEL_OK 
 */
kernel_errno_t resume_task(task_data_t *task);

#endif // _TASK_H_