#include "Flash.h"
#include <asf.h>
/*
 * Flash.c
 *
 * Created: 13.12.2016 15:22:43
 *  Author: 256090
 */ 
//Flash.c modified to support 256mbit flash space.
//This requires to use the read4, write4 and sectorErease4 commands
//The old functions were uncommented and but can be restored

void Flash_SPI_Init(void)
{
	sysclk_enable_module(SYSCLK_PORT_D, SYSCLK_SPI);
	
	//MOSI MISO SCK
	//5     6    7
	Flash_SPI_PORT.DIRSET = (1<<5);//MOSI

	Flash_SPI_PORT.DIRSET = (1<<7);//SCK

	PORTD.PIN4CTRL |= PORT_OPC_PULLUP_gc; //CS?
	SPID.CTRL = SPI_ENABLE_bm | SPI_MASTER_bm | SPI_PRESCALER_DIV4_gc | SPI_MODE_0_gc;
	//sysclk_disable_module(SYSCLK_PORT_C, SYSCLK_SPI);

	Flash_CS_PORT.DIRSET = (1<<Flash_CS_Pin);
	Flash_CS_HIGH();
	Flash_SPI_PORT.OUTSET = (1<<7);
	Flash_SPI_PORT.OUTSET = (1<<5);
	//PORTC.OUT |= (1<<3) | (1<<4)|(1<<5)|(1<<7);//CS auf High
}


void SPI_putc(uint8_t data)
{
	SPID.DATA = data;
	while(!(SPID.STATUS & SPI_IF_bm));
}

uint8_t SPI_getc(void)
{
	SPID.DATA = 0x00; //Dummy Byte
	while(!(SPID.STATUS & SPI_IF_bm));//Wait
	return SPID.DATA;//return data
}

uint8_t Flash_check_ID(void)
{
	Flash_CS_LOW();
	SPI_putc(0x9F);
	uint8_t man = SPI_getc();
	uint8_t devT = SPI_getc();
	uint8_t cap = SPI_getc();
	Flash_CS_HIGH();
	if(((man == 0x01) && (devT == 0x40)) && (cap == 0x17))return 1;
	else return 0;
}

uint8_t Flash_isBusy(void)
{
	Flash_CS_LOW();
	SPI_putc(Flash_Status);
	uint8_t status = SPI_getc();
	Flash_CS_HIGH();
	return (status & 0x01);
}

uint8_t Flash_read_Byte(FlashAddress add)
{
	 Flash_CS_LOW();
	 SPI_putc(Flash_Read);
	 //SPI_putc(add.High); //HighByte
	 SPI_putc(add.Mid); //MidByte
	 SPI_putc(add.Low); //LowByte
	 SPI_putc(add.XLow);
	 uint8_t data = SPI_getc();
	 Flash_CS_HIGH();
	 return data;
}

void Flash_read_Bytes(FlashAddress add,uint8_t *buffer,uint8_t length)
{
	Flash_CS_LOW();
	SPI_putc(Flash_Read);
	//SPI_putc(add.High); //HighByte
	SPI_putc(add.Mid); //MidByte
	SPI_putc(add.Low); //LowByte
	SPI_putc(add.XLow); //XLowByte
	for(uint8_t i=0; i<length; i++)
	{
		*buffer++=SPI_getc();
	}
	Flash_CS_HIGH();
}

void Flash_write_Byte(FlashAddress add,uint8_t data)
{
	Flash_CS_LOW();
	SPI_putc(Flash_WriteEnable);
	Flash_CS_HIGH();
	Flash_CS_LOW();
	SPI_putc(Flash_Write);
	//SPI_putc(add.High); //HighByte
	SPI_putc(add.Mid); //MidByte
	SPI_putc(add.Low); //LowByte
	SPI_putc(add.XLow); //XLowByte
	SPI_putc(data);
	Flash_CS_HIGH();
}

void Flash_write_Bytes(FlashAddress add,uint8_t *buffer,uint8_t offset,uint8_t length)
{
	Flash_CS_LOW();
	SPI_putc(Flash_WriteEnable);
	Flash_CS_HIGH();
	Flash_CS_LOW();
	SPI_putc(Flash_Write);
	//SPI_putc(add.High); //HighByte
	SPI_putc(add.Mid); //MidByte
	SPI_putc(add.Low); //LowByte
	SPI_putc(add.XLow); //XLowByte
	for (uint8_t i = 0; i < offset; i++) *buffer++;//Apply offset
	for (int i=0; i<length; i++)
	{
		SPI_putc(*buffer++);
	}
	Flash_CS_HIGH();
	while(Flash_isBusy());
}

void Flash_sector_Erase(FlashAddress add)
{
	Flash_CS_LOW();
	SPI_putc(Flash_WriteEnable);
	Flash_CS_HIGH();
	Flash_CS_LOW();
	SPI_putc(Flash_SectorErase);
	//SPI_putc(add.High); //HighByte
	SPI_putc(add.Mid); //MidByte
	SPI_putc(add.Low); //LowByte
	SPI_putc(add.XLow); //XLowByte
	Flash_CS_HIGH();
	while(Flash_isBusy());
}

FlashAddress FlashTempAdd;
FlashAddress Flash_CreateAddress(uint8_t high,uint8_t mid,uint8_t low,uint8_t xlow)
{
	FlashTempAdd.High = high;
	FlashTempAdd.Mid = mid;
	FlashTempAdd.Low = low;
	FlashTempAdd.XLow = xlow;
	return FlashTempAdd;
}

void Flash_chip_Erase(void)
{
	Flash_CS_LOW();
	SPI_putc(Flash_WriteEnable);
	Flash_CS_HIGH();
	Flash_CS_LOW();
	SPI_putc(Flash_ChipErase);
	Flash_CS_HIGH();
}