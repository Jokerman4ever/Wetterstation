/*
 * Xdelay.c
 *
 * Created: 27.06.2017 12:28:14
 *  Author: Stud
 */ 
#include "Xdelay.h"
uint8_t XDELAY_ISFAST;

void _xdelay_ms(uint16_t delay)
{
	if(XDELAY_ISFAST)
		for (uint8_t c=0; c<XDELAY_SCALEFACTOR; c++)
			for (uint16_t i=0; i<delay; i++)_delay_ms(1);
			
	for (uint16_t z=0; z<delay; z++)_delay_ms(1);
}

void _xdelay_us(uint16_t delay)
{
	if(XDELAY_ISFAST)
		for (uint8_t c=0; c<XDELAY_SCALEFACTOR; c++)
			for (uint16_t i=0; i<delay; i++)_delay_us(1);
	
	uint16_t z=0;		
	for (; z<delay; z++)_delay_us(1);
}
