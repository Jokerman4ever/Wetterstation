/*
 * server.h
 *
 * Created: 13.07.2017 11:34:32
 *  Author: Saba-Sultana Saquib
 */ 


#ifndef SERVER_H_
#define SERVER_H_
#include <avr/io.h>
#include <stdio.h>
#include <asf.h>
#include <stdbool.h>
#include <Clock/Xdelay.h>
uint8_t client_anfrage_auswertung();
void com_send_antwortclient(uint8_t senden_array[]);


#endif /* SERVER_H_ */