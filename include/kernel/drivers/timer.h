#ifndef _TIMER_H_
#define _TIMER_H_

#include <avr/io.h>
#include <inttypes.h>

#define clock_cycles_per_us() ( F_CPU / 1000000L )
#define clock_cycles_to_us(a) ( (a) / clock_cycles_per_us() )
// the prescaler is set so that timer0 ticks every 64 clock cycles, and the
// the overflow handler is called every 256 ticks.
#define MICROSECONDS_PER_TIMER0_OVERFLOW (clock_cycles_to_us(64 * 256))

// the whole number of milliseconds per timer0 overflow
#define MS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)

// the fractional number of milliseconds per timer0 overflow. we shift right
// by three to fit these numbers into a byte. (for the clock speeds we care
// about - 8 and 16 MHz - this doesn't lose precision.)
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)

// assertions to ensure that WGM bits match between timers 0 and 2
static_assert(WGM20 == WGM00, "ERROR WGM20 and WGM00 bit mismatch");
static_assert(WGM21 == WGM01, "ERROR WGM21 and WGM01 bit mismatch");
static_assert(WGM22 == WGM02, "ERROR WGM22 and WGM02 bit mismatch");

/**
 * @brief Timer0 Waveform Generation Modes
 * 
 */
enum class tmr0_wvf_gen_mode_t : uint8_t
{
    // top 0xff
    NORMAL = 0,
    // top 0xff
    PWM_PHASE_CORRECT =     (0 << WGM02) | (0 << WGM01) | (1 << WGM00),
    // top OCRA. clear timer on compare match
    CTC =                   (0 << WGM02) | (1 << WGM01) | (0 << WGM00),
    // top 0xff
    FAST_PWM =              (0 << WGM02) | (1 << WGM01) | (1 << WGM00),
    // top OCRA
    PWM_PHASE_CORRECT_OCRA =(1 << WGM02) | (0 << WGM01) | (1 << WGM00),
    // top OCRA
    FAST_PWM_OCRA =         (1 << WGM02) | (1 << WGM01) | (1 << WGM00)
};

/**
 * @brief Timer 0 compare units
 */
enum tmr0_cmp_unit_t : uint8_t
{
    A = 0,
    B = 1
};

/**
 * @brief Timer 0 interrupts 
 */
enum class tmr0_int_t : uint8_t
{
    COMPA_INTERRUPT =       (1 << OCIE0A),
    COMPB_INTERRUPT =       (1 << OCIE0B),
    OVERFLOW_INTERRUPT =    (1 << TOIE0)
};

/**
 * @brief Timer 0 clock source
 */
enum class tmr0_clk_src_t : uint8_t
{
   NO_CLOCK_SOURCE = 0,
   NO_PRESCALER =                   (0 << CS02) | (0 << CS01) | (1 << CS00),
   PRESCALER_8 =                    (0 << CS02) | (1 << CS01) | (0 << CS00),
   PRESCALER_64 =                   (0 << CS02) | (1 << CS01) | (1 << CS00),
   PRESCALER_256 =                  (1 << CS02) | (0 << CS01) | (0 << CS00),
   PRESCALER_1024 =                 (1 << CS02) | (0 << CS01) | (1 << CS00),
   EXTERNAL_CLOCK_SOURCE_FALLING =  (1 << CS02) | (1 << CS01) | (0 << CS00),
   EXTERNAL_CLOCK_SOURCE_RISING =   (1 << CS02) | (1 << CS01) | (1 << CS00) 
};

/**
 * @brief Timer 2 clock select  
 */
enum class tmr2_clk_src_t : uint8_t
{
   NO_CLOCK_SOURCE = 0,
   NO_PRESCALER =                   (0 << CS22) | (0 << CS21) | (1 << CS20),
   PRESCALER_8 =                    (0 << CS22) | (1 << CS21) | (0 << CS20),
   PRESCALER_32 =                   (0 << CS22) | (1 << CS21) | (1 << CS20),
   PRESCALER_64 =                   (1 << CS22) | (0 << CS21) | (0 << CS20),
   PRESCALER_128 =                  (1 << CS22) | (0 << CS21) | (1 << CS20),
   PRESCALER_256 =                  (1 << CS22) | (1 << CS21) | (0 << CS20),
   PRESCALER_1024 =                 (1 << CS22) | (1 << CS21) | (1 << CS20),
};

/**
 * @brief  Set waveform generation mode for timer0
 * @note   Affects both OC0A and OC0B comparators
 * @param  mode: 
 * @retval None
 */
void timer0_set_waveform_generation_mode(tmr0_wvf_gen_mode_t mode);

/**
 * @brief  Sets timer0's clock source
 * @note   
 * @param  clock_source: 
 * @retval None
 */
void timer0_set_clock_source(tmr0_clk_src_t clock_source);

/**
 * @brief  Enable timer0 interrupt
 * @note   
 * @param  interrupt: 
 * @retval None
 */
void enable_timer0_interrupt(tmr0_int_t interrupt);

/**
 * @brief  Disable timer0 interrupt
 * @note   
 * @param  interrupt: 
 * @retval None
 */
void disable_timer0_interrupt(tmr0_int_t interrupt);

/**
 * @brief  Enable timer0 
 * @note   
 * @retval None
 */
void enable_timer0(void);

/**
 * @brief  Disable timer0
 * @note   
 * @retval None
 */
void disable_timer0(void);

#endif