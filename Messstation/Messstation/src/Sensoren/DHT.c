/*
 * Feuchte.c
 *
 * Created: 24.05.2017 11:49:56
 *  Author: Stud
 */ 

#include "Sensoren/DHT.h"
#include <util/delay.h>

#define DHT_PORT PORTC
#define DHT_DATA 4
#define DHT_DATA_bm  0x10
#define DHT_VCC 0
#define DHT22	//DHT11 or DHT22


#define TIMEOUT 85 //Abort Reading after "x" µs

void DHT_init()
{
	DHT_PORT.DIRSET = (1<<DHT_VCC);
}

void DHT_on()
{
	DHT_PORT.OUTSET = (1<<DHT_VCC);
}

void DHT_off()
{
	DHT_PORT.OUTCLR = (1<<DHT_VCC);
}

bool DHT_read(uint16_t* feuchte, int16_t* temp)
{
	uint8_t laststate = (1<<DHT_DATA);
	uint8_t counter = 0;
	uint8_t j = 0, i;
	uint8_t data[5];
	uint8_t timings[40];

	//Send Start condition
	// pull pin down for 18 milliseconds
	DHT_PORT.DIRSET = (1<<DHT_DATA);
	DHT_PORT.OUTSET = (1<<DHT_DATA);
	_delay_ms(10);
	DHT_PORT.OUTCLR = (1<<DHT_DATA);
	_delay_ms(18);
	// then pull it up for 40 microseconds
	DHT_PORT.OUTSET = (1<<DHT_DATA);
	_delay_us(40);
	// prepare to read the pin
	DHT_PORT.DIRCLR = (1<<DHT_DATA);
	// detect change and read data
	for ( i=0; i< TIMEOUT; i++)
	{
		counter = 0;
		while (((DHT_PORT.IN & DHT_DATA_bm) == laststate) && (counter < 128))
		{
			counter++;
		#if (F_CPU  > 2000000UL)
			_delay_us(1);
		#endif
		}
		laststate = (DHT_PORT.IN & DHT_DATA_bm);
			if (counter == 128) break;
			// ignore first 3 transitions
		if ((i >= 4) && (i%2 == 0)) 
		{
			timings[j] = counter;
			j++;
		}
	}
		// check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
	// print it out if data is good
	if (j >= 40) 
	{	
		for (uint8_t x=0; x < 40; x++)
		{
			// shove each bit into the storage bytes
			data[x/8] <<= 1;
			if (timings[x] > 8)
			{
				data[x/8] |= 1;	
			}				
		}
		if(data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) 
		{
		#ifdef DHT22
			int16_t t, h;
			h = (int16_t)((data[0] << 8) + data[1]);
			t = (int16_t)(((data[2] & 0x7F) << 8) + data[3]);
			if ((data[2] & 0x80) != 0)  {t *= -1;}
		
			*feuchte = h;
			*temp = t;
		#endif
		
		#ifdef DHT11
			*feuchte = data[0];
			*temp = data[2];
		#endif
		
			return true;
		}
	}
	else
	{
		return false;
	}
	return false;
}