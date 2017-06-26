/*
 * Druck.c
 *
 * Created: 24.05.2017 10:28:36
 *  Author: Stud
 */ 

#include "i2c.h"
#include "Sensoren/Druck.h"

#define DRUCK_ADDR 0b01110111

static bool is_busy(void);


typedef struct {
	int16_t AC1;
	int16_t AC2;
	int16_t AC3;
	uint16_t AC4;
	uint16_t AC5;
	uint16_t AC6;
	int16_t B1;
	int16_t B2;
	int16_t MB;
	int16_t MC;
	int16_t MD;
	} calibrationData_t;

calibrationData_t calibrationData;

void Druck_init(void)
{
	uint8_t buffer[22];
	i2c_read(DRUCK_ADDR, 0xAA, buffer, 22);
	
	calibrationData.AC1 = (int16_t)((buffer[0] << 8) + buffer[1]);
	calibrationData.AC2 = (int16_t)((buffer[2] << 8) + buffer[3]);
	calibrationData.AC3 = (int16_t)((buffer[4] << 8) + buffer[5]);
	calibrationData.AC4 = (uint16_t)((buffer[6] << 8) + buffer[7]);
	calibrationData.AC5 = (uint16_t)((buffer[8] << 8) + buffer[9]);
	calibrationData.AC6 = (uint16_t)((buffer[10] << 8) + buffer[11]);
	calibrationData.B1 = (int16_t)((buffer[12] << 8) + buffer[13]);
	calibrationData.B2 = (int16_t)((buffer[14] << 8) + buffer[15]);
	calibrationData.MB = (int16_t)((buffer[16] << 8) + buffer[17]);
	calibrationData.MC = (int16_t)((buffer[18] << 8) + buffer[19]);
	calibrationData.MD = (int16_t)((buffer[20] << 8) + buffer[21]);
}

static bool is_busy(void)
{
	uint8_t data[1];
	bool status = false;
	if(i2c_read(DRUCK_ADDR, 0xF4, data, 1) == 1) //Check "Start of conversion"-Bit
	{
		if(data[0] & 0x20) 
		{
			status = true;
		}
	}
	return status;
}

float Druck_read_Temp(void)
{
	uint8_t data[2];
	int64_t x1, x2, b5, t, ut;
	
	i2c_write_byte(DRUCK_ADDR, 0xF4, 0x2E); //Start conversion
	while(is_busy());	//Wait until conversion finished
	i2c_read(DRUCK_ADDR, 0xF6, data, 2); //Read Data
	
	ut = (data[0] << 8) + (data[1]);
	x1 = (ut - calibrationData.AC6) * calibrationData.AC5 / 2^15;
	x2 = calibrationData.MC * 2^11 / (x1 + calibrationData.MD);
	b5 = x1 + x2;
	t = (b5 + 8) / 2^4;
	
	return ((float)t * 0.1);
}


float Druck_read(void)
{
	uint8_t data[2];
	int64_t x1, x2, x3, b3, b5, b6, t, p, ut, up;
	uint64_t b4, b7;
	int64_t bla, blub;
	
	ut = 27363;
	up = -22120;
	
	//Read Temp for compensation
	i2c_write_byte(DRUCK_ADDR, 0xF4, 0x2E); //Start conversion
	while(is_busy());	//Wait until conversion finished
	i2c_read(DRUCK_ADDR, 0xF6, data, 2); //Read Data
	
	//Temp compensation
	//ut = (data[0] << 8) + (data[1]);
	x1 = (ut - calibrationData.AC6) * calibrationData.AC5 / 32768;
	x2 = calibrationData.MC * (int64_t)2048 / (x1 + calibrationData.MD);
	b5 = x1 + x2;
	t = (b5 + 8) / 16;
	
	//Read Druck
	i2c_write_byte(DRUCK_ADDR, 0xF4, 0x34);
	while(is_busy());
	i2c_read(DRUCK_ADDR, 0xF6, data, 2);
	
	//Druck compensation
	//up = (data[0] << 8) + (data[1]<<0);
	b6 = b5 - 4000;
	x1 = (calibrationData.B2 * (b6 * b6 / 4096)) / 2048;
	x2 = calibrationData.AC2 * b6 / 2048;
	x3 = x1 + x2;
	b3 = (((calibrationData.AC1 * (int64_t)4 + x3) << 0) + 2) / 4;
	x1 = (calibrationData.AC3 * b6) / 8192;
	x2 = (calibrationData.B1 * (b6 * b6 / 4096)) / 65536;
	x3 = ((x1 + x2) + 2) / 4;
	b4 = calibrationData.AC4 * (uint64_t)(x3 + 32768) / 32768;
	b7 = ((uint64_t)up - b3) * (50000 >> 0);
	if(b7 < 0x80000000)
	{
		p = (b7 * 2)/b4;
	}
	else
	{
		p = (b7 / b4) * 2;
	}
	x1 = (p / 256) * (p / 256);
	x1 = (x1 * 3038) / 65536;
	x2 = (-7357 * p) / 65536;
	p = p + (x1 + x2 + 3791) / 16;
	return ((float) p / 1000);
}