/*
 * BME280.c
 *
 * Created: 19.06.2017 14:16:07
 *  Author: TIM
 */ 


#include "i2c.h"
#include "Sensoren/BME280.h"
#include <util/delay.h>

#define BME_ADDR 0b01110110

static bool is_busy(void);

typedef struct {
	//Temperatur
	uint16_t dig_T1;
	int16_t dig_T2;
	int16_t dig_T3;
	
	//Druck
	uint16_t dig_P1;
	int16_t dig_P2;
	int16_t dig_P3;
	int16_t dig_P4;
	int16_t dig_P5;
	int16_t dig_P6;
	int16_t dig_P7;
	int16_t dig_P8;
	int16_t dig_P9;
		
	//Feuchte
	uint8_t dig_H1;
	int16_t dig_H2;
	uint8_t dig_H3;
	int16_t dig_H4;
	int16_t dig_H5;
	int8_t dig_H6;
	
} BME_calibrationData_t;

BME_calibrationData_t BMEcalibrationData;

void BME280_init(void)
{
	uint8_t buffer[26];

	i2c_read_byte(BME_ADDR, 0xD0, &buffer[0]);
	i2c_read_byte(BME_ADDR, 0xF5, &buffer[1]);

	i2c_read(BME_ADDR, 0x88, buffer, 26);
	BMEcalibrationData.dig_T1 = ((uint16_t)buffer[1] <<8) + buffer[0];
	BMEcalibrationData.dig_T2 = ((int16_t)buffer[3] <<8) + buffer[2];
	BMEcalibrationData.dig_T3 = ((int16_t)buffer[5] <<8) + buffer[4];
	BMEcalibrationData.dig_P1 = (buffer[7] <<8) + buffer[6];
	BMEcalibrationData.dig_P2 = (buffer[9] <<8) + buffer[8];
	BMEcalibrationData.dig_P3 = (buffer[11] <<8) + buffer[10];
	BMEcalibrationData.dig_P4 = (buffer[13] <<8) + buffer[12];
	BMEcalibrationData.dig_P5 = (buffer[15] <<8) + buffer[14];
	BMEcalibrationData.dig_P6 = (buffer[17] <<8) + buffer[16];
	BMEcalibrationData.dig_P7 = (buffer[19] <<8) + buffer[18];
	BMEcalibrationData.dig_P8 = (buffer[21] <<8) + buffer[20];
	BMEcalibrationData.dig_P9 = (buffer[23] <<8) + buffer[22];
	BMEcalibrationData.dig_H1 = buffer[25];
	
	i2c_read_byte(BME_ADDR, 0xA1, &BMEcalibrationData.dig_H1);
	
	i2c_read(BME_ADDR, 0xE1, buffer, 10);
	BMEcalibrationData.dig_H2 = (buffer[1] << 8) + buffer[0];
	BMEcalibrationData.dig_H3 = buffer[2];
	BMEcalibrationData.dig_H4 = (buffer[3] << 4) + (buffer[4] & 0x0F);
	BMEcalibrationData.dig_H5 = (buffer[5] << 4) + (buffer[4] >> 4);
	BMEcalibrationData.dig_H6 = buffer[6];
	
	i2c_write_byte(BME_ADDR, 0xF2, 0x01); // Set humidity oversampling to 1;
	i2c_write_byte(BME_ADDR, 0xF4, 0x24); // Set Persure & Temp oversampling to 1;
}

static bool is_busy(void)
{
	uint8_t data;
	bool status = false;
	if(i2c_read_byte(BME_ADDR, 0xF4, &data))
	{
		if((data & 0x03) != 0)
		{
			return true;
		}
	}
/*	if(i2c_read_byte(BME_ADDR, 0xF3, &data)) //Check "Start of conversion"-Bit
	{
		if((data & 0x04) != 0)
		{
			status = true;
		}
	}*/
	return status;
}

int32_t v_x1_u32r;  

bool BME280_read(uint16_t* druck, int16_t* temp, uint16_t* feuchte)
{
	uint8_t data[8];
	int32_t ut, x1, x2, t, t_fine, up, uh;
	uint32_t p, h;
	
	uint8_t ctrl = (1 << 0) | (1 << 2) | (0x01 << 5);

	//i2c_write_byte(BME_ADDR, 0xF5, (0x04 << 2));

	//Read Temp
	i2c_write_byte(BME_ADDR, 0xF2, 0x01); // Set humidity oversampling to 1;
	i2c_write_byte(BME_ADDR, 0xF4, ctrl); //Start conversion
	while(is_busy()){_delay_ms(1);}	//Wait until conversion finished
	i2c_read(BME_ADDR, 0xF7, data, 8); //Read Data
	
	//Temp compensation
	ut = (int32_t)( (((int32_t)data[3]) << 12) | (((int32_t)data[4]) << 4) | ((((int32_t)data[5] >> 4)) & 0x0F));
	
	/* calculate x1*/
	x1 = (((ut >> 3) - ((int32_t)BMEcalibrationData.dig_T1 << 1)) * ((int32_t)BMEcalibrationData.dig_T2)) >> 11;
	/* calculate x2*/
	x2  = (((((ut >> 4) - ((int32_t)BMEcalibrationData.dig_T1)) * ((ut >> 4) - ((int32_t)BMEcalibrationData.dig_T1))) >> 12) * ((int32_t)BMEcalibrationData.dig_T3)) >> 14;
	/* calculate t_fine*/
	t_fine = x1 + x2;
	/* calculate temperature*/
	t  = (t_fine * 5 + 128) >> 8; //value in 0.01 degree
	*temp = (int16_t) (t/10); //in 0.1
	
	
	//Druck compensation
	up = (int32_t)((((uint32_t)(data[0])) << 12) | (((uint32_t)(data[1])) << 4) | (((uint32_t)data[2] >> 4) & 0x0F));
		
	/* calculate x1*/
	x1 = (t_fine >> 1) - (int32_t)64000;
	/* calculate x2*/
	x2 = (((x1 >> 2) * (x1 >> 2)) >> 11) * ((int32_t)BMEcalibrationData.dig_P6);
	/* calculate x2*/
	x2 = x2 + ((x1 * ((int32_t)BMEcalibrationData.dig_P5)) << 1);
	/* calculate x2*/
	x2 = (x2 >> 2) + (((int32_t)BMEcalibrationData.dig_P4) << 16);
	/* calculate x1*/
	x1 = (((BMEcalibrationData.dig_P3 *	(((x1 >> 2) * (x1 >> 2)) >> 13)) >> 3) + ((((int32_t)BMEcalibrationData.dig_P2) * x1) >> 1)) >> 18;
	/* calculate x1*/
	x1 = ((((32768 + x1)) * ((int32_t)BMEcalibrationData.dig_P1)) >> 15);
	/* calculate pressure*/
	p =	(((uint32_t)(((int32_t)1048576) - up) - (x2 >> 12))) * 3125;
	if (p < 0x80000000)
	{
		/* Avoid exception caused by division by zero */
		if (x1 != 0) {	p = (p << 1) / ((uint32_t)x1);	}
		else {	return false;	}
	}
	else
	{
		/* Avoid exception caused by division by zero */
		if (x1 != 0) {	p = (p / (uint32_t)x1) * 2;	}
		else {	return false;	}
	}
	
	/* calculate x1*/
	x1 = (((int32_t)BMEcalibrationData.dig_P9) * ((int32_t)(((p >> 3) * (p >> 3)) >> 13))) >> 12;
	/* calculate x2*/
	x2 = (((int32_t)(p >> 2)) *	((int32_t)BMEcalibrationData.dig_P8)) >> 13;
	/* calculate pressure*/
	p = (uint32_t)((int32_t)p + ((x1 + x2 + BMEcalibrationData.dig_P7) >> 4));
	*druck = (uint16_t) (p/10); //in 0.1
	
	
	//Humidity compensation
	uh = (int32_t)((((uint32_t)(data[6])) << 8) | ((uint32_t)(data[7])));
	
	/* calculate x1*/
	x1 = (t_fine - ((int32_t)76800));
	/* calculate x1*/
	x1 = (((((uh << 14) - (((int32_t)BMEcalibrationData.dig_H4) << 20) - (((int32_t)BMEcalibrationData.dig_H5) * x1)) + ((int32_t)16384)) >> 15) * (((((((x1 * ((int32_t)BMEcalibrationData.dig_H6)) >> 10) * (((x1 * ((int32_t)BMEcalibrationData.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) * ((int32_t)BMEcalibrationData.dig_H2) + 8192) >> 14));
	x1 = (x1 - (((((x1 >> 15) * (x1 >> 15)) >> 7) * ((int32_t)BMEcalibrationData.dig_H1)) >> 4));
	x1 = (x1 < 0 ? 0 : x1);
	x1 = (x1 > 419430400 ? 419430400 : x1);
	h = (uint32_t)(x1 >> 12);
	h = (h * 100) / 1024; //value in 0.01 %
	*feuchte = (uint16_t) (h/10); //in 0.1

	return true;
}