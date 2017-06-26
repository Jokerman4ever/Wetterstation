/*
 * BME280.h
 *
 * Created: 19.06.2017 14:15:55
 *  Author: TIM
 */ 


#ifndef BME280_H_
#define BME280_H_

#include <stdbool.h>

void BME280_init(void);
bool BME280_read(uint16_t* druck, int16_t* temp, uint16_t* feuchte);

#endif /* BME280_H_ */