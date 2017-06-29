/*
 * Xdelay.h
 *
 * Created: 27.06.2017 12:33:16
 *  Author: Stud
 */ 


#ifndef XDELAY_H_
#define XDELAY_H_
#include <util/delay.h>
#define  XDELAY_SCALEFACTOR 16
extern uint8_t XDELAY_ISFAST;

void _xdelay_ms(uint16_t delay);
void _xdelay_us(uint16_t delay);



#endif /* XDELAY_H_ */