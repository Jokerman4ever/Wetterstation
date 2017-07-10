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
#include <asf.h>
#include <stdbool.h>
#include <Clock/Xdelay.h>
//#define UART_MAXSTRLEN 100
//volatile char uart_string[UART_MAXSTRLEN + 1];
uint8_t COM_check_string(uint8_t len, const char* antwort,uint8_t laenge_antwort);
void com_init(void);
void COM_Send_string(char *);
void interrupt_init();
void empfangen_string();
void server_configuration();
void server_configuration_auswertung(uint8_t len);
uint8_t com_getString(uint8_t* buffer);
void com_ausgabe(uint8_t data);
uint8_t com_StrCmp(uint8_t* str1,uint8_t off1,uint8_t len1,const uint8_t* str2);
uint8_t server_initialisierung;




#endif /* COM_H_ */