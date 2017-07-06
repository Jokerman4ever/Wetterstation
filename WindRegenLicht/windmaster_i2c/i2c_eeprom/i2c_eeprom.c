/*
 * i2c_eeprom.c
 *
 * Created: 11.05.2017 08:41:07
 *  Author: Stud
 */ 

#include "i2c_eeprom.h"
#include <avr/io.h>
#include <util/delay.h>

/*
MAKRO ersetzt einen Text 
*/
#define TWI_BAUD(F_SYS, F_TWI) ((F_SYS / (2*F_TWI)) - 5)
#define TWI_BAUDRATE 100000
#define TWI_BAUDSETTING	TWI_BAUD(F_CPU, TWI_BAUDRATE)

// Makro zum warten, bis das Interrupt Flag vom TWI wieder freigegeben wird
#define TWI_WAIT_RW_COMPLETE() while(!(TWIC.MASTER.STATUS & TWI_MASTER_WIF_bm) && !(TWIC.MASTER.STATUS & TWI_MASTER_RIF_bm))
#define TWI_ACK_RECEIVED() !(TWIC.MASTER.STATUS & TWI_MASTER_RXACK_bm) 
// If a bus error occours WIF *and* BUSERR flag are set
#define TWI_BUSERR() ((TWIC.MASTER.STATUS & TWI_MASTER_BUSERR_bm) && (TWIC.MASTER.STATUS & TWI_MASTER_WIF_bm))

// static: ist nur innerhalb der Datei sichtbar (Verhindert den Aufruf der Funktion von auﬂen
static bool send_start_and_address(uint8_t addr, bool write)
{
	addr <<= 1;   // ist das gleiche wie Scheieben des Registers um eins nach links;  addr = addr << 1
	TWIC.MASTER.ADDR = write ? addr : addr | 0x01;     // ?-Operator, wenn write true dann vor dem Doppelpunkt ausgef¸hrt, falls false dann hinterer Teil
	TWI_WAIT_RW_COMPLETE();
	return TWI_ACK_RECEIVED();
}

static void send_stop_condition()
{
	TWIC.MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
}

static void send_nack_and_stop_condition()
{
	TWIC.MASTER.CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_STOP_gc;
}

static void ack_and_received()
{
	TWIC.MASTER.CTRLC = TWI_MASTER_CMD_RECVTRANS_gc;
}

static bool send_byte(uint8_t byte)
{
	TWIC.MASTER.DATA = byte;
	TWI_WAIT_RW_COMPLETE();
	return TWI_ACK_RECEIVED();
}

static bool receive_byte(uint8_t* data)
{
	TWI_WAIT_RW_COMPLETE();
	*data = TWIC.MASTER.DATA;
	return !TWI_BUSERR();
}

static uint8_t recieve()
{
	TWI_WAIT_RW_COMPLETE();
	return TWIC.MASTER.DATA;
}

void i2c_eeprom_init()
{
	// Raise pullups
	PORTC.PIN0CTRL = (PORTC.PIN0CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc;
	PORTC.PIN1CTRL = (PORTC.PIN1CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc;
	
	TWIC.MASTER.BAUD = TWI_BAUDSETTING;
	TWIC.MASTER.CTRLA = TWI_MASTER_ENABLE_bm;
	TWIC.MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;
}

void i2c_wind_read(uint8_t i2c_addr, uint8_t* buffer, size_t len)
{
	size_t num = 0;
	if (!send_start_and_address(i2c_addr,false))
	{
		send_stop_condition();
		return false;
	}
	
	while (num < len)
	{
		if (!receive_byte(&buffer[num]))
		{
			break;
		}
		ack_and_received();
		num++;
	}
	send_nack_and_stop_condition();
}

