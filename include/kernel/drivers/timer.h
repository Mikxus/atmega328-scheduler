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
 * @brief Timer 1 waveform generation modes.  
 */
enum class tmr1_wvf_gen_mode : uint8_t
{
    // top 0xFFFF
    NORMAL = 0,

    // top 0xFF
    PWM_PHC_8BIT    = (0 << WGM13) | (0 << WGM12) | (0 << WGM11) | (1 << WGM10),
    // top 0x1FF
    PWM_PHC_9BIT    = (0 << WGM13) | (0 << WGM12) | (1 << WGM11) | (0 << WGM10),
    // top 0x3FF
    PWM_PHC_10BIT   = (0 << WGM13) | (0 << WGM12) | (1 << WGM11) | (1 << WGM10),

    // top OCR1A
    CTC_OCR1A       = (0 << WGM13) | (1 << WGM12) | (0 << WGM11) | (0 << WGM10),

    // top 0xFF
    FAST_PWM_8BIT   = (0 << WGM13) | (1 << WGM12) | (0 << WGM11) | (1 << WGM10),
    // top 0x1FF
    FAST_PWM_9BIT   = (0 << WGM13) | (1 << WGM12) | (1 << WGM11) | (0 << WGM10),
    // top 0x3FF
    FAST_PWM_10BIT  = (0 << WGM13) | (1 << WGM12) | (1 << WGM11) | (1 << WGM10),

    // Phase & frequency correct. top ICR1
    PWM_PHFC_ICR1   = (1 << WGM13) | (0 << WGM12) | (0 << WGM11) | (0 << WGM10),
    // Phase & frequency correct. top OCR1A
    PWM_PHFC_OCR1A  = (1 << WGM13) | (0 << WGM12) | (0 << WGM11) | (1 << WGM10),

    // Phase correct. top ICR1
    PWM_PHC_ICR1    = (1 << WGM13) | (0 << WGM12) | (1 << WGM11) | (0 << WGM10),
    // Phase correct. top OCR1A
    PWM_PHC_OCR1A   = (1 << WGM13) | (0 << WGM12) | (1 << WGM11) | (1 << WGM10),

    // top ICR1
    CTC_ICR1        = (1 << WGM13) | (1 << WGM12) | (0 << WGM11) | (0 << WGM10),

    // top ICR1A
    FAST_PWM_ICR1A  = (1 << WGM13) | (1 << WGM12) | (1 << WGM11) | (0 << WGM10),
    // top OCR1A
    FAST_PWM_OCR1A  = (1 << WGM13) | (1 << WGM12) | (1 << WGM11) | (1 << WGM10)
};

/**
 * @brief Timer 2 waveform generation modes.  
 */
enum class tmr2_wvf_gen_mode : uint8_t
{
    // top 0xff
    NORMAL = 0,
    // top 0xff
    PWM_PHASE_CORRECT =     (0 << WGM22) | (0 << WGM21) | (1 << WGM20),
    // top OCRA. clear timer on compare match
    CTC =                   (0 << WGM22) | (1 << WGM21) | (0 << WGM20),
    // top 0xff
    FAST_PWM =              (0 << WGM22) | (1 << WGM21) | (1 << WGM20),
    // top OCRA
    PWM_PHASE_CORRECT_OCRA =(1 << WGM22) | (0 << WGM21) | (1 << WGM20),
    // top OCRA
    FAST_PWM_OCRA =         (1 << WGM22) | (1 << WGM21) | (1 << WGM20)
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

enum class tmr1_int : uint8_t
{
    COMPA_INT =             (1 << OCIE1A),
    COMPB_INT =             (1 << OCIE1B),
    OVERFLOW_INT =          (1 << TOIE1),
    CAPTURE_INT =           (1 << ICIE1)
};

/**
 * @brief Timer 2 interrupts 
 */
enum class tmr2_int : uint8_t
{
    COMPA_INTERRUPT =       (1 << OCIE2A),
    COMPB_INTERRUPT =       (1 << OCIE2B),
    OVERFLOW_INTERRUPT =    (1 << TOIE2)
};

/**
 * @brief Timer 0 clock source.
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
 * @brief Timer 1 clock select.
 */
enum class tmr1_clk_src : uint8_t
{
    NO_CLOCK_SOURCE = 0,
    NO_PRESCALER =                  (0 << CS12) | (0 << CS11) | (1 << CS10),
    PRESCALER_8 =                   (0 << CS12) | (1 << CS11) | (0 << CS10),
    PRESCALER_64 =                  (0 << CS12) | (1 << CS11) | (1 << CS10),
    PRESCALER_256 =                 (1 << CS12) | (0 << CS11) | (0 << CS10),
    PRESCALER_1024 =                (1 << CS12) | (0 << CS11) | (1 << CS10),
    EXT_PIN_FALL =                  (1 << CS12) | (1 << CS11) | (0 << CS10),
    EXT_PIN_RIS =                   (1 << CS12) | (1 << CS11) | (1 << CS10),
};

/**
 * @brief Timer 2 clock select. 
 */
enum class tmr2_clk_src : uint8_t
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
 * @brief  Set waveform generation mode for timer1
 * @param  mode: 
 * @retval None
 */
void tmr1_set_wvf_gen(tmr1_wvf_gen_mode mode);

/**
 * @brief  Set waveform generation mode for timer2
 * @note   Affects both OC2A and OC2B comparators
 * @retval None
 */
void tmr2_set_wvf_gen(tmr2_wvf_gen_mode mode);

/**
 * @brief  Sets timer0's clock source
 * @param  clock_source: 
 * @retval None
 */
void timer0_set_clock_source(tmr0_clk_src_t clock_source);

/**
 * @brief Set timer1 clock source.  
 * @param  clock_source: clock source
 * @retval None
 */
void tmr1_set_clk_src(tmr1_clk_src clock_source);

/**
 * @brief  Sets timer2's clock source
 * @param  clock_source: 
 * @retval None
 */
void tmr2_set_clk_src(tmr2_clk_src clock_source);

/**
 * @brief  Enable timer0 interrupt
 * @param  interrupt: 
 * @retval None
 */
void enable_timer0_interrupt(tmr0_int_t interrupt);

/**
 * @brief Enable or disable timer1 interrupt.
 * @param  interrupt: Specific interrupt. 
 * @param  state: TRUE: Enabled, FALSE: Disable.
 * @retval None
 */
void tmr1_set_int(tmr1_int interrupt, bool state);

/**
 * @brief Enable or disable timer2 interrupt.
 * @param  interrupt: Specific interrupt. 
 * @param  state: TRUE: Enabled, FALSE: Disable.
 * @retval None
 */
void tmr2_set_int(tmr2_int interrupt, bool state);

/**
 * @brief Set timer1 power state.
 * @param  state: TRUE: Enabled, FALSE: Disable.
 * @retval None
 */
void tmr1_set_power(bool state);

/**
 * @brief Set timer2 power state.
 * @param  state: TRUE: Enabled, FALSE: Disable.
 * @retval None
 */
void tmr2_set_power(bool state);

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