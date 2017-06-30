#ifndef KEY_H
#define KEY_H

#include <avr/io.h>
#include <avr/interrupt.h>

void key_init();
uint8_t getkeycnt();

#endif