#ifndef LED_H
#define LED_D

#include <avr/io.h>

void port_init(void);
void led_on(char port, uint8_t pin);
void led_off(char port, uint8_t pin);
void led_tgl(char port, uint8_t pin);

#endif