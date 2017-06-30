/************************************************************************/
/*				Praktikum Drahtlose Sensorsysteme                       */
/*				Versuch Kommunikation									*/
/*				Datum: 01.06.2017                                       */
/*				Author: Dennis Michalik                                 */
/*				Matrikelnummer: 257934									*/
/************************************************************************/

#ifndef COM_H_
#define COM_H_

#define UART_MAXSTRLEN 20

#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/delay.h>

void com_init(void);

uint8_t com_adjustbaudrate(uint32_t baudrate);

void com_sendchar(char sign);

static int ioputchar(char c, FILE *f);

#endif