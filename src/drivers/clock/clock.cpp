#include "clock.h"

volatile uint32_t timer0_overflow_count = 0;
volatile uint32_t timer0_ms = 0;
volatile uint8_t timer0_fractional_ms = 0;

ISR(TIMER0_OVF_vect)
{
    // copy these to local variables so they can be stored in registers
	// (volatile variables must be read from memory on every access)
	uint32_t ms = timer0_ms;
	uint8_t fract = timer0_fractional_ms;

	ms += MS_INC;
	fract += FRACT_INC;
	if (fract >= FRACT_MAX) {
		fract -= FRACT_MAX;
		ms += 1;
	}

	timer0_fractional_ms = fract;
	timer0_ms = ms;
	timer0_overflow_count++;
}

uint32_t get_ms(void)
{
    uint32_t ms;
    uint8_t sreg = SREG;
    
    cli();
    ms = timer0_ms;
    SREG = sreg;
    return ms;
}

uint32_t get_us(void)
{
	unsigned long ms;
	uint8_t sreg = SREG;
    uint8_t t;
	
	cli();
	ms = timer0_overflow_count;
#if defined(TCNT0)
	t = TCNT0;
#elif defined(TCNT0L)
	t = TCNT0L;
#else
	#error TIMER 0 not defined
#endif

#ifdef TIFR0
	if ((TIFR0 & _BV(TOV0)) && (t < 255))
		ms++;
#else
	if ((TIFR & _BV(TOV0)) && (t < 255))
		m++;
#endif

	SREG = sreg;
	
	return ((ms << 8) + t) * (64 / clock_cycles_per_us());
}

void initialize_clock(void)
{
    uint8_t sreg = SREG;

    cli();
    /* Set CTC mode
     * Allows to use OCR0A as top value
     */
    timer0_set_waveform_generation_mode(NORMAL);

    /*
     * 16,000,000 / (2 * prescaler (1+OCR0A))    
     *  
     */
    timer0_set_clock_source(PRESCALER_64);

    enable_timer0_interrupt(OVERFLOW_INTERRUPT);

    enable_timer0();

    SREG = sreg;
}