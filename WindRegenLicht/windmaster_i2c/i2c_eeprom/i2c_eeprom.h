/*
 * IncFile1.h
 *
 * Created: 11.05.2017 08:16:40
 *  Author: Stud
 */ 

#ifndef __I2C_EEPROM_H__
#define __I2C_EEPROM_H__

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/************************************************************************/
/* Initialisierung der i2c eeprom                                       */
/************************************************************************/
void i2c_eeprom_init();

/************************************************************************/
/*                                                                      */
/************************************************************************/
void i2c_wind_read(uint8_t i2c_addr, uint8_t* buffer, size_t len);


#endif