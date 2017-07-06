/*
 * WRL_Sensorknoten.c
 *
 * Created: 30.06.2017 16:25:33
 * Author : Stud
 */ 

#include "Lichtsensor/BH1750.h"
#include "Lichtsensor/com.h"
#include "Lichtsensor/i2c.h"
#include "ADC/adc.h"
#include <avr/io.h>
#include <util/delay.h>
#include <asf.h>


int main(void)
{
	uint16_t adcwert = 0x00;
	uint8_t test = 0x00;
	
	sysclk_init();
	
	adc_init();
	i2c_init();
	i2c_enable();
    while (1) 
    {
		//adcwert = adc_getvalue(&(ADCA.CH0),0);
		//_delay_ms(200);
		//BH1750_read(&test);
		_delay_ms(200);
    }
}

