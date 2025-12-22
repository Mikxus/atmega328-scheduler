#ifndef _MUTEX_H_
#define _MUTEX_H_

#include <avr/interrupt.h>
#include "src/task.h"
#include "src/drivers/scheduling/sched.h"
#include "src/scheduler.h"

typedef enum
{
    MUTEX_OK = 0,
    MUTEX_FREE,
    MUTEX_LOCKED,
    MUTEX_ERR_ALREADY_LOCKED,
    MUTEX_ERR_NOT_OWNER,
} mutex_errno_t;

typedef struct
{
    task_data_t* volatile owner;
    task_data_t* volatile blocked_tasks_head;
} mutex_t; 

/**
 * @brief Initializes mutex 
 * 
 * @param mtx 
 */
void mtx_init(mutex_t *mtx);

/**
 * @brief Blocks until mutex is acquired
 * 
 * @param mtx 
 */
void mtx_lock(mutex_t *mtx);

/**
 * @brief Tries to acquire mutex without blocking  
 * @note   
 * @param  *mtx: 
 * @retval  MUTEX_OK: Acquired successfully
 *          MUTEX_ERR_ALREADY_LOCKED: Mutex already locked by another task
 */
mutex_errno_t mtx_try_lock(mutex_t *mtx);

/**
 * @brief Releases mutex  
 * @note   
 * @param  *mtx: 
 * @retval MUTEX_OK: Released successfully
 *         MUTEX_ERR_NOT_OWNER: Current task is not the owner of the mutex
 */
mutex_errno_t mtx_release(mutex_t *mtx);

#endif // _MUTEX_H_