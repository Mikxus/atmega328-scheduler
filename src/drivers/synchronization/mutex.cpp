#include "mutex.h"

void mtx_init(mutex_t *mtx)
{
    mtx->owner = nullptr;
    mtx->blocked_tasks_head = nullptr;
}

void mtx_lock(mutex_t *mtx)
{
    uint8_t sreg = SREG;
    cli();

    if (mtx->owner == c_task) {
        // already locked by our task
        SREG = sreg;
        return;
    }

    while (mtx->owner != nullptr) {
        yield();
        cli();
    }

    mtx->owner = c_task;
    SREG = sreg;
}

mutex_errno_t mtx_try_lock(mutex_t *mtx)
{
    uint8_t sreg = SREG;
    cli();

    if (mtx->owner != nullptr) {
        SREG = sreg;
        return MUTEX_ERR_ALREADY_LOCKED;
    }

    mtx->owner = c_task;
    SREG = sreg;
    return MUTEX_OK;
}

mutex_errno_t mtx_release(mutex_t *mtx)
{
    uint8_t sreg = SREG;
    cli();

    if (c_task != mtx->owner) {
        SREG = sreg;
        return MUTEX_ERR_NOT_OWNER;
    }

    mtx->owner = nullptr;
    SREG = sreg;
    return MUTEX_OK;
}