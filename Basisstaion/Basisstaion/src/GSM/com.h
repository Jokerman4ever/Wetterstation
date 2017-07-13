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
#define UART_MAXSTRLEN 64
uint8_t com_check_string(uint8_t len, const char* antwort,uint8_t laenge_antwort);
void com_init(void);
void com_send_string(char *);
void com_send_antwortclient(char *, uint16_t wert);
void interrupt_init(void);
void server_configuration(uint8_t step);
void server_configuration_auswertung(uint8_t len);
uint8_t com_getNextMsg(uint8_t* str,uint8_t off,uint8_t len);
uint8_t com_getString(uint8_t* buffer);
uint8_t com_getChar(uint8_t* error);
uint8_t com_hasData(void);
void com_ausgabe(uint8_t data);
uint8_t com_StrCmp(uint8_t* str1,uint8_t off1,uint8_t len1,const char* str2);
uint8_t server_initialisierung;



#endif /* COM_H_ */