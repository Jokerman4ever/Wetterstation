/*
 * i2c_eeprom.c
 *
 * Created: 11.05.2017 08:41:07
 *  Author: Stud
 */ 

#define DUMMY 0x00
#define CHIPSELECT 0x10
#define PININIT 0xB0

#include "spi_wind.h"

 void spi_wind_masterinit()
{				
	PORTC.DIRSET = PININIT;															
	PORTC.OUTSET = CHIPSELECT;												
	SPIC.CTRL |= SPI_ENABLE_bm | SPI_MASTER_bm | SPI_PRESCALER_DIV4_gc | SPI_MODE_1_gc;		// Eventuell Prescaler niedriger -> 1 MHZ Clock
}

uint16_t spi_wind_read()
{
	uint8_t test;
	uint8_t datahigh = 0x13;
	uint8_t datalow = 0x37;
	PORTC.OUTTGL = CHIPSELECT;	
	_delay_us(1);	
	test=SPIC.STATUS; 				
	SPIC.DATA=DUMMY;
	while(!(SPIC.STATUS & SPI_IF_bm));
	datahigh = SPIC.DATA;
	test=SPIC.STATUS; 
	SPIC.DATA=DUMMY;
	while(!(SPIC.STATUS & SPI_IF_bm));
	datalow = SPIC.DATA;
	_delay_us(1);
	PORTC.OUTTGL = CHIPSELECT;
	return ((datahigh<<8) | datalow);
}

float spi_get_angle()						
{
	uint16_t anglehex = 0x00;
	float angledec = 0x00;
	uint8_t test;
	uint8_t datahigh = 0x13;
	uint8_t datalow = 0x37;
	PORTC.OUTTGL = CHIPSELECT;
	_delay_us(1);
	test=SPIC.STATUS;
	SPIC.DATA=DUMMY;
	while(!(SPIC.STATUS & SPI_IF_bm));
	datahigh = SPIC.DATA;
	test=SPIC.STATUS;
	SPIC.DATA=DUMMY;
	while(!(SPIC.STATUS & SPI_IF_bm));
	datalow = SPIC.DATA;
	_delay_us(1);
	PORTC.OUTTGL = CHIPSELECT;
	anglehex = ((datahigh<<2) | (datalow>>6));
	angledec = (((float)anglehex/(float)1024)*(float)360);
	return angledec;
}

int16_t differenz = -360;
int16_t spi_angledif(int16_t pre, int16_t cur)
{
	int16_t differenz = (cur-pre);

	if (differenz>180)
	{
		differenz = (differenz - 360) % 360;
		return differenz;
	}
	else if (differenz<-180)
	{
		differenz = (differenz + 360) % 360;
		return differenz;
	}
	else
	{
		return differenz;
	}
}
