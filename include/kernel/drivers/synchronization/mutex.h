/**
 * @file mutex.h 
 * @brief  Non recursive mutex with priority inheritance
 * @note   
 */
#ifndef _MUTEX_H_
#define _MUTEX_H_

#include <avr/interrupt.h>
#include <kernel/task.h>
#include <kernel/event.h>
#include <kernel/errno.h>
#include <kernel/drivers/synchronization/atomic.h>
#include <kernel/drivers/data_types/sorted_fifo_ptr.h>
#include <kernel/kernel.h>

typedef struct
{
    task_data_t* owner;
    event_t mtx_event;

    #if SCHEDULER_HAS_PRIORITIES == 1
    uint8_t priority;
    sorted_fifo_ptr_t<task_data_t, uint8_t, &task_data_t::priority> fifo;
    #endif
} mutex_t; 

static_assert(sizeof(((task_data_t*)0)->priority) == sizeof(((mutex_t*)0)->priority),
                "inherited_priority must have the same size as task_data_t->priority");

/**
 * @brief Initializes mutex 
 * 
 * @param mtx 
 */
void mtx_init(mutex_t *mtx);

/**
 * @brief Blocks until mutex is acquired
 * 
 * @retval KERNEL_OK: mutex acquired
 *         MUTEX_ERR_RECURSIVE_LOCK: Mutex already locked by the task
 */
kernel_errno_t mtx_lock(mutex_t *mtx);

/**
 * @brief Tries to acquire mutex without blocking  
 * @note   
 * @param  *mtx: 
 * @retval  KERNEL_OK: Acquired successfully
 *          MUTEX_ERR_LOCKED: Mutex already locked by another task
 */
kernel_errno_t mtx_try_lock(mutex_t *mtx);

/**
 * @brief Releases mutex  
 * @note   
 * @param  *mtx: 
 * @retval KERNEL_OK: Released successfully
 *         MUTEX_ERR_NOT_OWNER: Current task is not the owner of the mutex
 */
kernel_errno_t mtx_release(mutex_t *mtx);

#endif // _MUTEX_H_