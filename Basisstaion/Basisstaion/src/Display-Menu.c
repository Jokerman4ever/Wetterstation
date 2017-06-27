/*
 * Display_Menu.c
 *
 * Created: 02.01.2017 11:14:32
 *  Author: Felix
 */ 
#include "Display-Menu.h"
#include "lcd-routines.h"
uint8_t LineTemp[20];
void PGM_ReadStr(const uint8_t* str,char* dest,uint8_t start)
{
	uint8_t i=0;
	do 
	{
		dest[start+i]=pgm_read_byte((PGM_P)str++);
		i++;
	} while (dest[start + i -1]);
}

#define DStr_WSName PSTR ("MY WEATHER STATION")
#define DStr_WSVersion PSTR("ver. 1.0.0")
#define DStr_WSStatusbar PSTR("")
#define DStr_WSDate PSTR("")
#define DStr_WSMenu PSTR("**MENU**")
#define DStr_WSHome PSTR("Home")
#define DSTR_WSFehlerliste PSTR ("Fehlerliste")
#define DSTR_WSEinstellungen PSTR("Einstellungen")
#define DSTR_WSWettermonitor PSTR("Wettermonitor")

uint8_t DSP_MenuSelection;
uint8_t DSP_CurrentPage;


void DSP_ChangePage(uint8_t ID)
{
	DSP_CurrentPage=ID;
	lcd_clear();//Löschen
	switch(ID)
	{
		case PageWelcome:
		{
			lcd_set_cursor(0,1);
			//
			lcd_set_cursor(0,2);
			CenterStringPGM(DStr_WSName,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,3);
			CenterStringPGM(DStr_WSVersion,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			//
			break;
		}

		case PageMenuHome:
		{
			lcd_set_cursor(0,1);
			//
			lcd_set_cursor(0,2);
			//
			lcd_set_cursor(0,3);
			CenterStringPGM(DStr_WSName,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			//
			break;
		}

		case PageHome:
		{
			lcd_set_cursor(0,1);
			CenterStringPGM(DStr_WSStatusbar,LineTemp,0);
			//lcd_Xstring()// TODO
			lcd_set_cursor(0,2);
			//
			lcd_set_cursor(0,3);
			CenterStringPGM(DStr_WSName,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			CenterStringPGM(DStr_WSDate,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			break;
		}
		

		
		
		default: break;
	}
}

void DSP_ScrollMenu(uint8_t dir)
{
	switch(DSP_CurrentPage)
	{
		default: break;
	}
}

void DSP_SelectMenu(void)
{
	switch(DSP_CurrentPage)
	{
		default: break;
	}
}

void CenterString(char* str,char* Temp,uint8_t arrows)
{
	uint8_t len = strlen(str);
	uint8_t maxspace = 20 - (arrows == 1 || arrows == 2 ? 2 : arrows == 3 ? 4 : 0);
	if(arrows & 0x01){*Temp++='<';*Temp++='-';}
	for (uint8_t i = 0; i < (maxspace-len)/2; i++)*Temp++=32;//Leerzeichen einfügen
	for (uint8_t i = 0; i < len; i++)*Temp++=*str++;
	for (uint8_t i = 0; i < (maxspace-len)/2; i++)*Temp++=32;//Leerzeichen einfügen
	if(arrows & 0x02){*Temp++='-';*Temp++='>';}
}

void CenterStringPGM(const uint8_t* str,uint8_t* Temp,uint8_t arrows)
{
	char tt[20];
	PGM_ReadStr(str,tt,0);
	uint8_t len = strlen(tt);
	uint8_t maxspace = 20 - (arrows == 1 || arrows == 2 ? 2 : arrows == 3 ? 4 : 0);
	if(arrows & 0x01){*Temp++='<';*Temp++='-';}
	for (uint8_t i = 0; i < (maxspace-len)/2; i++)*Temp++=32;//Leerzeichen einfügen
	for (uint8_t i = 0; i < len; i++)*Temp++=tt[i];
	for (uint8_t i = 0; i < (maxspace-len)/2; i++)*Temp++=32;//Leerzeichen einfügen
	if(arrows & 0x02){*Temp++='-';*Temp++='>';}
}

void SelectString(char* str,uint8_t* Temp)
{
	for (uint8_t i = 0; i < 20; i++)
	{
		Temp[i] = 32;
	}
	uint8_t len = strlen(str);
	uint8_t maxspace = (20 - len - 4) /2;
	for (uint8_t i = 0; i < maxspace; i++)*Temp++=32;
	*Temp++='-';*Temp++='>';
	for (uint8_t i = 0; i < len; i++)*Temp++=*str++;
	*Temp++='<';*Temp++='-';
	for (uint8_t i = 0; i < maxspace; i++)*Temp++=32;
}

void SelectStringPGM(const uint8_t* str,uint8_t* Temp)
{
	for (uint8_t i = 0; i < 20; i++)
	{
		Temp[i] = 32;
	}
	char tt[20];
	PGM_ReadStr(str,tt,0);
	uint8_t len = strlen(tt);
	uint8_t maxspace = (20 - len - 4) /2;
	for (uint8_t i = 0; i < maxspace; i++)*Temp++=32;
	*Temp++='-';*Temp++='>';
	for (uint8_t i = 0; i < len; i++)*Temp++=tt[i];
	*Temp++='<';*Temp++='-';
	for (uint8_t i = 0; i < maxspace; i++)*Temp++=32;
}