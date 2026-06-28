#include <kernel/errno.h>

PGM_P kernel_strerror_P(kernel_errno_t errno)
{
    switch (errno)
    {
    case KERNEL_OK:
        return PSTR("Kernel ok");
    case TASK_ERR_NAME_TOO_LONG:
        return PSTR("Task err name too long");
    case TASK_ERR_STACK_TOO_SMALL:
        return PSTR("Task err stack too small");
    case TASK_ERR_INVALID_PRIORITY:
        return PSTR("Task err invalid priority");
    case TASK_ERR_INVALID_TIMESLICE:
        return PSTR("Task err invalid timeslice");
    case TASK_ERR_BLOCKED:
        return PSTR("Task err blocked");
    case MUTEX_ERR_LOCKED:
        return PSTR("Mutex err locked");
    case MUTEX_ERR_RECURSIVE_LOCK:
        return PSTR("Mutex err recursive lock");
    case MUTEX_ERR_NOT_OWNER:
        return PSTR("Mutex err not owner");
    case KERNEL_ERR_INVALID_PARAMETER:
        return PSTR("Kernel err invalid parameter");
    case KERNEL_ERR_NOT_FOUND:
        return PSTR("Kernel err not found");
    case KERNEL_ERR_EMPTY:
        return PSTR("Kernel err empty");
    case KERNEL_ERR_NOT_EMPTY:
        return PSTR("Kernel err not empty");
    case KERNEL_ERR_FULL:
        return PSTR("Kernel err full");
    case KERNEL_ERR_YIELD_PEND:
        return PSTR("Kernel err yield pend");
    default:
        return PSTR("Unknown error");
    }
}