#ifndef _IPC_FIFO_H_
#define _IPC_FIFO_H_

#include <stdlib.h>
#include "src/drivers/synchronization/mutex.h"
#include "src/drivers/data_types/fifo.h"

template <typename T>
struct ipc_fifo_t  
{
    volatile fifo_t<T> fifo;
    mutex_t mtx;
};

void ipc_fifo_init(ipc_fifo_t *ptr, mutex_t *mtx)

#endif // _IPC_FIFO_H_