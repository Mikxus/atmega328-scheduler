#include <kernel/drivers/timer.h>

void timer0_set_waveform_generation_mode(tmr0_wvf_gen_mode_t mode)
{
    const uint8_t mask = static_cast<uint8_t>(mode);
    /*
    * waveform generation mode is split between TCCR0A and TCCR0B registers 
    * TCCR0A: WGM01, WGM00
    * TCCR0B: WGM02
    */
    TCCR0A = (TCCR0A & ~((1 << WGM01) | 1 << WGM00)) | (mask & (1 << WGM01 | 1 << WGM00));
    TCCR0B = (TCCR0B & ~(1 << WGM02)) | (mask & (1 << WGM02));
}

void timer0_set_clock_source(tmr0_clk_src_t clock_source)
{
    const uint8_t mask = static_cast<uint8_t>(clock_source);

    TCCR0B &= ~((1 << CS02) | (1 << CS01) | (1 << CS00));
    TCCR0B |= mask;
}

void enable_timer0_interrupt(tmr0_int_t interrupt)
{
    const uint8_t mask = static_cast<uint8_t>(interrupt);

    TIMSK0 |= mask;
}

void disable_timer0_interrupt(tmr0_int_t interrupt)
{
    const uint8_t mask = static_cast<uint8_t>(interrupt);

    TIMSK0 &= ~mask;
}

void enable_timer0(void)
{
    PRR &= ~(1 << PRTIM0);
}

void disable_timer0(void)
{
    PRR |= (1 << PRTIM0);
}
