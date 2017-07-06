/*
 * GccApplication1.c
 *
 * Created: 11.05.2017 10:58:27
 * Author : Stud
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include "i2c_eeprom.h"
#include "spi_eeprom.h"

#define SLAVEADDR 0xE2

int main(void)
{
	i2c_eeprom_init();
	
	uint8_t rueckgabe[2] = {0xFF, 0xFF};
	size_t test;
	uint8_t ergebnis;
	uint8_t cnt = 0;
    while (1)
    {
		i2c_wind_read(SLAVEADDR,rueckgabe,2);
		_delay_ms(1000);
    }
}

