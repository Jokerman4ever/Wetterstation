/*
 * IncFile1.h
 *
 * Created: 11.05.2017 08:16:40
 *  Author: Stud
 */ 


#ifndef __TWI_WIND_H__
#define __TWI_WIND_H__

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

void twi_wind_init();
void twi_wind_sendack();
void twi_wind_send_byte(uint8_t data);

#endif