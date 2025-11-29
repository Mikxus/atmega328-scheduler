#ifndef _CPU_H_
#define _CPU_H_

#include <stdint.h>

/**
 * @brief Atmega328p's cpu registers' state for context switching
*/
struct __attribute__((packed)) cpu_registers
{
    uint8_t regs[32]; // r0 - r31
    uint16_t sp;     // stack pointer
    uint8_t sreg;    // status register
};

#endif