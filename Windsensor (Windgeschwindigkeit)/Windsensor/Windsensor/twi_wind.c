/*
 * i2c_eeprom.c
 *
 * Created: 11.05.2017 08:41:07
 *  Author: Stud
 */ 

#include "twi_wind.h"
#include "spi_wind.h"


#define SLAVEADDRESS 0xB6

void twi_wind_init()
{
	PORTE.DIR=0b0000;

	PORTE.PIN0CTRL = (PORTE.PIN0CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc;
	PORTE.PIN1CTRL = (PORTE.PIN1CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc;
	
	TWIE.SLAVE.ADDR = SLAVEADDRESS;
	TWIE.SLAVE.CTRLA |= TWI_SLAVE_INTLVL_MED_gc | TWI_SLAVE_APIEN_bm | TWI_SLAVE_ENABLE_bm;
	
	
	PMIC.CTRL |= PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
	sei();
}

uint16_t angle = 0;
uint8_t daten = 0xAA;
ISR(TWIE_TWIS_vect)
{
	angle = spi_get_angle();
	twi_wind_sendack();
	//twi_wind_send_byte(daten);
}


twi_wind_sendack()
{
	TWIE.SLAVE.CTRLB |= TWI_SLAVE_ACKACT_bm | TWI_SLAVE_CMD_RESPONSE_gc;
}

twi_wind_send_byte(uint8_t data)
{
	TWIE.SLAVE.DATA = data;
	while (!(TWIE.SLAVE.STATUS & TWI_SLAVE_RXACK_bm));
}