/*
 * Windsensor.c
 *
 * Created: 22.06.2017 14:26:11
 * Author : Stud
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include "spi_wind.h"
#include "twi_wind.h"
#include "clock.h"

int main(void)
{
	clock_init();
	spi_wind_masterinit();
	timer_init();
	//twi_wind_init();
	
    while (1) 
    {
		
    }
}

void timer_init()
{
	TCC0.CTRLA = TC_CLKSEL_DIV1024_gc;
	TCC0.CTRLB = TC_WGMODE_NORMAL_gc;
	TCC0.PER = 39;							// Timertakt = (Cpu / (Prescaler*Per))  Cpu=4 MHz  ; Prescaler=1024 ; Per = 3600  => 1 Hz => 1 x pro sekunde
	TCC0.CNT = 0x00;
	TCC0.INTCTRLA = 0x03;
	PMIC.CTRL |= PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm; 
	sei();
}

uint8_t cnt = 0;
volatile int16_t prewinkel;
volatile int16_t curwinkel;
int16_t winkeldifferenz = 0;
float timervalue = 0.01;
float radius = 0.022;
float windgeschwindigkeit = 0;   
ISR(TCC0_OVF_vect)
{

	if (cnt==0)
	{
		prewinkel = spi_get_angle();
		cnt++;
	}
	else
	{
		curwinkel = spi_get_angle();
		cnt = 0;
		winkeldifferenz = spi_angledif(prewinkel,curwinkel);
		windgeschwindigkeit = ((winkeldifferenz)/(timervalue)*radius);				// Einheit und Wert passen nicht
		windgeschwindigkeit = (windgeschwindigkeit * -1)/3.6;						// Kommt in etwa hin Zufall
	}
}


