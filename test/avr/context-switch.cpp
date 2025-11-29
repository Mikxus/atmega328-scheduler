#include <stdio.h>
#include <avr/sleep.h>
#include <avr/io.h>
#include "src/scheduler.h"

int main(void)
{
    volatile task_data_t task;
    task.exec_time_us = 0xFAFA;
    task.exec_time_overflow_count = 0xDD;
    task.state = READY;
    c_task = &task;

    cli();
    initialize_clock();
    initialize_context_switch_timer(1);
    /* set cpu registers to a known state before interrupt */
    __asm__ __volatile__ (
        "ldi r16, 0x61 \n\t"
        "ldi r17, 0x62 \n\t"
        "ldi r18, 0x63 \n\t"
        "ldi r19, 0x64 \n\t"
        "ldi r20, 0x65 \n\t"
        "ldi r21, 0x66 \n\t"
        "ldi r22, 0x67 \n\t"
        "ldi r23, 0x68 \n\t"
        "ldi r24, 0x69 \n\t"
        "ldi r25, 0x6A \n\t"
        "ldi r26, 0x6B \n\t"
        "ldi r27, 0x6C \n\t"
        "ldi r28, 0x6D \n\t"
        "ldi r29, 0x6F \n\t"
        "ldi r30, 0x70 \n\t"
        "ldi r31, 0x71 \n\t"
        /* move loaded values into r0-r15 (ldi only works for r16-r31) */
        "mov r0,  r16  \n\t"
        "mov r1,  r17  \n\t"
        "mov r2,  r18  \n\t"
        "mov r3,  r19  \n\t"
        "mov r4,  r20  \n\t"
        "mov r5,  r21  \n\t"
        "mov r6,  r22  \n\t"
        "mov r7,  r23  \n\t"
        "mov r8,  r24  \n\t"
        "mov r9,  r25  \n\t"
        "mov r10, r26  \n\t"
        "mov r11, r27  \n\t"
        "mov r12, r28  \n\t"
        "mov r13, r29  \n\t"
        "mov r14, r30  \n\t"
        "mov r15, r31  \n\t"
        :
        :
        : "memory"
    );
    sei();

    /* wait for context switch */
    while (1)
    {
    }
}