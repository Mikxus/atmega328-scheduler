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

struct task_data
{
private:
    /* singly linked list node for the current task */
    sl_list::node<task_data> task_node = sl_list::node<task_data>(this, nullptr);

public:
    uint16_t id;

    task_state_t state;
    cpu_registers cpu_state;

    uint32_t last_exec_time_ms;
    uint16_t burst_time_ms;
};

#endif