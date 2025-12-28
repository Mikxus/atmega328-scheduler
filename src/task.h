#ifndef _TASK_H_
#define _TASK_H_

#include <stdint.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "cpu.h"
#include "src/drivers/scheduling/sched.h"

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
 * @brief get task's stack usage in bytes
 * 
 * @param task 
 * @return uintptr_t 
 */
uint16_t _get_task_stack_usage(
    task_data_t *task);

/**
 * @brief Get task's stack size in bytes
 * 
 * @param task 
 * @return uintptr_t 
 */
uint16_t _get_task_stack_size(
    task_data_t *task);

/**
 * @brief get task list's head
 * 
 * @return task_data_t* 
 */
task_data_t *_get_head_task();

/**
 * @brief get task list's tail
 * 
 * @return task_data_t* 
 */
task_data_t *_get_tail();

/**
 * @brief Get next task 
 * 
 * @param task 
 * @return task_data_t* 
 */
task_data_t *_get_next_task(
    task_data_t *task);

/**
 * @brief Find preceding task
 *      Returns the preceding task which has ptr->next_node as target task
 * @param target_task 
 * @return task_data_t* 
 */
task_data_t  *_find_preceding_task(
    task_data_t * target_task);

/**
 * @brief Find task
 *      Returns ptr to the task
 * @param target_task 
 * @return task_data_t* 
 */
task_data_t *_fing_task(
    task_data_t *target_task);

/**
 * @brief Add task to the task list  
 * @param  *new_node: 
 * @retval None
 */
void _add_task(task_data_t *new_node);

/**
 * @brief Remove task from the ready list  
 * @param  *task: 
 * @retval bool: 0 success, 1 failure
 */
bool _remove_task_from_ready_list(task_data_t *task);

/**
 * @brief Set task state
 * 
 * @param task 
 * @param state 
 */
void _set_task_state(
    task_data_t *task,
    task_state_t state);

#endif