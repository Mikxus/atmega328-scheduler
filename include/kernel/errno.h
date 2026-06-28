#ifndef _KERNEL_ERRNO_H_
#define _KERNEL_ERRNO_H_

#include <avr/pgmspace.h>

typedef enum
{
    // Success
    KERNEL_OK = 0,

    /* Task managment errors */
    TASK_ERR_NAME_TOO_LONG = 1,
    TASK_ERR_STACK_TOO_SMALL = 2,
    TASK_ERR_INVALID_PRIORITY = 3,
    TASK_ERR_INVALID_TIMESLICE = 4,
    TASK_ERR_BLOCKED = 5,

    /* Synchronization errors */
    MUTEX_ERR_LOCKED = 10,
    MUTEX_ERR_RECURSIVE_LOCK = 11,
    MUTEX_ERR_NOT_OWNER = 12,

    /* Data structure errors */

    /* Generic errors */
    KERNEL_ERR_INVALID_PARAMETER = 30,
    KERNEL_ERR_NOT_FOUND = 31,
    KERNEL_ERR_EMPTY = 32,
    KERNEL_ERR_NOT_EMPTY = 33,
    KERNEL_ERR_FULL = 34,
    KERNEL_ERR_YIELD_PEND =35
} kernel_errno_t;

/**
 * @brief Get error's pointer to flash string
 * @note   
 * @param  errno: 
 * @retval 
 */
PGM_P kernel_strerror_P(kernel_errno_t errno);

#endif // _KERNEL_ERRNO_H_