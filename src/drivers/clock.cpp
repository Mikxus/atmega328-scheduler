#include <kernel/drivers/clock.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include <kernel/kernel.h>
#include <kernel/drivers/timer.h>
#include <kernel/atomic.h>
#include "scheduling/sched.h"
#include "../task_utils.h"

volatile uint32_t timer0_overflow_count = 0;
volatile uint32_t timer0_ms = 0;
volatile uint8_t timer0_fractional_ms = 0;

ISR(TIMER0_OVF_vect, ISR_BLOCK)
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

    /* Do sleep tick */
    task_data_t * node;
    uint8_t c_priority = 0;
    bool yield_pend    = false;

    node = _sched_lists.sleep.get_head();
    if (node == nullptr) {
        return;
    }

    node->sleep_tick -= 1;

    if (node->sleep_tick == 0) {
        /* Wakeup task */
        _sched_lists.sleep.remove(node);
        _sched_lists.ready.add(node);
        _set_task_state(node, READY);

        if (_get_current_task() != nullptr)
            c_priority = _get_current_task()->priority;
        else
            yield_pend = true;

        if (node->priority > c_priority)
            yield_pend = true;

        node = _sched_lists.sleep.get_head();

        /* Check if other tasks are ready*/
        while (node != nullptr) {
            if (node->sleep_tick != 0) {
                break;
            }

            _sched_lists.sleep.remove(node);
            _sched_lists.ready.add(node);
            _set_task_state(node, READY);

            if (node->priority > c_priority)
                yield_pend = true;

            node = _sched_lists.sleep.get_head();
        }
    }

    if (yield_pend)
        soft_yield();   // using yield() seems to crash TODO: investigate
    
    return;
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

void clock_init(void)
{
    ATOMIC_BLOCK() {
        timer0_set_waveform_generation_mode(tmr0_wvf_gen_mode_t::NORMAL);

        timer0_set_clock_source(tmr0_clk_src_t::PRESCALER_64);

        enable_timer0_interrupt(tmr0_int_t::OVERFLOW_INTERRUPT);

        enable_timer0();
    }
}