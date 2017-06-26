/*
 * LED.h
 *
 * Created: 04.05.2017 08:32:50
 *  Author: Stud
 */ 


#ifndef LED_H_
#define LED_H_

#include <avr/io.h>

#define LEDCount 4
#define LED0PIN 7
#define LED1PIN 6
#define LED2PIN 5
#define LED3PIN 4

#define LED0PORT PORTA
#define LED1PORT PORTA
#define LED2PORT PORTA
#define LED3PORT PORTA

typedef enum LEDState_t{LEDON,LEDOFF} LEDState_t;

//////////////////////////////////////////////////////////////////////////
//Prototypen
void LED_Init(void);
void LED_Set(uint8_t ledNum,LEDState_t state);
void LED_Toggle(uint8_t ledNum);
LEDState_t LED_Get(uint8_t ledNum);
void LED_Lauflicht(void);
void LED_Set_Num(uint8_t num);
void LED_Set_Num_Lin(uint8_t num);
void LED_SetAll(LEDState_t state);

#endif /* LED_H_ */