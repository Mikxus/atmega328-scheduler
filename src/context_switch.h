#ifndef _CONTEXT_SWITCH_H_
#define _CONTEXT_SWITCH_H_

#include <inttypes.h>
#include <stddef.h>
#include <avr/interrupt.h>

#include "cpu.h"
#include "task.h"
#include "drivers/clock/clock.h"
#include "drivers/timer/timer.h"

/**
 * @brief Converts frequency to timer compare value
 * @note  result may overflow if targeted frequency isn't possible with given prescaler 
 */
constexpr uint8_t freq_to_timer_comp_value(uint16_t freq, uint16_t prescaler) {
    return static_cast<uint8_t>((F_CPU / (static_cast<uint32_t>(prescaler) * freq)) - 4);
}

/**
 * @brief sets timer2's comp a unit to interrupt at defined intervals
 * 
 */
void start_context_switch_timer(void);

void yield_task(void);
void calculate_task_execution_time(task_data_t volatile *task);
void simple_schedule_next_task(void);

uint8_t get_context_switch_burst_length(uint8_t ms);

/**
 * @brief Saves the current task context into c_task structure
 *      This is inlined in the COMPB interrupt handler
 *      and should be the first thing the interrupt does 
 * @note Assumptions:
 *     - c_task is valid
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
#define _SAVE_CTX_ISR()                       \
    asm volatile (                            \
        "cli                            \n\t" \
        "push r29                       \n\t" \
        "push r30                       \n\t" \
        "push r31                       \n\t" \
        "lds  r30, %[ctask]             \n\t" \
        "lds  r31, %[ctask]+1           \n\t" \
        "in   r29, __SREG__             \n\t" \
        "std  Z+%[sreg_offset], r29     \n\t" \
        "pop  r29                       \n\t" \
        "std  Z+%[r31_offset], r29      \n\t" \
        "pop  r29                       \n\t" \
        "std  Z+%[r30_offset], r29      \n\t" \
        "pop  r29                       \n\t" \
        "std  Z+%[r29_offset], r29      \n\t" \
        "in   r29, __SP_L__             \n\t" \
        "std  Z+%[sp_offset], r29       \n\t" \
        "in   r29, __SP_H__             \n\t" \
        "std  Z+%[sp_offset]+1, r29     \n\t" \
        "std  Z+%[r0_offset]+0,  r0     \n\t" \
        "std  Z+%[r0_offset]+1,  r1     \n\t" \
        "std  Z+%[r0_offset]+2,  r2     \n\t" \
        "std  Z+%[r0_offset]+3,  r3     \n\t" \
        "std  Z+%[r0_offset]+4,  r4     \n\t" \
        "std  Z+%[r0_offset]+5,  r5     \n\t" \
        "std  Z+%[r0_offset]+6,  r6     \n\t" \
        "std  Z+%[r0_offset]+7,  r7     \n\t" \
        "std  Z+%[r0_offset]+8,  r8     \n\t" \
        "std  Z+%[r0_offset]+9,  r9     \n\t" \
        "std  Z+%[r0_offset]+10, r10    \n\t" \
        "std  Z+%[r0_offset]+11, r11    \n\t" \
        "std  Z+%[r0_offset]+12, r12    \n\t" \
        "std  Z+%[r0_offset]+13, r13    \n\t" \
        "std  Z+%[r0_offset]+14, r14    \n\t" \
        "std  Z+%[r0_offset]+15, r15    \n\t" \
        "std  Z+%[r0_offset]+16, r16    \n\t" \
        "std  Z+%[r0_offset]+17, r17    \n\t" \
        "std  Z+%[r0_offset]+18, r18    \n\t" \
        "std  Z+%[r0_offset]+19, r19    \n\t" \
        "std  Z+%[r0_offset]+20, r20    \n\t" \
        "std  Z+%[r0_offset]+21, r21    \n\t" \
        "std  Z+%[r0_offset]+22, r22    \n\t" \
        "std  Z+%[r0_offset]+23, r23    \n\t" \
        "std  Z+%[r0_offset]+24, r24    \n\t" \
        "std  Z+%[r0_offset]+25, r25    \n\t" \
        "std  Z+%[r0_offset]+26, r26    \n\t" \
        "std  Z+%[r0_offset]+27, r27    \n\t" \
        "std  Z+%[r0_offset]+28, r28    \n\t" \
        :                                                                   \
        : [ctask]       "m" (c_task),                                       \
          [r31_offset]  "n" (offsetof(task_data_t, cpu_state.regs[31])),    \
          [r30_offset]  "n" (offsetof(task_data_t, cpu_state.regs[30])),    \
          [r29_offset]  "n" (offsetof(task_data_t, cpu_state.regs[29])),    \
          [sreg_offset] "n" (offsetof(task_data_t, cpu_state.sreg)),        \
          [sp_offset]   "n" (offsetof(task_data_t, cpu_state.sp)),          \
          [r0_offset]   "n" (offsetof(task_data_t, cpu_state.regs[0]))      \
        : "r0","r1","r2","r3","r4","r5","r6","r7","r8","r9","r10",          \
          "r11","r12","r13","r14","r15","r16","r17","r18","r19","r20",      \
          "r21","r22","r23","r24","r25","r26","r27","r28","r29","r30","r31",\
          "memory","cc"                                                     \
    );


#define _RESTORE_CTX_ISR()                                                                  \
    asm volatile (                                                                          \
        "lds    r30, %[ctask]           \n\t"                                               \
        "lds    r31, %[ctask]+1         \n\t"                                               \
        "ldd    r29, Z+%[sreg_offset]   \n\t"                                               \
        "out    __SREG__, r29           \n\t"                                               \
        "ldd     r28, Z+%[sp_offset]    \n\t"                                               \
        "ldd     r29, Z+%[sp_offset]+1  \n\t"                                               \
        "out    __SP_L__, r28           \n\t"                                               \
        "out    __SP_H__, r29           \n\t"                                               \
        "ldd     r28, Z+%[r30_offset]   \n\t"                                               \
        "ldd     r29, Z+%[r30_offset]+1 \n\t"                                               \
        "push   r28                     \n\t"                                               \
        "push   r29                     \n\t"                                               \
        "ldd    r0, Z+%[r0_offset]      \n\t"                                               \
        "ldd    r1, Z+%[r0_offset]+1    \n\t"                                               \
        "ldd    r2, Z+%[r0_offset]+2    \n\t"                                               \
        "ldd    r3, Z+%[r0_offset]+3    \n\t"                                               \
        "ldd    r4, Z+%[r0_offset]+4    \n\t"                                               \
        "ldd    r5, Z+%[r0_offset]+5    \n\t"                                               \
        "ldd    r6, Z+%[r0_offset]+6    \n\t"                                               \
        "ldd    r7, Z+%[r0_offset]+7    \n\t"                                               \
        "ldd    r8, Z+%[r0_offset]+8    \n\t"                                               \
        "ldd    r9, Z+%[r0_offset]+9    \n\t"                                               \
        "ldd    r10, Z+%[r0_offset]+10  \n\t"                                               \
        "ldd    r11, Z+%[r0_offset]+11  \n\t"                                               \
        "ldd    r12, Z+%[r0_offset]+12  \n\t"                                               \
        "ldd    r13, Z+%[r0_offset]+13  \n\t"                                               \
        "ldd    r14, Z+%[r0_offset]+14  \n\t"                                               \
        "ldd    r15, Z+%[r0_offset]+15  \n\t"                                               \
        "ldd    r16, Z+%[r0_offset]+16  \n\t"                                               \
        "ldd    r17, Z+%[r0_offset]+17  \n\t"                                               \
        "ldd    r18, Z+%[r0_offset]+18  \n\t"                                               \
        "ldd    r19, Z+%[r0_offset]+19  \n\t"                                               \
        "ldd    r20, Z+%[r0_offset]+20  \n\t"                                               \
        "ldd    r21, Z+%[r0_offset]+21  \n\t"                                               \
        "ldd    r22, Z+%[r0_offset]+22  \n\t"                                               \
        "ldd    r23, Z+%[r0_offset]+23  \n\t"                                               \
        "ldd    r24, Z+%[r0_offset]+24  \n\t"                                               \
        "ldd    r25, Z+%[r0_offset]+25  \n\t"                                               \
        "ldd    r26, Z+%[r0_offset]+26  \n\t"                                               \
        "ldd    r27, Z+%[r0_offset]+27  \n\t"                                               \
        "ldd    r28, Z+%[r0_offset]+28  \n\t"                                               \
        "ldd    r29, Z+%[r0_offset]+29  \n\t"                                               \
        "pop    r31                     \n\t"                                               \
        "pop    r30                     \n\t"                                               \
        "reti                           \n\t"                                               \
        :                                                                                   \
        :                                                                                   \
            [ctask]         "m" (c_task),                                                   \
            [sreg_offset]   "n" (offsetof(task_data_t, cpu_state.sreg)),                    \
            [sp_offset]     "n" (offsetof(task_data_t, cpu_state.sp)),                      \
            [r30_offset]    "n" (offsetof(task_data_t, cpu_state.regs[30])),                \
            [r0_offset]     "n" (offsetof(task_data_t, cpu_state.regs[0]))                  \
                                                                                            \
        : "memory"                                                                          \
    );

#endif // _CONTEXT_SWITCH_H_