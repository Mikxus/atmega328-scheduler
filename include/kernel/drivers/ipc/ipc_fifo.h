#ifndef _IPC_FIFO_H_
#define _IPC_FIFO_H_

#include <stdlib.h>
#include <kernel/drivers/synchronization/mutex.h>
#include <kernel/drivers/data_types/fifo.h>

template <typename T>
struct ipc_fifo_t
{
    fifo_t<T> fifo;
    mutex_t mtx;

public:
    void init(volatile T* buffer, int size)
    {
        fifo.init(buffer, size);
        mtx_init(&mtx);
        return;
    }

    int get_size()
    {
        int size;
        mtx_lock(&mtx);
        size = fifo.get_size();
        mtx_release(&mtx);
        return size;
    }

    int get_used_size()
    {
        int size;
        mtx_lock(&mtx);
        size = fifo.get_used_size();
        mtx_release(&mtx);
        return size; 
    }

    bool is_full()
    {
        bool value;
        mtx_lock(&mtx);
        value = fifo.is_full();
        mtx_release(&mtx);
        return value;
    }

    bool enqueue(const T &item)
    {
        bool err;
        mtx_lock(&mtx);
        err = fifo.enqueue(item);
        mtx_release(&mtx);
        return err;
    }

    T dequeue()
    {
        T item;
        mtx_lock(&mtx);
        item = fifo.dequeue();
        mtx_release(&mtx);
        return item;
    }

};

#endif // _IPC_FIFO_H_