/*
 * Feuchte.h
 *
 * Created: 24.05.2017 11:49:44
 *  Author: Stud
 */ 


#ifndef FEUCHTE_H_
#define FEUCHTE_H_

#include <stdbool.h>
#include <avr/io.h>

bool DHT_read(uint16_t* feuchte, int16_t* temp);
void DHT_on(void);
void DHT_off(void);
void DHT_init(void);


#endif /* FEUCHTE_H_ */