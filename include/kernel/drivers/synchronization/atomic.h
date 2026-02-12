#ifndef _ATOMIC_H_
#define _ATOMIC_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>

/**
 * @brief Execute block of code with interrupts disabled 
 * example:
 *  ATOMIC_BLOCK() 
 * {
 *      -- interupts disabled here
 * }
 * -- Interrupt state restored here
 */
#define ATOMIC_BLOCK()                                                              \
    for (uint8_t _atomic_sreg_##__COUNTER__ __attribute__((cleanup(_atomic_restore))) = ({ uint8_t s = SREG; cli(); s; }), \
                 _once = 1; _once; _once = 0)

/**
 * @brief Atomic guard. After this block code is executed with interrupts disabled
 * @note MUST be used within a braced scope {}
 *      Do not use in single-line if/for/while statements without braces  
 */
#define ATOMIC_GUARD()                                                                 \
    uint8_t _old_sreg##__COUNTER__ __attribute__((cleanup(_atomic_restore))) = SREG;   \
    cli();

static inline __attribute__((always_inline)) void _atomic_restore(uint8_t *old_sreg_ptr) 
{
    __asm__ __volatile__ ("" ::: "memory"); // mem barrier
    SREG = *old_sreg_ptr;
    __asm__ __volatile__ ("" ::: "memory");
}
#endif // _ATOMIC_H_