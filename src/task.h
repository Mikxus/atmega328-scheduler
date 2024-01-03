#ifndef _TASK_H_
#define _TASK_H_

#include <cstdint>

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
} task_state;

struct task_data {
    uint16_t id;
    
    task_state state;
};

