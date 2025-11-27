#ifndef _TASK_H_
#define _TASK_H_

#include <stdint.h>
#include "lib/singly_linked_list/singly_linked_list.h"
#include "cpu.h"


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
 * @brief Task memory data structure
 */
typedef struct
{
    uint8_t * memory_ptr;
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
    sl_list::node<task_data_t> task_node;

    char name[SCHEDULER_TASK_NAME_MAX_LENGTH];
    uint8_t priority;

    volatile task_state_t state;
    volatile task_stack_t stack;
    volatile cpu_registers cpu_state;

    #if SCHEDULER_TASK_TRACK_STATISTICS == 1
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

#endif