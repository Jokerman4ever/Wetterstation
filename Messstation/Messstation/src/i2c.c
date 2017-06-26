/*
 * i2c.c
 *
 * Created: 22.05.2017 21:14:59
 *  Author: TIM
 */ 

#include "I2C.h"
#include <ASF/common/services/clock/sysclk.h>

#define TWI_BAUD(F_SYS, F_TWI) ((F_SYS / (2 * F_TWI)) - 5)
#define TWI_BAUDRATE 100000
#define TWI_BAUDSETTING TWI_BAUD(F_CPU, TWI_BAUDRATE)

#define TWI_WAIT_RW_COMPLETE() while(!(TWIC.MASTER.STATUS & TWI_MASTER_WIF_bm) && !(TWIC.MASTER.STATUS & TWI_MASTER_RIF_bm))
#define TWI_ACK_RECEIVED() !(TWIC.MASTER.STATUS & TWI_MASTER_RXACK_bm)
#define TWI_BUSERR() ((TWIC.MASTER.STATUS & TWI_MASTER_BUSERR_bm) && (TWIC.MASTER.STATUS & TWI_MASTER_WIF_bm))

static bool send_address(uint8_t addr, bool read)
{
	addr <<= 1;
	addr += read;
	/*if(read){
		addr++;
	}
*/
	//TWIC.MASTER.ADDR = write ? addr : addr | 0x01;
	TWIC.MASTER.ADDR = addr;
	TWI_WAIT_RW_COMPLETE();

	//ACK RECEIVED?
	return TWI_ACK_RECEIVED();
}

static void send_stop_condition(void)
{
	TWIC.MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
}

static void send_nack_and_stop(void)
{
	TWIC.MASTER.CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_STOP_gc;
}

static bool send_byte(const uint8_t data)
{
	TWIC.MASTER.DATA = data;
	TWI_WAIT_RW_COMPLETE();
	return TWI_ACK_RECEIVED();
}

static void send_ack(void)
{
	TWIC.MASTER.CTRLC = TWI_MASTER_CMD_RECVTRANS_gc;
}

static bool read_byte(uint8_t* data)
{
	TWI_WAIT_RW_COMPLETE();
	*data = TWIC.MASTER.DATA;
	return !TWI_BUSERR();
}

void i2c_init(void)
{
	i2c_enable();
	PORTC.PIN0CTRL |= PORT_OPC_PULLUP_gc;
	PORTC.PIN1CTRL |= PORT_OPC_PULLUP_gc;
	
	TWIC.MASTER.BAUD = TWI_BAUDSETTING;
	TWIC.MASTER.CTRLA = TWI_MASTER_ENABLE_bm;
	TWIC.MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;
	i2c_disable();
}

void i2c_enable(void)
{
	sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_TWI);
}

void i2c_disable(void)
{
	sysclk_disable_module(SYSCLK_PORT_C, SYSCLK_TWI);
}

bool i2c_send_oppcode(uint8_t i2c_addr,  uint8_t oppcode)
{

	if(!send_address(i2c_addr, false))
	{
		send_stop_condition();
		return false;
	}
	//Address Byte
	if(!send_byte(oppcode))
	{
		send_stop_condition();
		return false;
	}
	
	send_stop_condition();
	return true;
}

bool i2c_write_byte(uint8_t i2c_addr,  uint8_t addr, uint8_t data)
{

	if(!send_address(i2c_addr, false))
	{
		send_stop_condition();
		return false;
	}
	//Address Byte
	if(!send_byte((uint8_t)addr))
	{
		send_stop_condition();
		return false;
	}
	
	//Data Byte
	if(!send_byte((uint8_t)data))
	{
		send_stop_condition();
		return false;
	}
	send_stop_condition();
	return true;
}

bool i2c_write(uint8_t i2c_addr,  uint8_t addr, const uint8_t* buffer, size_t len)
{
	size_t i=0;
	
	if(!send_address(i2c_addr, false))
	{
		send_stop_condition();
		return false;
	}
	//Address Byte
	if(!send_byte((uint8_t)addr))
	{
		send_stop_condition();
		return false;
	}
	
	//Data Bytes
	for (i=0; i<len; i++)
	{
		if(!send_byte(buffer[i])) { break; }
	}
	
	send_stop_condition();
	return i==len;
}

bool i2c_read_byte(uint8_t i2c_addr, uint8_t addr, uint8_t* data)
{
	if(!send_address(i2c_addr, false))
	{
		send_stop_condition();
		return false;
	}
	//Address Byte
	if(!send_byte((uint8_t)addr))
	{
		send_stop_condition();
		return false;
	}
	
	if(!send_address(i2c_addr, true))
	{
		send_stop_condition();
		return false;
	}
	
	//Read Data Byte
	if(!read_byte(data)) 
	{  
		send_stop_condition();
		return false;
	}

	send_nack_and_stop();
	return true;
}

size_t i2c_read(uint8_t i2c_addr, uint8_t addr, uint8_t* buffer, size_t len)
{
	size_t i=0;
	
	if(!send_address(i2c_addr, false))
	{
		send_stop_condition();
		return false;
	}
	//Address Byte
	if(!send_byte((uint8_t)addr))
	{
		send_stop_condition();
		return false;
	}
	
	if(!send_address(i2c_addr, true))
	{
		send_stop_condition();
		return false;
	}
	
	for (i=0; i<len; i++)
	{
		if(!read_byte(&buffer[i])) { break; } //Break on transmisson Error
		if((i+1) != len) { send_ack(); } //Don't send ACK if last Byte to read
	}
	
	send_nack_and_stop();
	return i;
}