/*
 * usart.h
 *
 * Created: 25.05.2016 08:49:25
 *  Author: Stud
 */ 


/*#ifndef USART_H_
#define USART_H_


void com_init();
void com_baud();
void char_ausgabe(char s);
void send_string(char *data);

#endif /* USART_H_ */

/*
 * COM.h
 *
 * Created: 03.06.2015 10:55:38
 *  Author: Stud
 */ 


#ifndef COM_H_
#define COM_H_
#include <avr/io.h>
#include <stdio.h>
//#define UART_MAXSTRLEN 100
//volatile char uart_string[UART_MAXSTRLEN + 1];

void com_init(void);
void COM_Baudrate(void);
void COM_Send(char);
void COM_Send_string(char *);
void interrupt_init();
void empfangen_string();
void server_configuration();
void server_configuration_auswertung();
uint8_t server_initialisierung;




#endif /* COM_H_ */