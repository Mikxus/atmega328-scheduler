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
    task_data_t *owner;
    task_data_t *blocked_tasks_head;
} mutex_t; 

/**
 * @brief Initializes mutex 
 * 
 * @param mtx 
 */
void mutex_init(mutex_t *mtx);

/**
 * @brief Blocks until mutex is acquired
 * 
 * @param mtx 
 */
void mutex_acquire(mutex_t *mtx);

mutex_errno_t mutex_try_acquire(mutex_t *mtx);

mutex_errno_t mutex_release(mutex_t *mtx);

#endif _MUTEX_H_