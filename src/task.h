#ifndef _TASK_H_
#define _TASK_H_

#include <stdint.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "cpu.h"
#include "lib/singly_linked_list/singly_linked_list.h"

typedef struct task_data_t task_data_t;

/* Currently running task */
extern task_data_t volatile *c_task;

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
    task_data_t volatile *next_node;

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

/**
 * @brief Create task & initialize it
 * 
 * @param task 
 * @param name 
 * @param priority 
 * @param slice_ms 
 * @param entry 
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
    const void *(entry)(void));

/**
 * @brief get task list's head
 * 
 * @return task_data_t* 
 */
task_data_t volatile *_get_head_task();

/**
 * @brief get task list's tail
 * 
 * @return task_data_t* 
 */
task_data_t volatile *_get_tail();

/**
 * @brief Get next task 
 * 
 * @param task 
 * @return task_data_t* 
 */
task_data_t volatile *_get_next_task(
    task_data_t volatile *task);

/**
 * @brief Find preceding task
 *      Returns the preceding task which has ptr->next_node as target task
 * @param target_task 
 * @return task_data_t* 
 */
task_data_t volatile *_find_preceding_task(
    task_data_t volatile* target_task);

/**
 * @brief Find task
 *      Returns ptr to the task
 * @param target_task 
 * @return task_data_t* 
 */
task_data_t volatile *_fing_task(
    task_data_t volatile *target_task);

void _add_task(task_data_t volatile *new_node);

#endif