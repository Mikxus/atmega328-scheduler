/** 
 * @file task_utils.h
 * @brief PRIVATE - internal task managment utilities
 */
#ifndef _TASK_UTILS_H_
#define _TASK_UTILS_H_

#include <kernel/task.h>
#include <kernel/errno.h>
#include "drivers/scheduling/sched.h"

/**
 * @brief get task's stack usage in bytes
 * @param task 
 * @return uintptr_t 
 */
uint16_t _get_task_stack_usage(
    task_data_t* task);

/**
 * @brief Get task's stack size in bytes
 * 
 * @param task 
 * @return uintptr_t 
 */
uint16_t _get_task_stack_size(
    task_data_t* task);

/**
 * @brief Set task state
 * 
 * @param task 
 * @param state 
 */
void _set_task_state(
    task_data_t *task,
    task_state_t state);

/**
 * @brief Set task to given priority  
 * @note   
 * @param  task: 
 * @retval None
 */
void _set_task_priority(
    task_data_t* task,
    uint8_t priority);

/**
 * @brief Get task's priority  
 * @note   
 * @param  task: 
 * @retval 
 */
uint8_t _get_task_priority(task_data_t* task);

/**
 * @brief get task list's head
 * 
 * @return task_data_t* 
 */
task_data_t* _get_ready_list_head();

#endif // _TASK_UTILS_H_