#include "context_switch.h"

/**
 * @brief Task switch interrupt
 * naked ISR to avoid C++ prologue/epilogue generation
 * 
 */
ISR(TIMER0_COMPB_vect, ISR_NAKED) __attribute__((hot, flatten));
ISR(TIMER0_COMPB_vect)
{
    /**
     * @brief Saves the current task context into c_task structure
     *      This is inlined in the COMPB interrupt handler
     *      and should be the first thing the interrupt does 
     * @note Assumptions:
     *     - c_task is valid
     *     - Task's stack has atleast 5 bytes free for the context switch to
     *      happen without corrupting memory
     *     - pc is saved at stack, not in task's cpu structure
     * 
     * Stack layout:
     *  SP[-2]  : currently running tasks pc l
     *  SP[-1]  : currently running tasks pc h
     * ------ Below Stack pointer changes made by asm ---  
     *  sp[0]   : r29
     *  sp[1]   : r30
     *  sp[2]   : r31
     */
    asm volatile (
        "cli                            \n\t" // Disable interrupts while saving context
        "push   r29                     \n\t"
        "push   r30                     \n\t" // Push Z ptr to stack
        "push   r31                     \n\t"
        "lds    r30, %[ctask]           \n\t" // Then load c_task ptr
        "lds    r31, %[ctask]+1         \n\t"
        "in     r29, __SREG__           \n\t"
        "std    Z+%[sreg_offset], r29   \n\t" // store SREG
        "pop    r29                     \n\t" // Get r31 from stack
        "std    Z+%[r31_offset], r29    \n\t" // Store r31 
        "pop    r29                     \n\t" 
        "std    Z+%[r30_offset], r29    \n\t" // store r30
        "pop    r29                     \n\t"
        "std    Z+%[r29_offset], r29    \n\t" // store r29
        "in     r29, __SP_L__           \n\t" 
        "std    Z+%[sp_offset], r29     \n\t" // Store sp low byte
        "in     r29, __SP_H__           \n\t"
        "std    Z+%[sp_offset]+1, r29   \n\t" // Store sp high byte
        // Now push all remaining registers r0-r28
        "std    Z+%[r0_offset], r0      \n\t"
        "std    Z+%[r0_offset]+1, r1    \n\t"
        "std    Z+%[r0_offset]+2, r2    \n\t"
        "std    Z+%[r0_offset]+3, r3    \n\t"
        "std    Z+%[r0_offset]+4, r4    \n\t"
        "std    Z+%[r0_offset]+5, r5    \n\t"
        "std    Z+%[r0_offset]+6, r6    \n\t"
        "std    Z+%[r0_offset]+7, r7    \n\t"
        "std    Z+%[r0_offset]+8, r8    \n\t"
        "std    Z+%[r0_offset]+9, r9    \n\t"
        "std    Z+%[r0_offset]+10, r10  \n\t"
        "std    Z+%[r0_offset]+11, r11  \n\t"
        "std    Z+%[r0_offset]+12, r12  \n\t"
        "std    Z+%[r0_offset]+13, r13  \n\t"
        "std    Z+%[r0_offset]+14, r14  \n\t"
        "std    Z+%[r0_offset]+15, r15  \n\t"
        "std    Z+%[r0_offset]+16, r16  \n\t"
        "std    Z+%[r0_offset]+17, r17  \n\t"
        "std    Z+%[r0_offset]+18, r18  \n\t"
        "std    Z+%[r0_offset]+19, r19  \n\t"
        "std    Z+%[r0_offset]+20, r20  \n\t"
        "std    Z+%[r0_offset]+21, r21  \n\t"
        "std    Z+%[r0_offset]+22, r22  \n\t"
        "std    Z+%[r0_offset]+23, r23  \n\t"
        "std    Z+%[r0_offset]+24, r24  \n\t"
        "std    Z+%[r0_offset]+25, r25  \n\t"
        "std    Z+%[r0_offset]+26, r26  \n\t"
        "std    Z+%[r0_offset]+27, r27  \n\t"
        "std    Z+%[r0_offset]+28, r28  \n\t"
        :
        : 
            [ctask]         "m" (c_task),           // Ctask pointer value
            [r31_offset]    "n" (offsetof(task_data_t, cpu_state.regs[31])),
            [r30_offset]    "n" (offsetof(task_data_t, cpu_state.regs[30])),
            [r29_offset]    "n" (offsetof(task_data_t, cpu_state.regs[29])),
            [sreg_offset]   "n" (offsetof(task_data_t, cpu_state.sreg)),
            [sp_offset]     "n" (offsetof(task_data_t, cpu_state.sp)),
            [r0_offset]     "n" (offsetof(task_data_t, cpu_state.regs[0]))
        : 
            "r0","r1","r2","r3","r4","r5","r6","r7","r8","r9","r10",
            "r11","r12","r13","r14","r15","r16","r17","r18","r19","r20",
            "r21","r22","r23","r24","r25","r26","r27","r28","r29","r30","r31","memory","cc"
    );
    /* 
     * I think we can write c++ code from here on
     */
    asm volatile ("clr r1" ::: "memory");

    // schedule next task
    // for now just re-run the same task
    
    /* Schedule next 1ms time slice */
    OCR0B = FREQ_TO_TIMER_VALUE(1000, 64) + TCNT0; // ~1 ms task switch interval

    asm volatile (
        "lds    r30, %[ctask]           \n\t" // setup z pointer with new ctask
        "lds    r31, %[ctask]+1         \n\t"
        "ldd    r29, Z+%[sreg_offset]   \n\t"
        "out    __SREG__, r29           \n\t" // Load sreg
        "ldd     r28, Z+%[sp_offset]    \n\t"
        "ldd     r29, Z+%[sp_offset]+1  \n\t"
        "out    __SP_L__, r28           \n\t" // Load sp low byte
        "out    __SP_H__, r29           \n\t" // load sp high
        // Now we have sp pointing to new task's stack
        // Lets push new task's r30 & r31 to it's stack for later use
        "ldd     r28, Z+%[r30_offset]   \n\t"
        "ldd     r29, Z+%[r30_offset]+1 \n\t"
        "push   r28                     \n\t"
        "push   r29                     \n\t"
        // Now load registers r0 - r29
        "ldd    r0, Z+%[r0_offset]      \n\t"
        "ldd    r1, Z+%[r0_offset]+1    \n\t"
        "ldd    r2, Z+%[r0_offset]+2    \n\t"
        "ldd    r3, Z+%[r0_offset]+3    \n\t"
        "ldd    r4, Z+%[r0_offset]+4    \n\t"
        "ldd    r5, Z+%[r0_offset]+5    \n\t"
        "ldd    r6, Z+%[r0_offset]+6    \n\t"
        "ldd    r7, Z+%[r0_offset]+7    \n\t"
        "ldd    r8, Z+%[r0_offset]+8    \n\t"
        "ldd    r9, Z+%[r0_offset]+9    \n\t"
        "ldd    r10, Z+%[r0_offset]+10  \n\t"
        "ldd    r11, Z+%[r0_offset]+11  \n\t"
        "ldd    r12, Z+%[r0_offset]+12  \n\t"
        "ldd    r13, Z+%[r0_offset]+13  \n\t"
        "ldd    r14, Z+%[r0_offset]+14  \n\t"
        "ldd    r15, Z+%[r0_offset]+15  \n\t"
        "ldd    r16, Z+%[r0_offset]+16  \n\t"
        "ldd    r17, Z+%[r0_offset]+17  \n\t"
        "ldd    r18, Z+%[r0_offset]+18  \n\t"
        "ldd    r19, Z+%[r0_offset]+19  \n\t"
        "ldd    r20, Z+%[r0_offset]+20  \n\t"
        "ldd    r21, Z+%[r0_offset]+21  \n\t"
        "ldd    r22, Z+%[r0_offset]+22  \n\t"
        "ldd    r23, Z+%[r0_offset]+23  \n\t"
        "ldd    r24, Z+%[r0_offset]+24  \n\t"
        "ldd    r25, Z+%[r0_offset]+25  \n\t"
        "ldd    r26, Z+%[r0_offset]+26  \n\t"
        "ldd    r27, Z+%[r0_offset]+27  \n\t"
        "ldd    r28, Z+%[r0_offset]+28  \n\t"
        "ldd    r29, Z+%[r0_offset]+29  \n\t"
        "pop    r31                     \n\t" // Get r31 from stack
        "pop    r30                     \n\t" // Get r30 from stack
        "reti                           \n\t" // Return from interrupt, pops pc from stack
        :
        :
            [ctask]         "m" (c_task),
            [sreg_offset]   "n" (offsetof(task_data_t, cpu_state.sreg)),
            [sp_offset]     "n" (offsetof(task_data_t, cpu_state.sp)),
            [r30_offset]    "n" (offsetof(task_data_t, cpu_state.regs[30])),
            [r0_offset]     "n" (offsetof(task_data_t, cpu_state.regs[0]))

        : "memory"
    );
}

void initialize_context_switch_timer(uint8_t burst_lenght)
{
    uint8_t sreg = SREG;
    cli();

    // assume timer0 is already initialized 
    enable_timer0_interrupt(COMPB_INTERRUPT);

    OCR0B = (uint8_t) 249 + TCNT0;  // try to trigger at 1 ms intervals 
    SREG = sreg;
    return;
    
}
