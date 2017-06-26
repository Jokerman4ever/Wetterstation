/*
 * BH1750.h
 *
 * Created: 15.06.2017 11:07:42
 *  Author: Stud
 */ 


#ifndef BH1750_H_
#define BH1750_H_

#include <stdbool.h>
#include <avr/io.h>

bool BH1750_read(uint8_t* lux);

#endif /* BH1750_H_ */