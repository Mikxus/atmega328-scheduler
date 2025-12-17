#include <stdint.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

const uint8_t register_values[32] =
{
    255, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    11, 12, 13, 14, 15, 16, 17 ,18, 19, 20,
    21, 22, 23, 24, 25 ,26, 27 ,28, 29, 30, 31
};

int main(void)
{
    __asm__ (
        "lds r0,  %0     \n\t"
        "lds r1,  %1     \n\t"
        "lds r2,  %2     \n\t"
        "lds r3,  %3     \n\t"
        "lds r4,  %4     \n\t"
        "lds r5,  %5     \n\t"
        "lds r6,  %6     \n\t"
        "lds r7,  %7     \n\t"
        "lds r8,  %8     \n\t"
        "lds r9,  %9     \n\t"
        "lds r10, %10    \n\t"
        "lds r11, %11    \n\t"
        "lds r12, %12    \n\t"
        "lds r13, %13    \n\t"
        "lds r14, %14    \n\t"
        "lds r15, %15    \n\t"
        "lds r16, %16    \n\t"
        "lds r17, %17    \n\t"
        "lds r18, %18    \n\t"
        "lds r19, %19    \n\t"
        "lds r20, %20    \n\t"
        "lds r21, %21    \n\t"
        "lds r22, %22    \n\t"
        "lds r23, %23    \n\t"
        "lds r24, %24    \n\t"
        "lds r25, %25    \n\t"
        "lds r26, %26    \n\t"
        "lds r27, %27    \n\t"
        "lds r28, %28    \n\t"
        : 
        :   
            "i" (&register_values[0]),
            "i" (&register_values[1]),
            "i" (&register_values[2]),
            "i" (&register_values[3]),
            "i" (&register_values[4]),
            "i" (&register_values[5]),
            "i" (&register_values[6]),
            "i" (&register_values[7]),
            "i" (&register_values[8]),
            "i" (&register_values[9]),
            "i" (&register_values[10]),
            "i" (&register_values[11]),
            "i" (&register_values[12]),
            "i" (&register_values[13]),
            "i" (&register_values[14]),
            "i" (&register_values[15]),
            "i" (&register_values[16]),
            "i" (&register_values[17]),
            "i" (&register_values[18]),
            "i" (&register_values[19]),
            "i" (&register_values[20]),
            "i" (&register_values[21]),
            "i" (&register_values[22]),
            "i" (&register_values[23]),
            "i" (&register_values[24]),
            "i" (&register_values[25]),
            "i" (&register_values[26]),
            "i" (&register_values[27]),
            "i" (&register_values[28])
        :
    );

    asm (
        "lds r29,  %0     \n\t"
        "lds r30,  %1     \n\t"
        "lds r31,  %2     \n\t"
        : 
        :   
            "i" (&register_values[29]),
            "i" (&register_values[30]),
            "i" (&register_values[31])
        :
    );

    cli();
    sleep_cpu();

    return 1;
}