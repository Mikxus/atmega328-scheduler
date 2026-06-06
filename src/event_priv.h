/**
 * @file event_priv.h  
 * @brief   Internal event list handling functions
 * @note    Functions are not atomic. Callee must ensure atomicity
 * 
 */
#ifndef _EVENT_PRIV_H_
#define _EVENT_PRIV_H_

#include <kernel/event.h>
#include <kernel/errno.h>
#include <kernel/drivers/synchronization/atomic.h>
#include "drivers/scheduling/sched.h"

/**
 * @brief Add event to event_list  
 * @note   
 * @param  event: 
 * @retval  KERNEL_OK 
 *          KERNEL_ERR_INVALID_PARAMETER 
 */
kernel_errno_t _add_event(event_t* event);

/**
 * @brief Remove event from event_list  
 * @note   
 * @param  *event: 
 * @return  KERNEL_OK
 *          KERNEL_ERR_INVALID_PARAMETER
 *          KERNEL_ERR_NOT_FOUND
 *          KERNEL_ERR_NOT_EMPTY
 */
kernel_errno_t _remove_event(event_t* event);

/**
 * @brief Is event empty
 * @note   Tells if there are tasks blocked on that event
 * @param  event:
 * @retval 1: empty 0: not empty
 */
bool _is_event_empty(const event_t* event);

/**
 * @brief Block task & add it to event  
 * @note   
 * @param  task: 
 * @param  event: 
 * @retval 
 */
kernel_errno_t _event_block_task(task_data_t* task, event_t* event);

/**
 * @brief Unblock the first blocked task  
 * @note   
 * @param  event: 
 * @retval success: ptr to unblocked task
 *         fail: nullptr
 */
task_data_t* _event_unblock_first(event_t* event);

/**
 * @brief Unblock the task with highest priority  
 * @note  With same priority tasks the task which is blocked first is unblocked
 * @param  event: 
 * @retval success: ptr to unblocked task
 *         fail: nullptr
 */
task_data_t* _event_unblock_highest_prio(event_t* event);

#endif // _EVENT_PRIV_H_