/*
 * i2c_eeprom.c
 *
 * Created: 11.05.2017 08:41:07
 *  Author: Stud
 */ 

#include "spi_eeprom.h"
#include <avr/io.h>

 void spi_eeprom_masterinit()
{
	PORTD.DIR = 0x0A;						// PD1 - PD3 als MOSI, MISO, SCK	
	PORTD.PIN2CTRL = PORT_OPC_PULLUP_gc;	// Pullup für Pin D2 MISO 
	PORTC.DIR = 0x01;						// SDA als Chipselect pin
	PORTC.OUTSET = 0x01;					// Chip select auf high	
		
	USARTD0.CTRLC = USART_CMODE_MSPI_gc;		
	USARTD0.BAUDCTRLA = USART_BSEL0_bm;
	USARTD0.CTRLB = USART_TXEN_bm | USART_RXEN_bm;
}

 bool spi_eeprom_write(const uint8_t* data, uint8_t eepromaddr)
{
	PORTC.OUTCLR = 0x01;		// CS wählt den Slave aus, setzen auf low
	USARTD0.STATUS = 0x40;
	USARTD0.DATA = 0x06;			// Write Enable Instruction WREN 
	while (!(USARTD0.STATUS & 0x40));
	USARTD0.STATUS = 0x40;
	USARTD0.DATA = 0x0A;			// Entspricht der Instruction für write nach dem Datenblatt 
	while (!(USARTD0.STATUS & 0x40));
	USARTD0.STATUS = 0x40;
	USARTD0.DATA = eepromaddr;		// Übertragen der Speicheradresse 
	while (!(USARTD0.STATUS & 0x40));
	USARTD0.STATUS = 0x40;
	USARTD0.DATA = data[0];		// Übertragen des ersten Bytes im übergebenen Array
	while (!(USARTD0.STATUS & 0x40));
	USARTD0.STATUS = 0x40;
	PORTC.OUTSET = 0x01;		// CS auf high, dadurch wird der Slave deselektiert
	return true;
}

uint8_t spi_eeprom_read(uint8_t eepromaddr)
{
	uint8_t daten = 0;
	PORTC.OUTCLR = 0x01;		// CS wählt den Slave aus, setzen auf low
	USARTD0.STATUS = 0x40;		// cleart das Interrupt Flag im Register, wird andernfalls nicht durchgeführt	
	USARTD0.DATA = 0x0B;		// Entspricht der Instruktion für read nach dem Datenblatt		
	while (!(USARTD0.STATUS & 0x40));
	USARTD0.STATUS = 0x40;
	USARTD0.DATA = eepromaddr;		
	while (!(USARTD0.STATUS & 0x40));
	USARTD0.STATUS = 0x40;
	USARTD0.DATA = 0x00;				// Dummy		
	while (!(USARTD0.STATUS & 0x40));
	USARTD0.STATUS = 0x40;		
	daten = USARTD0.DATA;
	PORTC.OUTSET = 0x01;		// CS auf high, dadurch wird der Slave deselektiert	
	return daten;
}

uint8_t read_status_reg()
{
	uint8_t statreg = 0;
	PORTC.OUTCLR = 0x01;		// CS wählt den Slave aus, setzen auf low
	USARTD0.STATUS = 0x40;
	USARTD0.DATA = 0x05;		// Entspricht der Instruktion für read Status Register nach dem Datenblatt
	while (!(USARTD0.STATUS & 0x40));
	USARTD0.STATUS = 0x40;
	USARTD0.DATA = 0x00;				// Dummy
	while (!(USARTD0.STATUS & 0x40));
	USARTD0.STATUS = 0x40;
	statreg = USARTD0.DATA;
	PORTC.OUTSET = 0x01;		// CS auf high, dadurch wird der Slave deselektiert
	return statreg;
}

uint8_t send_byte_read_buffer(uint8_t daten)
{
	USARTD0.STATUS = USART_DREIF_bm;
	while (!(USARTD0.STATUS & USART_DREIF_bm));
	USARTD0.DATA = daten;
	USARTD0.STATUS = USART_TXCIF_bm;
	while (!(USARTD0.STATUS & USART_TXCIF_bm));
	
	USARTD0.STATUS = USART_RXCIF_bm;
	while(!(USARTD0.STATUS & USART_RXCIF_bm));
	return USARTD0.DATA;
}