/*
 * LED.c
 *
 * Created: 04.05.2017 08:21:05
 *  Author: Stud
 */ 

#include "LED.h"

void LED_Init(void)
{
	LED0PORT.DIR |= (1<<LED0PIN);
	LED1PORT.DIR |= (1<<LED1PIN);
	LED2PORT.DIR |= (1<<LED2PIN);
	LED3PORT.DIR |= (1<<LED3PIN);
}

void LED_Set_Num(uint8_t num)
{
	for (uint8_t i = 0; i<LEDCount; i++)
	{
		if(num & (1<<i))LED_Set(i,LEDON);
		else LED_Set(i,LEDOFF);
	}
}

void LED_Set_Num_Lin(uint8_t num)
{
	LED_SetAll(LEDOFF);
	if(num > (1<<4))LED_Set(0,LEDON);
	if(num > (1<<5))LED_Set(1,LEDON);
	if(num > (1<<6))LED_Set(2,LEDON);
	if(num > (1<<7))LED_Set(3,LEDON);
}

void LED_SetAll(LEDState_t state)
{
	LED_Set(0,state);
	LED_Set(1,state);
	LED_Set(2,state);
	LED_Set(3,state);
}

void LED_Set(uint8_t ledNum,LEDState_t state)
{
		switch(ledNum)
		{
			case 0: 
			{
				if(state == LEDON)LED0PORT.OUT |= (1<<LED0PIN);
				else LED0PORT.OUT &= ~(1<<LED0PIN);
				break;
			}
			case 1:
			{
				if(state ==LEDON)LED1PORT.OUT |= (1<<LED1PIN);
				else LED1PORT.OUT &= ~(1<<LED1PIN);
				break;
			}
			case 2:
			{
				if(state == LEDON)LED2PORT.OUT |= (1<<LED2PIN);
				else LED2PORT.OUT &= ~(1<<LED2PIN);
				break;
			}
			case 3:
			{
				if(state == LEDON)LED3PORT.OUT |= (1<<LED3PIN);
				else LED3PORT.OUT &= ~(1<<LED3PIN);
				break;
			}
			default: break;
		}
};

void LED_Toggle(uint8_t ledNum)
{
	LEDState_t state = LED_Get(ledNum);
	if(state == LEDON)state = LEDOFF;
	LED_Set(ledNum,state);
}

LEDState_t LED_Get(uint8_t ledNum)
{
	switch(ledNum)
	{
		case 0: return LED0PORT.OUT & (1>>LED0PIN);
		case 1: return LED1PORT.OUT & (1>>LED1PIN);
		case 2: return LED2PORT.OUT & (1>>LED2PIN);
		case 3: return LED3PORT.OUT & (1>>LED3PIN);
		default: break;
	}
	return 0;
}

void LED_Lauflicht(void)
{
	static uint8_t pos = 0;
	static uint8_t dir = 0;
	if(!dir && pos >= LEDCount-1)dir = 1;
	else if(dir && pos == 0)dir = 0;
	LED_Set(pos,LEDOFF);
	pos = dir ? pos-1 : pos+1;
	LED_Set(pos,LEDON);
}