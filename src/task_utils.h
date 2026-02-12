/** 
 * @file task_utils.h
 * @brief PRIVATE - internal task managment utilities
 */
#ifndef _TASK_UTILS_H_
#define _TASK_UTILS_H_

#include <kernel/task.h>
#include <kernel/errno.h>

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
 * @brief get task list's head
 * 
 * @return task_data_t* 
 */
task_data_t* _get_ready_list_head();

/**
 * @brief get list's last node
 * 
 * @return task_data_t* 
 */
task_data_t* _get_tail(task_data_t* list_head);

/**
 * @brief Get next task 
 * 
 * @param task 
 * @return task_data_t* 
 */
task_data_t* _get_next_task(
    task_data_t* task);

/**
 * @brief Find preceding task
 *      Returns the preceding task which has ptr->next_node as target task
 * @param target_task
 * @param list_head list's head node
 * @return task_data_t* 
 */
task_data_t* _find_preceding_task(
    task_data_t* target_task,
    task_data_t* list_head);

/**
 * @brief Find task
 *      Returns ptr to the task
 * @param target_task 
 * @param list_head list's head node
 * @return task_data_t* 
 */
task_data_t* _find_task(
    task_data_t* target_task,
    task_data_t* list_head);

/**
 * @brief Add task to the task list  
 * @param new_node: 
 * @param list_head list's head node
 * @retval None
 */
void _add_task(task_data_t* new_node,
    task_data_t* list_head);

/**
 * @brief Remove task from specified list  
 * @param  *task: 
 * @retval KERNEL_OK:
 *         KERNEL_INVALID_PARAMETER: invalid input
 */
kernel_errno_t _remove_task_from_list(
    task_data_t* task,
    task_data_t* list_head);

/**
 * @brief Set task state
 * 
 * @param task 
 * @param state 
 */
void _set_task_state(
    task_data_t *task,
    task_state_t state);

#endif // _TASK_UTILS_H_