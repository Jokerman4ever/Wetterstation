

/*
 * DSN_EinfuerungsversuchSaba.c
 *
 * Created: 20.04.2016 10:31:13
 * Author : Saba-Sultana Saquib
 */

#ifndef F_CPU
#define F_CPU 2000000
#endif
#include <string.h>
#include <avr/io.h>
#include "display.h"
#include "stdbool.h"
#include <util/delay.h>
#define segmentg_an PORTE.OUTSET= PIN6_bm;
#define segmenta_aus PORTE.OUTCLR= PIN0_bm;
#include "stdio.h"
#include "com.h"
#include "coms.h"


#include <avr/io.h>
#include <avr/interrupt.h>
#define UART_MAXSTRLEN 2000
#define RF_RECBUF_SIZE 100
#define RF_RECBUF_SIZE2 100
 volatile uint8_t uart_str_complete = 0;
_Bool daten_enmpfangen=false;
extern _Bool server_initialisierung;
int main(void)
{ 
uart_init();
com_init();
Clock_init();
interrupt_init();

 //server_configuration();
//send_string("AT+CSQ");
server_configuration();
while(1){




if(uart_str_complete==1){

uart_str_complete=0;}
}

}

void Clock_init(void)
{
	OSC.CTRL |= OSC_RC32MEN_bm;
	while(!(OSC.STATUS & OSC_RC32MRDY_bm));
	CCP = CCP_IOREG_gc;
	CLK.CTRL = CLK_SCLKSEL_RC32M_gc;
}