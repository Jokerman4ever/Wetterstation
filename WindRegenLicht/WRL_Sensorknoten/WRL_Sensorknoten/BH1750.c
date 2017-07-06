/*
 * BH1750.c
 *
 * Created: 15.06.2017 11:08:10
 *  Author: Stud
 */ 

#include "BH1750.h"
#include "i2c.h"
#include <util/delay.h>

#define LUX_ADDR 0b00100011

static bool power_up(void);

bool BH1750_read(uint8_t* lux)
{
		uint8_t data[2];
		bool state = false;
		if(power_up())
		{
			state = i2c_send_oppcode(LUX_ADDR, 0x23); //Star Conversion
			if(state)
			{
				_delay_ms(24);
				state = i2c_read(LUX_ADDR, 0x47, data, 2); //Read Data
				//*lux = (data[0] << 8) + data[1];
				*lux = data[0];
			}
		}
		return state;														//Daten zurückgeben
}

static bool power_up(void)
{
	return i2c_send_oppcode(LUX_ADDR, 0x01);
}