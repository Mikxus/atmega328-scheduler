#include "mutex.h"

void mutex_init(mutex_t *mtx)
{
    mtx->owner = nullptr;
    mtx->blocked_tasks_head = nullptr;
}

void mutex_acquire(mutex_t *mtx)
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

mutex_errno_t mutex_try_acquire(mutex_t *mtx)
{
    uint8_t sreg = SREG;
    cli();

    if (c_task != mtx->owner) {
        SREG = sreg;
        return MUTEX_ERR_NOT_OWNER;
    }
}

mutex_errno_t mutex_release(mutex_t *mtx)
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