/**
 * @file sched_slinked_list.h
 *
 * @brief **INTERNAL** intrusive singly linked list with scheduler specific tweaks.
 * 
 * @note Functions are **NOT ATOMIC**. Callee must ensure atomicity if needed.
 *
 * @details Usage:
 * 
 * @code {.c}
 *      struct my_data {
 *          intrusive_slinked_list_node<my_data> list_node;
 *          uint8_t priority;
 *          # own data after this
 *      }
 * 
 *      # List head & accessor: 
 *      slinked_list<my_data, &my_data::list_node, &my_data::priority> my_list;
 * 
 *      # Adding & removing to the list
 *      # where obj is my_data type
 *      my_list.add(&obj);
 *      my_list.remove(&obj); 
 * @endcode
 */
#ifndef _SCHED_SLINKED_LIST_H_
#define _SCHED_SLINKED_LIST_H_
#include <stdint.h>
#include <kernel/errno.h>
#include <kernel/data_types/intrusive_slinked_list.h>

/**
 * @brief Scheduler's list implementation. Implements intrusive_slinked_list.
 */
template<
    typename T,
    intrusive_slinked_list_node<T> T::*node_ptr,
    uint8_t T::*priority_ptr
>
class sched_slinked_list : private intrusive_slinked_list<T, node_ptr>
{
    static uint8_t _get_prio(const T &value) {
        return value.*priority_ptr;
    }

    /**
     * @brief Add new node sorted by priority
     * @details The resulting list will have highest priority at head and lowest
     *          at tail.
     * 
     *          If the list has same priority nodes as new_node it
     *          will be placed behind the last same priority node whose next_ptr
     *          points to either lower priority node or nullptr (list end).
     * 
     * @note New_node must not be already linked
     * @param  new_node: 
     *
     * @retval kernel_errno_t:
     *              - KERNEL_OK
     *              - KERNEL_ERR_NOT_EMPTY   (new_node is linked to other nodes)
     *              - KERNEL_ERR_INVALID_PARAMETER
     */
    kernel_errno_t _add_sorted(T* new_node) {
        uint8_t priority    = _get_prio(*new_node);
        T* seek_ptr         = nullptr;
        T* last_node        = nullptr;

        if (!new_node)
            return KERNEL_ERR_INVALID_PARAMETER;

        if (this->get_next(new_node) != nullptr)
            return KERNEL_ERR_NOT_EMPTY;

        if (!this->head) {
            this->head = new_node;
            return KERNEL_OK;
        }


        /*
         * Case: new_node's priority is higher than first node's.
         * Manually insert new_node at this->head, since
         * this->insert() can only place new node behind node.
         */
        if (priority > _get_prio(*this->head)) {
            intrusive_slinked_list<T, node_ptr>::_set_next(new_node, this->head);
            this->head = new_node;
            return KERNEL_OK;
        }

        bool same_priority = false;

        last_node = this->head;
        seek_ptr = get_next(this->head);

        while (seek_ptr != nullptr) {
            const uint8_t cur_priority = _get_prio(*seek_ptr);

            if (priority > cur_priority) {
                this->insert(last_node, new_node);
                return KERNEL_OK;

            } else if (priority == cur_priority) {
                /* Only place new_node behind same priority nodes */
                same_priority = true;

            } else if (priority != cur_priority) {
                if (same_priority) {
                    /* We are now behind same priority nodes */
                    this->insert(last_node, new_node);
                    return KERNEL_OK;
                }
            }

            last_node = seek_ptr;
            seek_ptr = get_next(seek_ptr);
        }

        this->insert(last_node, new_node);
        return KERNEL_OK;
    }

public:
    /** @note Only expose inherited functions which don't mess up the sorting */
    using intrusive_slinked_list<T, node_ptr>::get_head;
    using intrusive_slinked_list<T, node_ptr>::get_next;
    using intrusive_slinked_list<T, node_ptr>::remove;

    /**
     * @brief Add new node to the list
     * @param  new_node: 
     * @retval kernel_errno_t:
     *              - KERNEL_OK
     *              - KERNEL_ERR_NOT_EMPTY
     *              - KERNEL_ERR_INVALID_PARAMETER
     */
    kernel_errno_t add(T* new_node) {
        if constexpr (CONF_SCHED_PRIORITIES == true) {
            return _add_sorted(new_node);
        } else if (CONF_SCHED_PRIORITIES == false) {
            return intrusive_slinked_list<T, node_ptr>::add_tail(new_node);
        } else {
            static_assert(false, "No valid priority support defined");
        }
    }
};

#endif  // _SCHED_SLINKED_LIST_PRIV_H_