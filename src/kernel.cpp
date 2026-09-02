#include <kernel/kernel.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stddef.h>

#include <kernel/task.h>
#include <kernel/atomic.h>
#include <kernel/drivers/clock.h>
#include <kernel/drivers/uart.h>
#include <kernel/drivers/timer.h>

#include "task_utils.h"
#include "drivers/scheduling/sched.h"

#define CONTEXT_SWITCH_HZ 1000 
#define CONTEXT_SWITCH_PRESCALER 64

/**
 * @brief Converts frequency to timer compare value
 */
template<uint16_t _FREQ, uint16_t _PRESCALER>
constexpr uint8_t freq_to_timer_comp_value() {
    static_assert(_FREQ > 0, "Frequency must be greater than 0");
    static_assert(_PRESCALER == 1 || _PRESCALER == 8 || _PRESCALER == 64 || _PRESCALER == 256 || _PRESCALER == 1024, "Invalid prescaler value");

    constexpr int32_t raw = (F_CPU / (static_cast<uint32_t>(_PRESCALER) * _FREQ)) - 4;

    static_assert(raw >= 0, "Frequency too low for given prescaler");
    static_assert(raw <= UINT8_MAX, "Frequency too high for given prescaler");
    return static_cast<uint8_t>(raw);
}

/**
 * @brief Global variable to store the currently running task  
 */
task_data_t * volatile _c_task = nullptr;

/**
 * @brief Calculate and update task execution time statistics
 * @note   
 * @param  *task: 
 * @retval None
 */
void calculate_task_execution_time(task_data_t volatile *task)
{
    #if CONF_TRACK_TASK_CPU_TIME == 1
    if (task == nullptr)
        return;

    uint32_t current_time = get_us();
    uint32_t exec_time;
    
    // Handle timer wraparound correctly
    if (current_time >= task->exec_start_time_us) {
        exec_time = current_time - task->exec_start_time_us;
    } else {
        // Wraparound occurred
        exec_time = (UINT32_MAX - task->exec_start_time_us) + current_time + 1;
    }
    
    // Check if adding exec_time would overflow
    if (task->exec_time_us > UINT32_MAX - exec_time) {
        task->exec_time_overflow_count++;
        // Optional: wrap the counter or reset it
        task->exec_time_us = exec_time;  // Start fresh after overflow
    } else {
        task->exec_time_us += exec_time;
    }
    #endif
} 

/**
 * @brief Saves the current task context into task_data_t structure
 *      This is inlined in the COMPB interrupt handler
 *      and should be the first thing the interrupt does 
 * @note Assumptions:
 *     - _c_task is valid
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
        "in   r29, __SREG__             \n\t" \
        "push r30                       \n\t" \
        "push r31                       \n\t" \
        "lds  r30, %[ctask]             \n\t" \
        "lds  r31, %[ctask]+1           \n\t" \
        "std  Z+%[sreg_offset], r29     \n\t" \
        "pop  r29                       \n\t" \
        "std  Z+%[r31_offset],  r29     \n\t" \
        "pop  r29                       \n\t" \
        "std  Z+%[r30_offset],  r29     \n\t" \
        "pop  r29                       \n\t" \
        "std  Z+%[r29_offset],  r29     \n\t" \
        "in   r29, __SP_L__             \n\t" \
        "std  Z+%[sp_offset],   r29     \n\t" \
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
        : [ctask]       "m" (_c_task),                                      \
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

/**
 * @brief Saves the current task context into task_data_t structure
 *        Enables interrupts as soon as possible and is used in 
 *        non interrupt funcs
 * @note Assumptions:
 *     - _c_task is valid
 *     - pc is saved at stack, not in task's cpu structure
 * 
 * Stack layout:
 *  SP[-2]  : currently running tasks pc l
 *  SP[-1]  : currently running tasks pc h
 * ------ Below temporary stack pointer changes made by asm ------
 *  sp[0]   : r29
 *  sp[1]   : r30
 *  sp[2]   : r31
 */
#define _SAVE_CTX()                       \
    asm volatile (                            \
        "push r29                       \n\t" \
        "in   r29, __SREG__             \n\t" \
        "cli                            \n\t" \
        "push r30                       \n\t" \
        "push r31                       \n\t" \
        "lds  r30, %[ctask]             \n\t" \
        "lds  r31, %[ctask]+1           \n\t" \
        "std  Z+%[sreg_offset], r29     \n\t" \
        "pop  r29                       \n\t" \
        "std  Z+%[r31_offset],  r29     \n\t" \
        "pop  r29                       \n\t" \
        "std  Z+%[r30_offset],  r29     \n\t" \
        "pop  r29                       \n\t" \
        "std  Z+%[r29_offset],  r29     \n\t" \
        "in   r29, __SP_L__             \n\t" \
        "std  Z+%[sp_offset],   r29     \n\t" \
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
        : [ctask]       "m" (_c_task),                                       \
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


/**
 * @brief Restores the next task context from _c_task  
 */
#define _RESTORE_CTX_ISR()                                                                  \
    asm volatile (                                                                          \
        "lds    r30, %[ctask]           \n\t"                                               \
        "lds    r31, %[ctask]+1         \n\t"                                               \
        "ldd    r28, Z+%[sp_offset]     \n\t"                                               \
        "ldd    r29, Z+%[sp_offset]+1   \n\t"                                               \
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
        "ldd    r29, Z+%[sreg_offset]   \n\t"                                               \
        "out    __SREG__, r29           \n\t"                                               \
        "ldd    r29, Z+%[r0_offset]+29  \n\t"                                               \
        "pop    r31                     \n\t"                                               \
        "pop    r30                     \n\t"                                               \
        :                                                                                   \
        :                                                                                   \
            [ctask]         "m" (_c_task),                                                  \
            [sreg_offset]   "n" (offsetof(task_data_t, cpu_state.sreg)),                    \
            [sp_offset]     "n" (offsetof(task_data_t, cpu_state.sp)),                      \
            [r30_offset]    "n" (offsetof(task_data_t, cpu_state.regs[30])),                \
            [r0_offset]     "n" (offsetof(task_data_t, cpu_state.regs[0]))                  \
                                                                                            \
        : "memory"                                                                          \
    );

#define _RESTORE_CTX() _RESTORE_CTX_ISR()

/**
 * @brief Context switch C++ code has to be in it's own function
 *        with attribute noinline to force GCC generate a prologue  
 *        otherwise gcc would trust that the registers are set up correctly
 *        leading to spurious memory corruption 
 */
void __attribute__((noinline)) _do_ctx_isr(void) {
    calculate_task_execution_time(_c_task);

    /* 
     * The hardware automatically disables interrupts upon entering isr and 
     * restores them with reti. yield() Doesn't explicitly enable interrupts upon
     * rescheduling tasks. Leading to possibility of deadlock when task is 
     * saved in isr and rescheduled by yield().
     */
    (*_c_task).cpu_state.sreg |= (1 << SREG_I);

    _schedule_next_task();

    OCR0B = freq_to_timer_comp_value
    <CONTEXT_SWITCH_HZ, CONTEXT_SWITCH_PRESCALER>() + TCNT0; // ~1 ms task switch interval

    #if CONF_TRACK_TASK_CPU_TIME == 1
    _c_task->exec_start_time_us = get_us();
    #endif
    return;
}

/**
 * @brief Task switch interrupt
 * naked ISR to avoid prologue/epilogue messing with registers
 */
ISR(TIMER0_COMPB_vect, ISR_NAKED)
{
    _SAVE_CTX_ISR();
    asm volatile ("clr r1" ::: "memory");
    _do_ctx_isr();
    _RESTORE_CTX_ISR();
    asm volatile ("reti"  ::: "memory");
}

void kernel_init_timer(void)
{
    ATOMIC_GUARD();

    // assume timer0 is already initialized 
    enable_timer0_interrupt(tmr0_int_t::COMPB_INTERRUPT);

    // try to trigger at 1 ms intervals 
    OCR0B = freq_to_timer_comp_value<
        CONTEXT_SWITCH_HZ, CONTEXT_SWITCH_PRESCALER>() + TCNT0;
    return;
}

void kernel_init(void)
{
    ATOMIC_BLOCK() {
        initialize_uart();
        initialize_clock();
    }

    return;
}

void __attribute__((noreturn)) kernel_start(void)
{
    kernel_init_timer();
    _RESTORE_CTX_ISR();
    asm volatile ("ret" ::: "memory");
    __builtin_unreachable();
}

void soft_yield(void)
{
    ATOMIC_BLOCK() {
        OCR0B = TCNT0;
    }
}

/**
 * @brief Yield c++ code has to be in it's own function to
 *        force generate prologue for the C++ code
 * @retval None
 */
void __attribute__((noinline)) _do_ctx_yield(void) {
    calculate_task_execution_time(_c_task);

    _schedule_next_task();

    OCR0B = freq_to_timer_comp_value<1000, 64>() + TCNT0; // ~1 ms task switch interval
    
    /* 
     * it is possible that task's time slice had
     *  run out while we're switching task. So lets clear the flag
     */
    TIFR0 = (1 << OCF0B);

    #if CONF_TRACK_TASK_CPU_TIME == 1
    _c_task->exec_start_time_us = get_us();
    #endif
    return;
}

void __attribute__((naked, noinline)) yield(void)
{
    _SAVE_CTX();
    /**
     *  GCC ABI expects R1 to be 0  
     */
    asm volatile ("clr r1" ::: "memory");
    _do_ctx_yield();
    _RESTORE_CTX();
    asm volatile ("ret" ::: "memory");
}