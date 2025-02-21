#include "timer.h"

void timer0_set_waveform_generation_mode(timer0_waveform_generation_mode_t mode)
{
    /*
    * waveform generation mode is split between TCCR0A and TCCR0B registers 
    * TCCR0A: WGM01, WGM00
    * TCCR0B: WGM02
    */
    TCCR0A = (TCCR0A & ~((1 << WGM01) | 1 << WGM00)) | (mode & (1 << WGM01 | 1 << WGM00));
    TCCR0B = (TCCR0B & ~(1 << WGM02)) | (mode & (1 << WGM02));
}

void timer0_set_clock_source(timer0_clock_select_t clock_source)
{
    TCCR0B &= ~((1 << CS02) | (1 << CS01) | (1 << CS00));
    TCCR0B |= clock_source;
}

void enable_timer0(void)
{
    PRR |= (1 << PRTIM0);
}

void disable_timer0(void)
{
    PRR &= ~(1 << PRTIM0);
}