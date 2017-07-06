/*
 * IncFile1.h
 *
 * Created: 11.05.2017 08:16:40
 *  Author: Stud
 */ 

#ifndef __SPI_WIND_H__
#define __SPI_WIND_H__

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

void spi_wind_masterinit();
uint16_t spi_wind_read();
float spi_get_angle();
int16_t spi_angledif(int16_t pre, int16_t cur);
int16_t spi_windspeed();


#endif