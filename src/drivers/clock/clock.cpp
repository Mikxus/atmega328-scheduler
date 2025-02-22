#include "clock.h"

volatile uint64_t timer0_overflow_counter = 0;
volatile uint64_t timer0_ms = 0;
volatile uint64_t timer0_fractional_ms = 0;

ISR(TIMER0_OVF_vect)
{
    // copy these to local variables so they can be stored in registers
	// (volatile variables must be read from memory on every access)
	unsigned long m = timer0_ms;
	unsigned char f = timer0_fractional_ms;

	m += MILLIS_INC;
	f += FRACT_INC;
	if (f >= FRACT_MAX) {
		f -= FRACT_MAX;
		m += 1;
	}

	timer0_fractional_ms = f;
	timer0_ms = m;
	timer0_overflow_counter++;
}

void initialize_clock(void)
{
    uint8_t sreg = SREG;

    cli();
    /* Set CTC mode
     * Allows to use OCR0A as top value
     */
    timer0_set_waveform_generation_mode(CTC);

    /*
     * 16,000,000 / (2 * prescaler (1+OCR0A))    
     *  
     */
    timer0_set_clock_source(PRESCALER_64);

    OCR0A = 0xFF;


    SREG = sreg;
}


