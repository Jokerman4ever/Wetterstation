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


#define DStr_BSName PSTR (" MY WEATHER STATION ")
#define DStr_BSVersion PSTR("ver. 1.0.0")
#define DStr_BSMenu PSTR("      **MENU**      ")
#define DStr_BSHome PSTR("    Home            ")
#define DStr_BSFehlerliste PSTR ("    Fehlerliste     ")
#define DStr_BSEinstellungen PSTR("    Einstellungen   ")
#define DStr_BSWettermonitor PSTR("    Wettermonitor   ")

uint8_t BatState[4] = "...";
uint8_t GSMState[4] = "...";
uint8_t NumNode = 0;
uint8_t Day[3] = "00";
uint8_t Month[3] = "00";
uint8_t Year[3] = "00";
uint8_t Minute[3] = "00";
uint8_t Hour[3] = "00";

tm_t DSP_Time_Ms;	

uint8_t DSP_MenuSelection;
uint8_t DSP_CurrentPage;

// Muss im Sekundentakt aufgerufen werden (oder schneller)
void DSP_Refresh(uint8_t BS_BatState,uint8_t BS_GSMState,uint8_t BS_NumNode)
{
	time_GetLocalTime(&DSP_Time_Ms);

	// Datum und Zeit aktualisieren:	
	sprintf(Day,"%d",DSP_Time_Ms.tm_mday);
	sprintf(Month,"%d",DSP_Time_Ms.tm_mon);
	sprintf(Year, "%d",DSP_Time_Ms.tm_year);
	sprintf(Minute,"%d",DSP_Time_Ms.tm_min);
	sprintf(Hour,"%d",DSP_Time_Ms.tm_hour);	
	
	// Batteriestatus aktualisieren:
	switch(BS_BatState)
	{
		case BatLow:
		{
			*BatState = "o..";
			break;
		}
		case BatMid:
		{
			*BatState = "oo.";
			break;
		}
		case BatHigh:
		{
			*BatState = "ooo";
			break;
		}
		default:
		{
			*BatState = "...";
			break;
		}
	}
	
	// GSM Sendeleistung aktualisieren
	switch(BS_GSMState)
	{
		case GSMLow:
		{
			*GSMState = "|..";
			break;
		}
		case GSMMid:
		{
			*GSMState = "||.";
			break;
		}
		case GSMHigh:
		{
			*GSMState = "|||";
			break;
		}
		default:
		{
			*GSMState = "...";
			break;
		}
	}	
	
	// Anzahl verbundener Knoten aktualisieren:
	NumNode = BS_NumNode;
	
	//DSP_ChangePage();
}

void DSP_ChangePage(uint8_t ID)
{
	DSP_CurrentPage=ID;
	lcd_clear(); //Löschen
	switch(ID)
	{
		case PageWelcome:
		{
			lcd_set_cursor(0,1);
			//
			lcd_set_cursor(0,2);
			CenterStringPGM(DStr_BSName,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,3);
			CenterStringPGM(DStr_BSVersion,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			//
			break;
		}

		case PageMenuHome:
		{
			lcd_set_cursor(0,1);
			CenterStringPGM(DStr_BSMenu,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,2);
			CenterStringPGM(DStr_BSHome,LineTemp,2);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,3);
			CenterStringPGM(DStr_BSFehlerliste,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			CenterStringPGM(DStr_BSEinstellungen,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			//Pfeil nach unten:
			lcd_set_cursor(0,4);
			lcd_string("v");
			break;
		}

		case PageHome:
		{
			lcd_set_cursor(0,1);
			sprintf(LineTemp,"[%s]-  GSM %s   %d ",BatState,GSMState,NumNode);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,2);
			//
			lcd_set_cursor(0,3);
			CenterStringPGM(DStr_BSName,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			sprintf(LineTemp,"  %d.%d.%d   %d:%d  ",Day,Month,Year,Minute,Hour);
			lcd_Xstring(LineTemp,0);
			break;
		}
		
		case PageMenuFehlerliste:
		{
			lcd_set_cursor(0,1);
			CenterStringPGM(DStr_BSMenu,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,2);
			CenterStringPGM(DStr_BSHome,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,3);
			CenterStringPGM(DStr_BSFehlerliste,LineTemp,2);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			CenterStringPGM(DStr_BSEinstellungen,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			//Pfeil nach unten:
			lcd_set_cursor(0,4);
			lcd_string("v");
			break;
		}

		case PageMenuEinstellungen:
		{
			lcd_set_cursor(0,1);
			CenterStringPGM(DStr_BSMenu,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,2);
			CenterStringPGM(DStr_BSFehlerliste,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,3);
			CenterStringPGM(DStr_BSEinstellungen,LineTemp,2);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			CenterStringPGM(DStr_BSWettermonitor,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			//Pfeil nach oben:
			lcd_set_cursor(0,2);
			lcd_string("^");
			break;
		}
		
		case PageMenuWettermonitor:
		{
			lcd_set_cursor(0,1);
			CenterStringPGM(DStr_BSMenu,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,2);
			CenterStringPGM(DStr_BSFehlerliste,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,3);
			CenterStringPGM(DStr_BSEinstellungen,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			CenterStringPGM(DStr_BSWettermonitor,LineTemp,2);
			lcd_Xstring(LineTemp,0);
			//Pfeil nach oben:
			lcd_set_cursor(0,2);
			lcd_string("^");
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
	if(arrows & 0x01){*Temp++='<';*Temp++='<';}
	for (uint8_t i = 0; i < (maxspace-len)/2; i++)*Temp++=32;//Leerzeichen einfügen
	for (uint8_t i = 0; i < len; i++)*Temp++=*str++;
	for (uint8_t i = 0; i < (maxspace-len)/2; i++)*Temp++=32;//Leerzeichen einfügen
	if(arrows & 0x02){*Temp++='>';*Temp++='>';}
}

void CenterStringPGM(const uint8_t* str,uint8_t* Temp,uint8_t arrows)
{
	char tt[20];
	PGM_ReadStr(str,tt,0);
	uint8_t len = strlen(tt);
	uint8_t maxspace = 20 - (arrows == 1 || arrows == 2 ? 2 : arrows == 3 ? 4 : 0);
	if(arrows & 0x01){*Temp++='<';*Temp++='<';}
	for (uint8_t i = 0; i < (maxspace-len)/2; i++)*Temp++=32;//Leerzeichen einfügen
	for (uint8_t i = 0; i < len; i++)*Temp++=tt[i];
	for (uint8_t i = 0; i < (maxspace-len)/2; i++)*Temp++=32;//Leerzeichen einfügen
	if(arrows & 0x02){*Temp++='>';*Temp++='>';}
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