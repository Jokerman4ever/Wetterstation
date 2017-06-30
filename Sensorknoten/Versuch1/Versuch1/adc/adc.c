#include "adc.h"

void adc_init()
{
	ADCA.CTRLB = ADC_RESOLUTION_12BIT_gc;
	ADCA.REFCTRL = ADC_REFSEL_VCC_gc;
	ADCA.PRESCALER = ADC_PRESCALER_DIV16_gc,
	ADCA.CTRLA = ADC_ENABLE_bm;
}

uint16_t adc_getvalue(ADC_CH_t *Channel, char Pin)
{
	Channel->CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;
	Channel->MUXCTRL = (Pin<<3);
	Channel->CTRL |= ADC_CH_START_bm;
	while(!Channel->INTFLAGS);
	return Channel->RES;
}
