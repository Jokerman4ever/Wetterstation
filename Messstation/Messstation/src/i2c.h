/*
 * i2c.h
 *
 * Created: 22.05.2017 21:14:43
 *  Author: TIM
 */ 


#ifndef I2C_H_
#define I2C_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <avr/io.h>

/**
 * Initialisiere I2C 
 */
void i2c_init(void);

void i2c_enable(void);

void i2c_disable(void);

/**
 * Schreibe auf I2C
 * 
 * i2c_addr: I2C Addresse
 * addr: Speicheraddresse
 * buffer: Datenquelle
 * len: Anzahl der Datenbytes
 */
bool i2c_write(uint8_t i2c_addr, uint8_t addr, const uint8_t* buffer, size_t len);

bool i2c_write_byte(uint8_t i2c_addr, uint8_t addr, uint8_t data);

bool i2c_send_oppcode(uint8_t i2c_addr,  uint8_t oppcode);

/**
 * Lese von I2C
 *
 * i2c_addr: I2C Addresse
 * addr: Speicheraddresse
 * buffer: Datenziel
 * len: Anzahl der Datenbytes
 */
size_t i2c_read(uint8_t i2c_addr, uint8_t addr, uint8_t* buffer, size_t len);

bool i2c_read_byte(uint8_t i2c_addr, uint8_t addr, uint8_t* data);

#endif /* I2C_H_ */