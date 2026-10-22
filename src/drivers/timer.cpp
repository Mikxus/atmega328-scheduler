#include <kernel/drivers/timer.h>

void timer0_set_waveform_generation_mode(tmr0_wvf_gen_mode_t mode)
{
    const uint8_t mask = static_cast<uint8_t>(mode);
    /*
     * Waveform generation mode is split between TCCR0A and TCCR0B registers.
     * TCCR0A: WGM01, WGM00
     * TCCR0B: WGM02
     */
    TCCR0A = (TCCR0A & ~((1 << WGM01) | 1 << WGM00)) | (mask & (1 << WGM01 | 1 << WGM00));
    TCCR0B = (TCCR0B & ~(1 << WGM02)) | (mask & (1 << WGM02));
}

void tmr1_set_wvf_gen(tmr1_wvf_gen_mode mode)
{
    constexpr uint8_t a_mask = (1 << WGM11) | (1 << WGM10);
    constexpr uint8_t b_mask = (1 << WGM13) | (1 << WGM12);
    const uint8_t mask       = static_cast<uint8_t>(mode);

    /*
     * Waveform generation mode is split between TCCR1A and TCCR1B registers.
     * TCCR1A: WGM11, WGM10
     * TCCR1B: WGM13, WMG12
     */
    TCCR1A &= ~a_mask;
    TCCR1B &= ~b_mask;

    TCCR1A |= (mask & a_mask);
    TCCR1B |= (mask & b_mask);
}

void tmr2_set_wvf_gen(tmr2_wvf_gen_mode mode)
{
    const uint8_t mask = static_cast<uint8_t>(mode);

    TCCR2A = (TCCR2A & ~((1 << WGM21) | 1 << WGM20)) | (mask & (1 << WGM21 | 1 << WGM20));
    TCCR2B = (TCCR2B & ~(1 << WGM22)) | (mask & (1 << WGM22));
}

void timer0_set_clock_source(tmr0_clk_src_t clock_source)
{
    const uint8_t mask = static_cast<uint8_t>(clock_source);

    TCCR0B &= ~((1 << CS02) | (1 << CS01) | (1 << CS00));
    TCCR0B |= mask;
}

void tmr1_set_clk_src(tmr1_clk_src clock_source) {
    const uint8_t mask = static_cast<uint8_t>(clock_source);

    TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));
    TCCR1B |= mask;
}

void tmr2_set_clk_src(tmr2_clk_src clock_source)
{
    const uint8_t mask = static_cast<uint8_t>(clock_source);

    TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20));
    TCCR2B |= mask;
}

void enable_timer0_interrupt(tmr0_int_t interrupt)
{
    const uint8_t mask = static_cast<uint8_t>(interrupt);

    TIMSK0 |= mask;
}

void tmr1_set_int(tmr1_int interrupt, bool state) 
{
    const uint8_t mask = static_cast<uint8_t>(interrupt);

    if (state) {
        TIMSK1 |= mask;  
    } else {
        TIMSK1 &= ~mask;
    }
}

void tmr2_set_int(tmr2_int interrupt, bool state) 
{
    const uint8_t mask = static_cast<uint8_t>(interrupt);

    if (state) {
        TIMSK2 |= mask;  
    } else {
        TIMSK2 &= ~mask;
    }
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

void tmr1_set_power(bool state) 
{
    if (state) {
        PRR &= ~(1 << PRTIM1);
    } else {
        PRR |= (1 << PRTIM1);
    }
}

void tmr2_set_power(bool state) 
{
    if (state) {
        PRR &= ~(1 << PRTIM2);
    } else {
        PRR |= (1 << PRTIM2);
    }
}

void disable_timer0(void)
{
    PRR |= (1 << PRTIM0);
}
