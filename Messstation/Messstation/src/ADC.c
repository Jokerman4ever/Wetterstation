/*
 * ADC.c
 *
 * Created: 03.05.2017 17:42:13
 *  Author: Felix
 */ 
#include "ADC.h"
#include <ASF/common/services/clock/sysclk.h>


 void ADC_Init()
 {
	sysclk_enable_module(SYSCLK_PORT_A, SYSCLK_ADC);

	ADCA.CTRLB = ADC_RESOLUTION_12BIT_gc;
	ADCA.REFCTRL = ADC_REFSEL_INT1V_gc;
	ADCA.PRESCALER = ADC_PRESCALER_DIV16_gc;

	sysclk_disable_module(SYSCLK_PORT_A, SYSCLK_ADC);
 }

 uint16_t ADCA_GetValue(ADC_Channel_t channel,ADC_CH_MUXPOS_t mux)
 {
	sysclk_enable_module(SYSCLK_PORT_A, SYSCLK_ADC);
	uint16_t res = 0;
	uint8_t regsave = PORTA.DIR;
	switch(mux)
	{
		case ADC_CH_MUXPOS_PIN0_gc: PORTA.DIRCLR = (1 << 0); break;
		case ADC_CH_MUXPOS_PIN1_gc: PORTA.DIRCLR = (1 << 1); break;
		case ADC_CH_MUXPOS_PIN2_gc: PORTA.DIRCLR = (1 << 2); break;
		case ADC_CH_MUXPOS_PIN3_gc: PORTA.DIRCLR = (1 << 3); break;
		case ADC_CH_MUXPOS_PIN4_gc: PORTA.DIRCLR = (1 << 4); break;
		case ADC_CH_MUXPOS_PIN5_gc: PORTA.DIRCLR = (1 << 5); break;
		case ADC_CH_MUXPOS_PIN6_gc: PORTA.DIRCLR = (1 << 6); break;
		case ADC_CH_MUXPOS_PIN7_gc: PORTA.DIRCLR = (1 << 7); break;
		default: break;
	}
	switch(channel)
	{
		case ADC_CH0:
		{
			ADCA.CH0.MUXCTRL =  mux;
			ADCA.CTRLA = ADC_ENABLE_bm;
			ADCA.CH0.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;
			ADCA.CH0.CTRL |= ADC_CH_START_bm;//Start Conversion
			while(!ADCA.CH0.INTFLAGS);
			res = ADCA.CH0RES;
			break;
		}
		case ADC_CH1:
		{
			ADCA.CH1.MUXCTRL =  mux;
			ADCA.CTRLA = ADC_ENABLE_bm;
			ADCA.CH1.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;
			ADCA.CH1.CTRL |= ADC_CH_START_bm;//Start Conversion
			while(!ADCA.CH1.INTFLAGS);
			res = ADCA.CH1RES;
			break;
		}
		case ADC_CH2:
		{
			ADCA.CH2.MUXCTRL =  mux;
			ADCA.CTRLA = ADC_ENABLE_bm;
			ADCA.CH2.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;
			ADCA.CH2.CTRL |= ADC_CH_START_bm;//Start Conversion
			while(!ADCA.CH2.INTFLAGS);
			res = ADCA.CH2RES;
			break;
		}
		case ADC_CH3:
		{
			ADCA.CH3.MUXCTRL =  mux;
			ADCA.CTRLA = ADC_ENABLE_bm;
			ADCA.CH3.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;
			ADCA.CH3.CTRL |= ADC_CH_START_bm;//Start Conversion
			while(!ADCA.CH3.INTFLAGS);
			res = ADCA.CH3RES;
			break;
		}
		default:break;
	}
	PORTA.DIR = regsave;
	sysclk_disable_module(SYSCLK_PORT_A, SYSCLK_ADC);
	return res;
 }