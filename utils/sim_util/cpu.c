#include "cpu.h"

uint8_t read_register(avr_t *avr, uint8_t reg_num)
{
    if (reg_num > 31)
    {
        ERROR("Out of range register read: %u", reg_num);
        return 0;
    } 

    return avr->data[reg_num];
}