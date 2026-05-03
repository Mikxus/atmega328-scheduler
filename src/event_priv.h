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
 * @retval kernel_errno_t:
 *          KERNEL_ERR_INVALID_PARAMETER 
 */
kernel_errno_t _add_event(event_t* event);

/**
 * @brief Remove event from event_list  
 * @note   
 * @param  &event: 
 * @retval None
 */
kernel_errno_t _remove_event(event_t* event);

#endif _EVENT_PRIV_H_