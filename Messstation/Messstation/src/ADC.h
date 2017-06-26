/*
 * IncFile1.h
 *
 * Created: 03.05.2017 18:39:38
 *  Author: Felix
 */ 

#include <avr/pgmspace.h>

#ifndef ADC_H_
#define ADC_H_

typedef enum ADC_Channel_t {ADC_CH0,ADC_CH1,ADC_CH2,ADC_CH3} ADC_Channel_t;

uint16_t ADCA_GetValue(ADC_Channel_t channel,ADC_CH_MUXPOS_t mux);
void ADC_Init(void);

#endif /* ADC_H_ */