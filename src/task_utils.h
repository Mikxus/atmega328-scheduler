#ifndef _TASK_UTILS_H_
#define _TASK_UTILS_H_

#include <kernel/task.h>

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

#endif // _TASK_UTILS_H_