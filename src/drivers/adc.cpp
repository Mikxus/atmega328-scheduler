#include <kernel/drivers/adc.h>
#include <kernel/atomic.h>
#include <avr/sfr_defs.h> 

void adc_set_power(bool state) {
    if (state) {
        PRR &= ~(1 << PRADC);
    } else {
        PRR |= (1 << PRADC);
    }
}

void adc_set_enable(bool state) {
    ADCSRA = (ADCSRA & ~(1 << ADEN)) | (state << ADEN);
}


void adc_set_vref(adc_vref reference) {
    uint8_t mask = static_cast<uint8_t>(reference);

    ADMUX = (ADMUX & ~((1 << REFS1) | (1 << REFS0))) | mask;
}

void adc_set_auto_trig(bool state) {
    if (state) {
        ADCSRA |= (1 << ADATE);
    } else {
        ADCSRA &= ~(1 << ADATE);
    }
}

void adc_set_auto_trig_src(adc_auto_trig_src source) {
    uint8_t mask = static_cast<uint8_t>(source);

    const uint8_t clr_mask = ~((1 << ADTS2) | (1 << ADTS1) | (1 << ADTS0));

    ADCSRB = (ADCSRB & clr_mask) | mask;
}

void adc_set_channel(adc_ch channel) {
    uint8_t mask = static_cast<uint8_t>(channel);

    ADMUX = (ADMUX & ~0xF) | mask;
}

void adc_set_dbuffer(adc_ch channel, bool state) {
    uint8_t pin = static_cast<uint8_t>(channel);

    /* Only 0 - 5 pins have digital buffer */
    if (pin > 5) 
        return;
    
    if (state) {
        DIDR0 &= ~(1 << pin);
    } else {
        DIDR0 |= (1 << pin);
    }
}

void adc_set_prescaler(adc_prescaler scale) {
    uint8_t mask = static_cast<uint8_t>(scale);

    ADCSRA = (ADCSRA & ~0x7) | mask;
}


void adc_set_8bit_mode(bool state) {
    if (state) {
        ADMUX |= (1 << ADLAR);
    } else {
        ADMUX &= ~(1 << ADLAR);
    }
}

void adc_set_interrupt(bool state) {
    if (state) {
        ADCSRA |= (1 << ADIE);
    } else {
        ADCSRA &= ~(1 << ADIE);
    }
}

uint16_t adc_read(adc_ch channel) {
    ATOMIC_BLOCK();
    adc_set_power(true);
    adc_set_prescaler(adc_prescaler::PRESCALER_128);
    adc_set_channel(channel);
    adc_set_8bit_mode(false);
    adc_set_dbuffer(channel, false);
    adc_set_enable(true);
    
    /* Start adc conversion */
    ADCSRA |= (1 << ADSC);

    loop_until_bit_is_clear(ADCSRA, ADSC);
    adc_set_dbuffer(channel, true);
    return ADC;
}