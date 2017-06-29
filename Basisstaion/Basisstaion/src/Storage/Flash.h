/*
 * Flash.h
 *
 * Created: 13.12.2016 15:22:22
 *  Author: 256090
 */ 


#ifndef FLASH_H_
#define FLASH_H_

#include <asf.h>
#include <util/delay.h>
#define Flash_CS_PORT PORTE
#define Flash_CS_Pin 0
#define Flash_SPI_PORT PORTD

#define Flash_CS_HIGH() (Flash_CS_PORT.OUTSET = (1<<Flash_CS_Pin))//PC3 HIGH
#define Flash_CS_LOW() (Flash_CS_PORT.OUTCLR = (1<<Flash_CS_Pin))//PC3 LOW


#define Flash_WriteEnable 0x06
#define Flash_Write 0x02
#define Flash_Read 0x03
#define Flash_Status 0x05
#define Flash_SectorErase 0x20
#define Flash_ChipErase 0xC7

#define Flash_Read4 0x13
#define Flash_Write4 0x12
#define Flash_SectorErase4 0x21

typedef struct
{
	uint8_t High;
	uint8_t Mid;
	uint8_t Low;
	uint8_t XLow;
} FlashAddress;

void Flash_write_Byte(FlashAddress add,uint8_t data);
void Flash_write_Bytes(FlashAddress add,uint8_t *buffer,uint8_t offset,uint8_t length);
uint8_t Flash_read_Byte(FlashAddress add);
void Flash_read_Bytes(FlashAddress add,uint8_t *buffer,uint8_t length);
void Flash_sector_Erase(FlashAddress add);
void Flash_chip_Erase(void);
uint8_t Flash_check_ID(void);
uint8_t Flash_isBusy(void);
void SPI_putc(uint8_t data);
uint8_t SPI_getc(void);
FlashAddress Flash_CreateAddress(uint8_t high,uint8_t mid,uint8_t low,uint8_t xlow);
void Flash_SPI_Init(void);
#endif /* FLASH_H_ */