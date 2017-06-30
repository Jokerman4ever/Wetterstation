/*
 * Versuch1.c
 *
 * Created: 06.05.2017 16:05:39
 * Author : dennis.michalik
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include "led/led.h"
#include "key/key.h"
#include "adc/adc.h"
#include "com/com.h"

int main(void)

{	
	com_init();
	port_init();
	key_init();
	adc_init();
	
	volatile uint16_t adcvalue = 0;
	
    while (1) 
    {
		
		adcvalue = adc_getvalue(&(ADCA.CH0),3);
		printf("%s","ADC-Wert: ");
		printf("%d",adcvalue);
		printf("%s\n\r","");
		
		printf("%s","Counter-Wert: ");
		printf("%d",getkeycnt());
		printf("%s\n\r\n\r","");
		
		_delay_ms(1500);
		
    }
}


