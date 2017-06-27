/*
 * com.c
 *
 * Created: 01.06.2017 08:27:16
 *  Author: Stud
 */ 

#include "com.h"
#include <stdio.h>
#include <ASF/common/services/clock/sysclk.h>

#define COM_INTERFACE USARTC0
#define COM_PORT PORTC
#define	TX_PIN 7
#define RX_PIN 6

#define BAUD 9600


static int ioputchar( char c, FILE *f)
{
	com_putc(c);
	return 0;
}

static FILE mystdout = FDEV_SETUP_STREAM(ioputchar, NULL, _FDEV_SETUP_WRITE);

void com_init(void)
{
	stdout = &mystdout;
	
	PORTC.OUTSET = (1<<TX_PIN);
	PORTC.DIRSET = (1<<TX_PIN);
		
	//Set Baudrate
	COM_INTERFACE.BAUDCTRLA = (F_CPU/16/BAUD-1) & 0xff;
	COM_INTERFACE.BAUDCTRLB = (F_CPU/16/BAUD-1) >> 8;
	//Set Frameformate
	COM_INTERFACE.CTRLC = USART_PMODE_DISABLED_gc | USART_CHSIZE_8BIT_gc; //8N1
	//Set Operation Mode
	COM_INTERFACE.CTRLC |= USART_CMODE_ASYNCHRONOUS_gc;
	
	//Enable Interface
	COM_INTERFACE.CTRLB = USART_TXEN_bm | USART_RXEN_bm;
}

void com_putc(uint8_t data)
{
	while(!(COM_INTERFACE.STATUS & USART_DREIF_bm));
	COM_INTERFACE.STATUS |= USART_TXCIF_bm;
	COM_INTERFACE.DATA = data;
	while(!(COM_INTERFACE.STATUS & USART_TXCIF_bm));
}

uint8_t com_getc(void)
{
	COM_INTERFACE.STATUS |= USART_RXCIF_bm;
	while(!(COM_INTERFACE.STATUS & USART_RXCIF_bm));
	return COM_INTERFACE.DATA;
}

void com_enable()
{
	sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_USART0);
}

void com_disable()
{
	sysclk_disable_module(SYSCLK_PORT_C, SYSCLK_USART0);
}
