#ifndef _ATOMIC_H_
#define _ATOMIC_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>

class atomic_guard {
    uint8_t old_sreg;

public:
    atomic_guard() __attribute__((always_inline)) {
        old_sreg = SREG;
        cli();
    }

    ~atomic_guard() __attribute__((always_inline)) {
        __asm__ __volatile__ ("" ::: "memory");
        SREG = old_sreg;
        __asm__ __volatile__ ("" ::: "memory");
    }

    /* Block copy constructor */
    atomic_guard(const atomic_guard&) = delete;

    /* Block copy assignment */
    atomic_guard& operator=(const atomic_guard&) = delete;
};

/**
 * @brief Execute block of code with interrupts disabled 
 * example:
 *  ATOMIC_BLOCK() 
 * {
 *      -- interupts disabled here
 * }
 * -- Interrupt state restored here
 */
#define ATOMIC_BLOCK()  \
    for (bool _once = 1; _once; _once = 0)  \
        for (atomic_guard _guard_obj; _once; _once = 0)

/**
 * @brief Atomic guard. After this block code is executed with interrupts disabled
 *      until this goes out of scope.
 * @note MUST be used within a braced scope {}
 *      Do not use in single-line if/for/while statements without braces.  
 */
#define ATOMIC_GUARD()  \
    atomic_guard _guard_obj;

#endif // _ATOMIC_H_