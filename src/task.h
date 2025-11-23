#ifndef _TASK_H_
#define _TASK_H_

#include <stdint.h>
#include "lib/singly_linked_list/singly_linked_list.h"
#include "cpu.h"


/**
 * @brief Enum for task's possible states
 */
typedef enum
{
    READY,
    RUNNING,
    BLOCKED,
    SLEEP,
    UNDEFINED
} task_state_t;

/**
 * @brief Task memory data structure
 */
struct task_memory
{
    uint8_t * memory_ptr;
    uint16_t size;

    uint8_t * get_ram_end()
    {
        return memory_ptr + size;
    }

    uint8_t * get_ram_start()
    {
        return memory_ptr;
    }
};

/**
 * @brief Task data structure
 * @note: No initializers, or methods
 *      This structure has to standard layout for offsetof to work correctly
 */
typedef struct task_data_t
{
    /* singly linked list node for the current task */
    sl_list::node<task_data_t> task_node;

    uint16_t id;

    struct task_memory memory;
    volatile task_state_t state;
    volatile cpu_registers cpu_state;

    volatile uint32_t last_exec_time_ms;
    volatile uint16_t burst_time_ms;
};

#endif