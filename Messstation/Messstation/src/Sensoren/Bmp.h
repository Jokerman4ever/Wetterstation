/*
 * Druck.h
 *
 * Created: 24.05.2017 10:28:01
 *  Author: Stud
 */ 


#ifndef DRUCK_H_
#define DRUCK_H_

#include <stdbool.h>

void BMP_init(void);
bool BMP_read(uint16_t* druck, int16_t* temp);

#endif /* DRUCK_H_ */