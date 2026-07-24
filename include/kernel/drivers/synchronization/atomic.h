/**
 * @file atomic.h
 * @brief RAII style atomic helper macros
 */
#ifndef _ATOMIC_H_
#define _ATOMIC_H_

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>

/*
 * @brief Internal atomic guard class
 * 
 * @details RAII style interrupt state disabling and restoring.
 * 
 * @note Only meant for internal use by atomic.h
 */
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
 *
 * @details Usage example:
 * @code {.c}
 *      ATOMIC_BLOCK() 
 *      {
 *          -- interupts disabled here
 *      }
 *      -- Interrupt state restored here
 * @endcode
 * 
 */
#define ATOMIC_BLOCK()  \
	for (bool _once = 1; _once; _once = 0)  \
		for (atomic_guard _guard_obj; _once; _once = 0)

/**
 * @brief Execute current scope with interrupts disabled
 * 
 * @details Usage example:
 * @code {.c}
 *      void my_func(void) 
 *      {
 *              -- interrupts enabled here
 *          ATOMIC_GUARD();
 *				-- interupts disabled here
 *          return;		-- Interrupt state restored here
 *      }
 * @endcode
 * 
 * @note MUST be used within a braced scope {}.
 *      Do not use in single-line if/for/while statements without braces.  
 * 
 */
#define ATOMIC_GUARD()  \
	atomic_guard _guard_obj;

#endif // _ATOMIC_H_