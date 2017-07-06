/*
 * i2c_eeprom.c
 *
 * Created: 11.05.2017 08:41:07
 *  Author: Stud
 */ 

#define DUMMY 0x00
#define CHIPSELECT 0x10
#define PININIT 0xB0

#define  SLAVE_ADDRESS 0xE2

void twi_senddata(void);

#include "spi_wind.h"
#include "twi_slave_driver.h"

TWI_Slave_t twiSlave;

 void spi_wind_masterinit()
{				
	PORTC.DIRSET = PININIT;															
	PORTC.OUTSET = CHIPSELECT;												
	SPIC.CTRL |= SPI_ENABLE_bm | SPI_MASTER_bm | SPI_PRESCALER_DIV4_gc | SPI_MODE_1_gc;		
	
	PORTE.PIN0CTRL |= PORT_OPC_PULLUP_gc;		// Pullup für SDA
	PORTE.PIN1CTRL |= PORT_OPC_PULLUP_gc;		// Pullup für SCL
	
	TWI_SlaveInitializeDriver(&twiSlave, &TWIE, twi_senddata);
	TWI_SlaveInitializeModule(&twiSlave, SLAVE_ADDRESS, TWI_SLAVE_INTLVL_HI_gc);

	PMIC.CTRL |= PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm;
	sei();
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

uint8_t test = 0x00;
volatile int16_t prewinkel;
volatile int16_t curwinkel;
int16_t winkeldifferenz = 0;
float timervalue = 0.01;
float radius = 0.022;
int16_t windgeschwindigkeit = 0;
int16_t spi_windspeed()
{
	prewinkel = spi_get_angle();
	_delay_ms(10);
	curwinkel = spi_get_angle();
	winkeldifferenz = spi_angledif(prewinkel,curwinkel);
	windgeschwindigkeit = ((winkeldifferenz)/(timervalue)*radius);				// Einheit und Wert passen nicht
	windgeschwindigkeit = (windgeschwindigkeit * -1)/3.6;
	
	twiSlave.sendData[0] = test;
	test = test + 0x01;
	twiSlave.sendData[1] = 0x34;
	
	//twiSlave.sendData[0] = (uint8_t)(windgeschwindigkeit>>8);
	//twiSlave.sendData[1] = (uint8_t)(windgeschwindigkeit);
	
	return windgeschwindigkeit;
}

void twi_senddata(void)
{
	twiSlave.sendData[0] = 0x00;
	twiSlave.sendData[1] = 0x00;
}

ISR(TWIE_TWIS_vect)
{
	TWI_SlaveInterruptHandler(&twiSlave);
}