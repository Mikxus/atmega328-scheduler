/**
 * @file sched_delta_list.h
 *
 * @brief **INTERNAL** singly linked timer tick delta list.
 * 
 * @note Functions are **NOT ATOMIC**. Callee must ensure atomicity if needed.
 *
 * @details Each list node constains tick delta to previous node.
 *          Sleep then decrements the head node by one until it's timeout 
 *          reaches zero and the task gets rescheduled.
 * 
 */
#ifndef _SCHED_DELTA_LIST_H_
#define _SCHED_DELTA_LIST_H_
#include <stdint.h>
#include <kernel/errno.h>
#include <kernel/data_types/intrusive_slinked_list.h>

/**
 * @brief Scheduler's list implementation.
 */
template<
    typename T,
    intrusive_slinked_list_node<T> T::*node_ptr,
    uint16_t T::*sleep_tick_ptr
>
class sched_delta_list : private intrusive_slinked_list<T, node_ptr>
{
    static uint16_t _get_tick(const T &task) {
        return task.*sleep_tick_ptr;
    }

    static void _set_tick(T &task, uint16_t new_ticks) {
        task.*sleep_tick_ptr = new_ticks;
    }

public:
    /** @note Only expose inherited functions which don't mess up the sorting */
    using intrusive_slinked_list<T, node_ptr>::get_head;
    using intrusive_slinked_list<T, node_ptr>::get_next;

    /**
     * @brief Add new node to delta list.
     * 
     * @details 
     *          - The resulting list will have next expiring task at head.
     *          - If the new node has same expiration as node in the list the new 
     *          node will be placed foward of the pre existing node.
     * 
     * @note    
     *          - New node must have it's sleep_tick_ptr populated with the 
     *            wanted timeout in milliseconds.
     *          - new_node must not be already linked.
     *          - LIFO ordering when same timeout. (TODO: fix ordering to FIFO)
     * 
     * @param  new_node: Pointer to new node. 
     *
     * @retval KERNEL_OK
     * @retval KERNEL_ERR_NOT_EMPTY         (new_node is linked to other nodes)
     * @retval KERNEL_ERR_INVALID_PARAMETER (Nullptr as parameter)
     */
    kernel_errno_t add(T* new_node) {
        using base_list = intrusive_slinked_list<T, node_ptr>;
        uint16_t ticks  = _get_tick(*new_node);
        T* seek_ptr     = nullptr;
        T* last_node    = nullptr;

        if (!new_node)
            return KERNEL_ERR_INVALID_PARAMETER;

        if (this->get_next(new_node) != nullptr)
            return KERNEL_ERR_NOT_EMPTY;

        if (this->head == nullptr) {
            this->head = new_node;
            return KERNEL_OK;
        }

        /*
         * Case: new_node's sleep time is smaller than first node's.
         * Manually insert new_node at this->head, since
         * this->insert() can only place new node behind node.
         */
        if (ticks <= _get_tick(*this->head)) {
            base_list::_set_next(new_node, this->head);
            _set_tick(*this->head, _get_tick(*this->head) - ticks);
            this->head = new_node;
            return KERNEL_OK;
        }

        last_node = this->head;
        seek_ptr  = get_next(this->head);
        ticks    -= _get_tick(*this->head);
        while (seek_ptr != nullptr) {
            const uint16_t cur_ticks = _get_tick(*seek_ptr);

            if (ticks <= cur_ticks) {
                this->insert(last_node, new_node);
                _set_tick(*seek_ptr, cur_ticks - ticks);
                _set_tick(*new_node, ticks);
                return KERNEL_OK;
            }

            ticks -= _get_tick(*seek_ptr);
            last_node = seek_ptr;
            seek_ptr = get_next(seek_ptr);
        }

        this->insert(last_node, new_node);
        _set_tick(*new_node, ticks);
        return KERNEL_OK;
    }

    /**
     * @brief  Remove node from delta list.
     * 
     * @param  node: Pointer to node.
     * 
     * @retval KERNEL_OK
     * @retval KERNEL_ERR_EMPTY                 (Empty list)
     * @retval KERNEL_ERR_NOT_FOUND             (Not found)
     * @retval KERNEL_ERR_INVALID_PARAMETER     (Nullptr as parameter)
     */
    kernel_errno_t remove(T* node) {
        using base_list = intrusive_slinked_list<T, node_ptr>;
        T * prev_node   = nullptr; 
        uint16_t ticks  = 0;

        if (node == nullptr) {
            return KERNEL_ERR_INVALID_PARAMETER;
        }

        if (this->head == nullptr) {
            return KERNEL_ERR_EMPTY;
        }

        ticks = _get_tick(*node);

        if (this->head == node) {
            this->head = get_next(this->head);

            if (this->head != nullptr) {
                /* Add remaining ticks to next node */
                _set_tick(*this->head, _get_tick(*this->head) + ticks);
            }

            base_list::_set_next(node, nullptr);
            return KERNEL_OK;
        }

        prev_node = this->head; 
        while (get_next(prev_node) != nullptr && get_next(prev_node) != node) {
            prev_node = get_next(prev_node);
        }

        if (get_next(prev_node) != node) {
            return KERNEL_ERR_NOT_FOUND;
        }

        if (get_next(node) != nullptr) {
            T * next_node = get_next(node);
            _set_tick(*next_node, _get_tick(*next_node) + ticks);
        }

        base_list::_set_next(prev_node, get_next(node));
        base_list::_set_next(node, nullptr);
        return KERNEL_OK;
    }
};

#endif  // _SCHED_DELTA_LIST_H_