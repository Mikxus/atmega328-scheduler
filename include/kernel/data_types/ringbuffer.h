/**
 * @brief Simple template ringbuffer implementation.
 * @note Is **not atomic**.  
 */
#ifndef _RINGBUFFER_H_
#define _RINGBUFFER_H_

#include <stdint.h>
#include <kernel/errno.h>

template <typename T>
struct ringbuffer_t
{
private:
    T *buffer       = nullptr;
    uint16_t size    = 0;
    uint16_t count   = 0;
    uint16_t head    = 0;
    uint16_t tail    = 0;

    bool increment_head()
    {
        head = (head + 1) % size;

        if (is_full()) {
            increment_tail();
        } 
        return 0;
    }

    bool increment_tail()
    {
        tail = (tail + 1) % size;
        return 0;
    }

public:
    void init(T *buf, uint16_t n)
    {
        buffer = buf;
        size = n;
        head = 0;
        tail = 0;
    }

    /**
     * @brief Get ringbuffer max size.
     *
     */
    uint16_t get_size()
    {
        return size;
    }

    uint16_t get_used_size()
    {
        return count;
    }

    bool is_full() {
        return count == size;
    }

    /**
     * @brief Add item to ringbuffer.  
     * @note When full, discards the last item.
     *
     * @param  &item: T
     * 
     * @retval KERNEL_OK
     * @retval KERNEL_ERR_FULL
     * @retval KERNEL_ERR_INVALID_PARAMETER
     */
    kernel_errno_t add(const T &item)
    {
        if (buffer == nullptr)
            return KERNEL_ERR_INVALID_PARAMETER;
        
        buffer[head] = item;
        increment_head();

        if (!is_full()) {
            count += 1;
        }

        return KERNEL_OK;
    }

    /**
     * @brief Remove item. 
     * 
     * @param  &output: Output variable to store the removed item. 
     *
     * @retval KERNEL_OK
     * @retval KERNEL_ERR_EMPTY
     */
    kernel_errno_t remove(T &output)
    {
        if (get_used_size() == 0)
            return KERNEL_ERR_EMPTY;
        
        output = buffer[tail];
        increment_tail();
        count -= 1;
        return KERNEL_OK;
    }

    T read(uint16_t idx)
    {
        return buffer[((uint16_t) tail + idx) % size];
    }
};

#endif  // _RINGBUFFER_H_