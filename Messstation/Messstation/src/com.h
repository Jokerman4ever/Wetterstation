/*
 * com.h
 *
 * Created: 01.06.2017 08:27:05
 *  Author: Stud
 */ 


#ifndef COM_H_
#define COM_H_

#include <avr/io.h>
#include <stdbool.h>


void com_init(void);
void com_putc(uint8_t data);
uint8_t com_getc(void);
bool com_getc_timeout(uint8_t *data);
void com_enable(void);
void com_disable(void);

#endif /* COM_H_ */