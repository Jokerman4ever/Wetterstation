/*
 * com.h
 *
 * Created: 26.06.2017 08:49:25
 *  Author: Saba-Sultana Saquib
 */ 


#ifndef COM_H_
#define COM_H_
#include <avr/io.h>
#include <stdio.h>
#include <asf.h>
#include <stdbool.h>
#include <Clock/Xdelay.h>
#define UART_MAXSTRLEN 64
//uint8_t com_check_string(uint8_t len, const char* antwort,uint8_t laenge_antwort);
void com_init(void);
void com_send_string(uint8_t* data);
void interrupt_init(void);
void server_configuration();
void server_configuration_auswertung(uint8_t uart_string[]);
uint8_t com_getNextMsg(uint8_t* str,uint8_t off,uint8_t len);
uint8_t com_getString(uint8_t* buffer);
uint8_t com_getChar(uint8_t* error);
uint8_t com_hasData(void);
void com_ausgabe(uint8_t data);
uint8_t com_StrCmp(uint8_t* str1,uint8_t off1,uint8_t len1,const char* str2);
uint8_t server_initialisierung;
uint8_t com_strlen(uint8_t* data); //
void com_empfangen();
void ip_adresse_zwischenspeichern(uint8_t antwort_ip[]);
void signalstaerke_zwischenspeichern(uint8_t antwort[]);
extern volatile uint8_t ip_adresse[20];
extern int8_t init_schritt;
#endif /* COM_H_ */