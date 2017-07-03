#ifndef ADCCONFIG_H
#define ADCCONFIG_H

#include <avr/io.h>

void adc_init();
uint16_t adc_getvalue(ADC_CH_t *Channel, char Pin);
void adc_ident(uint16_t adcerg);

#endif