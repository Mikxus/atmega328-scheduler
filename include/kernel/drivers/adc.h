/**
 * @file adc.h
 * @brief Simple adc utilities. 
 * 
 */
#ifndef _ADC_H_
#define _ADC_H_

#include <avr/io.h>
#include <inttypes.h>

/* 
 * Adc voltage references.
 */
enum class adc_vref : uint8_t 
{
	/**  AREF, internal Vref turned off. */
	AREF		= (0 << REFS1) | (0 << REFS0),
	/** AVcc with external cap */
	AVCC		= (0 << REFS1) | (1 << REFS0),
	/** Internal 1.1 V reference */
	INTERNAL	= (1 << REFS1) | (1 << REFS0)
};

/**
 * @brief Adc channel selections.  
 */
enum class adc_ch : uint8_t
{
	ADC0	= (0 << MUX3) | (0 << MUX2) | (0 << MUX1) | (0 << MUX0),
	ADC1	= (0 << MUX3) | (0 << MUX2) | (0 << MUX1) | (1 << MUX0),
	ADC2	= (0 << MUX3) | (0 << MUX2) | (1 << MUX1) | (0 << MUX0),
	ADC3	= (0 << MUX3) | (0 << MUX2) | (1 << MUX1) | (1 << MUX0),
	ADC4	= (0 << MUX3) | (1 << MUX2) | (0 << MUX1) | (0 << MUX0),
	ADC5	= (0 << MUX3) | (1 << MUX2) | (0 << MUX1) | (1 << MUX0),
	ADC6	= (0 << MUX3) | (1 << MUX2) | (1 << MUX1) | (0 << MUX0),
	ADC7	= (0 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0),
	TEMP	= (1 << MUX3) | (0 << MUX2) | (0 << MUX1) | (0 << MUX0),
	/**  1.1 V (Vbg) */
	ONE		= (1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (0 << MUX0),
	/**  Ground reference */
	GND		= (1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0)
};

/**
 * @brief Adc auto trigger source.
 */
enum class adc_auto_trig_src : uint8_t {
	FREE_RUN		= (0 << ADTS2) | (0 << ADTS1) | (0 << ADTS0),
	AN_CMP			= (0 << ADTS2) | (0 << ADTS1) | (1 << ADTS0),
	EXT_INT_REQ0	= (0 << ADTS2) | (1 << ADTS1) | (0 << ADTS0),
	TMR0_COMPA		= (0 << ADTS2) | (1 << ADTS1) | (1 << ADTS0),
	TMR0_OVF		= (1 << ADTS2) | (0 << ADTS1) | (0 << ADTS0),
	TMR1_COMPB		= (1 << ADTS2) | (0 << ADTS1) | (1 << ADTS0),
	TMR1_OVF		= (1 << ADTS2) | (1 << ADTS1) | (0 << ADTS0),
	TMR1_CAPT		= (1 << ADTS2) | (1 << ADTS1) | (1 << ADTS0)
};

/**
 * @brief Adc prescaler.
 */
enum class adc_prescaler : uint8_t {
	PRESCALER_2 	= (0 << ADPS2) | (0 << ADPS1) | (1 << ADPS0),
	PRESCALER_4 	= (0 << ADPS2) | (1 << ADPS1) | (0 << ADPS0),
	PRESCALER_8 	= (0 << ADPS2) | (1 << ADPS1) | (1 << ADPS0),
	PRESCALER_16 	= (1 << ADPS2) | (0 << ADPS1) | (0 << ADPS0),
	PRESCALER_32 	= (1 << ADPS2) | (0 << ADPS1) | (1 << ADPS0),
	PRESCALER_64 	= (1 << ADPS2) | (1 << ADPS1) | (0 << ADPS0),
	PRESCALER_128 	= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0),
};

/**
 * @brief Set adc pheriphals power state.  
 * @param  state: Logical one enables.
 * @retval None
 */
void adc_set_power(bool state);

/**
 * @brief Set Adc's enable bit.
 * @param  state: 
 * @retval None
 */
void adc_set_enable(bool state);

/**
 * @brief Set ADC's auto trigger state.  
 *		Enabling auto trigger adc will start conversion on a positive edge of
 *		selected trigger source.		
 * 
 * @param  state: 
 * @retval None
 */
void adc_set_auto_trig(bool state);

/**
 * @brief Set ADC's auto trigger source.  
 * @param  source: 
 * @retval None
 */
void adc_set_auto_trig_src(adc_auto_trig_src source);

/**
 * @brief Set ADC's voltage reference.  
 * @retval None
 */
void adc_set_vref(adc_vref reference);

/**
 * @brief Set ADC's mux to specific channel.  
 * @note  To change adc mux the following apply:
 * 			- ADC must be disabled adc_set_enable(false) 
 * 
 * @param  channel: 
 * @retval None
 */
void adc_set_channel(adc_ch channel);

/**
 * @brief Set digital buffer state  
 * @note   
 * @param  channel: A0 - A5
 * @retval None
 */
void adc_set_dbuffer(adc_ch channel, bool state);

/**
 * @brief Set ADC's prescaler  
 * @param  scale: 
 * @retval None
 */
void adc_set_prescaler(adc_prescaler scale);

/**
 * @brief Set ADC to left adjust the ADCL & ADCH registers.  
 * 
 * @param state: TRUE: 8bit mode. FALSE: 10 bit mode.
 * @retval None
 */
void adc_set_8bit_mode(bool state);

/**
 * @brief Set ADC to interrupt on finished conversion. 
 * 
 * @details	When adc conversion is finished and global interrupts are enabled
 * 			the ADC_vect interrupt is executed.
 * 
 * @param state: TRUE: Int enabled. FALSE: Int disabled
 * @retval None
 */
void adc_set_interrupt(bool state);

/**
 * @brief Start adc conversion and return result.
 *
 * @note 	- Doesn't restore adc state.
 * 			- Waits until conversion finishes. (60us to 250us)
 * 			- The corresponding IO_PORTC pin must be input without pullups.
 * 
 * @param  channel: 
 * @return 10 bit conversion result.
 */
uint16_t adc_read(adc_ch channel);

#endif  // _ADC_H_