#include "led.h"

void port_init(void)
{
	PORTD.DIR = 0xFF;
	PORTC.DIR = 0xFD;
}

void led_on(char port, uint8_t pin)
{
	if (port == 'D')
	{
		PORTD.OUTSET |= (1<<pin);
	}
	if (port == 'C')
	{
		PORTC.OUTSET |= (1<<pin);
	}
}

void led_off(char port, uint8_t pin)
{
	if (port == 'D')
	{
		PORTD.OUTCLR &= (1<<pin);
	}
	if (port == 'C')
	{
		PORTC.OUTCLR &= (1<<pin);
	}
}

void led_tgl(char port, uint8_t pin)
{
	if (port == 'D')
	{
		PORTD.OUTTGL |= (1<<pin);
	}
	if (port == 'C')
	{
		PORTC.OUTTGL |= (1<<pin);
	}
}