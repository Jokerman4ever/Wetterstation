/*
 * Windsensor.c
 *
 * Created: 22.06.2017 14:26:11
 * Author : Stud
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include "spi_wind.h"
#include "twi_slave_driver.h"
#include "clock.h"


int main(void)
{
	clock_init();
	spi_wind_masterinit();

    while (1) 
    {
		spi_windspeed();
    }
}





