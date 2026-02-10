#include "mutex.h"

void mtx_init(mutex_t *mtx)
{
    mtx->owner = nullptr;
    mtx->blocked_head = nullptr;
}

void mtx_lock(mutex_t *mtx)
{
    while (1)
    {
        ATOMIC_BLOCK() {
            if (mtx->owner == nullptr || mtx->owner == c_task) {
                mtx->owner = c_task;
                return;
            }
        }
        yield();
    }
}

mutex_errno_t mtx_try_lock(mutex_t *mtx)
{
    ATOMIC_GUARD();
    if (mtx->owner != nullptr) {
        return MUTEX_ERR_ALREADY_LOCKED;
    }

    mtx->owner = c_task;
    return MUTEX_OK;
}

mutex_errno_t mtx_release(mutex_t *mtx)
{
    ATOMIC_GUARD();
    if (c_task != mtx->owner) {
        return MUTEX_ERR_NOT_OWNER;
    }

    mtx->owner = nullptr;
    return MUTEX_OK;
}