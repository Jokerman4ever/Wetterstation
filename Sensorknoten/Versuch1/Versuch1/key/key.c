#include "key.h"

void key_init()
{
	PORTC.PIN1CTRL = 0x19;			// interner Pullup und Rising Flag
	PORTC.INT0MASK = 0x02;			// Pin1 and Port C für Interrupt ausgewählt
	PORTC.INTCTRL = PORT_INT0LVL_MED_gc;
	PMIC.CTRL |= PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;	
	sei();
}

uint8_t counter = 0;
ISR(PORTC_INT0_vect)
{	
	led_tgl('D',1);
	counter = counter+1;
	if (counter==99)
	{
		counter=0;
	}
}

uint8_t getkeycnt()
{
	return counter;
}