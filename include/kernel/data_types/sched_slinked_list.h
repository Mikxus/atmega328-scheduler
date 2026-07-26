/**
 * @file sorted_slinked_list.h
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
 *      my_list.add_sorted(&obj);
 *      my_list.remove(&obj); 
 * @endcode
 */
#ifndef _SCHED_SLINKED_LIST_H_
#define _SCHED_SLINKED_LIST_H_
#include <stdint.h>
#include <kernel/errno.h>
#include <kernel/data_types/intrusive_slinked_list.h>

template<
    typename T,
    intrusive_slinked_list_node<T> T::*node_ptr,
    uint8_t T::*priority_ptr
>
class sorted_slinked_list : private intrusive_slinked_list<T, node_ptr>
{
    static uint8_t _get_prio(const T &value) {
        return value.*priority_ptr;
    }

public:
    /* @note Only expose inherited functions which don't mess up the sorting */

    /**
     * @brief Get the head node
     * 
     * @return T* or nullptr if no head
     */
    T* get_head() const {
        return intrusive_slinked_list<T, node_ptr>::get_head();
    }

    /**
     * @brief Get the next node
     * 
     * @param node 
     * @return T* or nullptr if no next
     */
    T* get_next(T* node) const {
        return intrusive_slinked_list<T, node_ptr>::get_next(node);
    }

    /**
     * @brief Remove node from the list
     * 
     * @param node 
     * @return kernel_errno_t:
     *          KERNEL_ERR_INVALID_PARAMETER
     *          KERNEL_ERR_NOT_FOUND
     *          KERNEL_OK
     */
    kernel_errno_t remove(T* node) {
        return intrusive_slinked_list<T, node_ptr>::remove(node);
    }

    /**
     * @brief Add new node sorted by priority
     * @details The resulting list will have highest priority at head and lowest
     *          at tail.
     * 
     *          If the list has same priority nodes as new_node new_node
     *          will be placed behind the last same priority node whose next_ptr
     *          points to either lower priority node or nullptr (list end).
     * 
     * @note New_node must not be already linked
     * @param  new_node: 
     * @param  skip_node: Node where to start searching (@note **MUST BE SAME PRIORITY AS new_node**)
     *                     if the priority is smaller at skip_node
     *                      the search will fall back to starting at head
     * @retval kernel_errno_t:
     *              - KERNEL_OK
     *              - KERNEL_ERR_NOT_EMPTY   (new_node is linked to other nodes)
     *              - KERNEL_ERR_INVALID_PARAMETER
     */
    kernel_errno_t add_sorted(T* new_node, T* skip_node=nullptr) {
        uint8_t priority    = 0;
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

        priority = _get_prio(*new_node);

        /*
         * Case: new_node's priority is higher than first node's
         * Manually insert new_node at this->head, since
         * this->insert() can only place new node behind node.
         */
        if (priority > _get_prio(*this->head)) {
            intrusive_slinked_list<T, node_ptr>::_set_next(new_node, this->head);
            intrusive_slinked_list<T, node_ptr>::head = new_node;
            return KERNEL_OK;
        }

        bool same_priority = false;

        if (skip_node != nullptr) {
            if (_get_prio(*this->head) > _get_prio(*skip_node)) {
                last_node   = this->head;
                seek_ptr    = get_next(this->head);
            } else {
                last_node   = skip_node;
                seek_ptr    = get_next(skip_node);
            }
        } else {
            last_node = this->head;
            seek_ptr = get_next(this->head);
        }

        while (seek_ptr != nullptr) {
            const uint8_t cur_priority = _get_prio(*seek_ptr);


            if (priority > cur_priority) {
                this->insert(last_node, new_node);
                goto exit;

            } else if (priority == cur_priority) {
                /* Only place new_node behind same priority nodes */
                same_priority = true;

            } else if (priority != cur_priority) {
                if (same_priority) {
                    /* We are now behind same priority nodes */
                    this->insert(last_node, new_node);
                    goto exit;
                }
            }

            last_node = seek_ptr;
            seek_ptr = get_next(seek_ptr);
        }

        /* 
         * case: (skip_node == nullptr) 
         *      Every node had higher priority than new_node *or same
         * 
         * case: (skip_node != nullptr)
         *      skip_node was either: 
         *          - last task with the same priority
         *          - or list end
         */
        this->insert(last_node, new_node);
    exit:
        return KERNEL_OK;
    }
};

#endif  // _SCHED_SLINKED_LIST_PRIV_H_