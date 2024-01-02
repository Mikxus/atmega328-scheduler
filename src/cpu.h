#ifndef _CPU_H_
#define _CPU_H_

#include <stdint.h>

/**
 * @brief Atmega328p's cpu state for context switching
*/
struct cpu_state
{
    union
    {
        uint8_t r0;
        uint8_t r1;
        uint8_t r2;
        uint8_t r3;
        uint8_t r4;
        uint8_t r5;
        uint8_t r6;
        uint8_t r7;
        uint8_t r8;
        uint8_t r9;
        uint8_t r10;
        uint8_t r11;
        uint8_t r12;
        uint8_t r13;
        uint8_t r14;
        uint8_t r15;
        uint8_t r16;
        uint8_t r17;
        uint8_t r18;
        uint8_t r19;
        uint8_t r20;
        uint8_t r21;
        uint8_t r22;
        uint8_t r23;
        uint8_t r24;
        uint8_t r25;
        
        /* X pointer registers */
        union {
            struct
            {
                union {
                    uint8_t xl_pointer;
                    uint8_t r26;
                };

                union {
                    uint8_t xh_pointer;
                    uint8_t r27;
                };
            };

            uint16_t x_pointer;
        };

        /* Y pointer registers */
        union {
            struct
            {
                union {
                    uint8_t yl_pointer;
                    uint8_t r28;
                };

                union {
                    uint8_t yh_pointer;
                    uint8_t r29;
                };
            };

            uint16_t y_pointer;
        };

        /* Z pointer registers */
        union {
            struct
            {
                union {
                    uint8_t zl_pointer;
                    uint8_t r30;
                };

                union {
                    uint8_t zh_pointer;
                    uint8_t r31;
                };
            };

            uint16_t z_pointer;
        };
    };

    uint16_t pc;

    /* Stack pointer */
    union {
        struct
        {
            uint8_t spl;
            uint8_t sph;
        };

        uint16_t sp;
    };

    /* SREG status register */
    union {
        struct
        {
            /* C: Carry Flag */
            uint8_t c : 1;
            
            /* Z: Zero Flag */
            uint8_t z : 1;
            
            /* N: Negative Flag */
            uint8_t n : 1;
            
            /* V: Two's complement overflow indicator */
            uint8_t v : 1;
            
            /* S: N ⊕ V, For signed tests */
            uint8_t s : 1;
            
            /* H: Half Carry Flag */
            uint8_t h : 1;
            
            /* T: Bit copy storage */
            uint8_t t : 1;
            
            /* I: Global interrupt enable/disable */
            uint8_t i : 1;
        } sreg_bits;

        /* AVR status register */
        uint8_t sreg;
    };
};

#endif