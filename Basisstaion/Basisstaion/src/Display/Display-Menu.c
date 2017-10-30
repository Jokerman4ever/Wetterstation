/*
 * Display_Menu.c
 *
 * Created: 02.01.2017 11:14:32
 *  Author: Felix
 */ 
#include "Display-Menu.h"
#include "lcd-routines.h"
#include "Clock/Xdelay.h"
#include "Storage/FileSys.h"
#include "ErrorList.h"
#include "RF.h"
#include "GSM/com.h"

uint8_t LineTemp[20];

void PGM_ReadStr(const char* str,char* dest,uint8_t start)
{
	uint8_t i=0;
	do 
	{
		dest[start+i]=pgm_read_byte((PGM_P)str++);
		i++;
	} while (dest[start + i -1]);
}


#define DStr_BSName PSTR				(" MY WEATHER STATION ")
#define DStr_BSVersion PSTR				("   Version 1.0.0    ")
#define DStr_BSMenu PSTR				("**MENU**")
#define DStr_BSHome PSTR				("    Home            ")
#define DStr_BSFehlerliste PSTR			("    Fehlerliste     ")
#define DStr_BSEinstellungen PSTR		("    Einstellungen   ")
#define DStr_BSWettermonitor PSTR		("    Wettermonitor   ")
#define DStr_BSMenuFehlerliste PSTR		("  **FEHLERLISTE**   ")
#define DStr_BSMenuEinstellungen PSTR	("  **Einstellung**   ")
#define DStr_BSZurueck PSTR				("    Zurueck         ")
#define DStr_BSNamenVergeben PSTR		("    Namen vergeben  ")
#define DStr_BSSpeicherverwaltung PSTR	("    Flashverwaltung ")
#define DStr_BSMenuSpeicherverwaltung PSTR	("**Flashverwaltung** ")
#define DStr_BSRFverwaltung PSTR		("    Funkverwaltung  ")
#define DStr_BSGSMverwaltung PSTR		("    GSM-Verwaltung  ")
#define DStr_BSEnergiemanagement PSTR	("    Energiemanage   ")
#define DStr_BSEinheiten PSTR			("    Einheit waehlen ")
#define DStr_BSIntervalle PSTR			("    Messintervalle  ")
#define DStr_BSSyncWort PSTR			("    Syncwort        ")
#define DStr_BSMenuSyncWort PSTR		("    **Syncwort**    ")
#define DStr_BSFehler PSTR				("    **Fehler   **   ")
#define DStr_BSSpeicherLoeschen PSTR	(" Speicher Loeschen? ")

char BatState[4] = "...";
char GSMState[4] = "...";
uint8_t NumNode = 0;
char Day[3] = "00";
char Month[3] = "00";
char Year[3] = "00";
char Minute[3] = "00";
char Hour[3] = "00";
// Fehlerliste:
uint8_t DSP_Fehler[3][20] = {};
uint8_t Anzahl_Fehler = 0;
uint8_t Fehler_i;
uint8_t Fehler_j;
uint8_t Fehler_k;
uint8_t ScrollPosition;
uint8_t down = 0x1E;
uint8_t up = 0x1D;

tm_t DSP_Time_Ms;	

uint8_t DSP_MenuSelection;
uint8_t DSP_CurrentPage;

int8_t Temp_MS=0;
uint8_t Temp_10_MS=0;
uint16_t Druck_MS=0;
uint8_t Feuchte_MS=0;
uint8_t Wind_MS=0;
uint8_t ID_MS=0xFF;
uint8_t Weather_State[8];
uint8_t big_station = 0;

void DSP_Refresh_Timer_Init()
{
	sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_TC0); //TC1 SysClock Enable
	TCC0.CTRLA = TC_CLKSEL_DIV1024_gc; //Presackler
	TCC0.CTRLB = TC_WGMODE_NORMAL_gc;
	TCC0.PER = XDELAY_ISFAST ? 32000 : 2000; //Zähler Top-Wert
	TCC0.CNT = 0x00; //Reset Zähler-Wert
	TCC0.INTCTRLA = TC_OVFINTLVL_MED_gc;
}

void DSP_Update_Weatherdata(FS_StationRecord_t* data)
{
	Druck_MS = data->Pressure;
	Temp_MS = data->Temperature / 10;
	Temp_10_MS = data->Temperature % 10;
	Feuchte_MS = data->Humidity / 10;
	
	
	if(data->RainState > 128)
	{
		sprintf(Weather_State,"Regen  ");
	}
	else if(data->LightStrength > 128)
	{
		sprintf(Weather_State,"Sonnig ");
	}
	else
	{
		sprintf(Weather_State,"Wolkig ");
	}
	Wind_MS = data->WindLevel;
	
	if(data->WindLevel == 0 && data->LightStrength == 0 && data->RainState == 0)
	{
		big_station = 0;
	} else {big_station = 1;}
	
	ID_MS = data->ID;
	if(DSP_CurrentPage == PageWettermonitor){DSP_ChangePage(PageWettermonitor);}
}

// Muss im Sekundentakt aufgerufen werden (oder schneller)
void DSP_Refresh(uint8_t BS_BatState,uint8_t BS_GSMState,uint8_t BS_NumNode)
{
	if(BS_GSMState < 2)BS_GSMState = GSMNone;
	else if(BS_GSMState >= 2 && BS_GSMState < 11)BS_GSMState = GSMLow;
	else if(BS_GSMState >= 11 && BS_GSMState < 20)BS_GSMState = GSMMid;
	else if(BS_GSMState >= 20)BS_GSMState = GSMHigh;
	
	
	time_GetLocalTime(&DSP_Time_Ms);

	// Datum und Zeit aktualisieren:	
	sprintf(Day,"%02d",DSP_Time_Ms.tm_mday);
	sprintf(Month,"%02d",DSP_Time_Ms.tm_mon);
	sprintf(Year, "%02d",(DSP_Time_Ms.tm_year-2000));
	sprintf(Minute,"%02d",DSP_Time_Ms.tm_min);
	sprintf(Hour,"%02d",DSP_Time_Ms.tm_hour);	
	
	// Batteriestatus aktualisieren:
	switch(BS_BatState)
	{
		case BatLow:
		{
			DSP_CopyString("o..",BatState);
			break;
		}
		case BatMid:
		{
			DSP_CopyString("oo.",BatState);
			break;
		}
		case BatHigh:
		{
			DSP_CopyString("ooo",BatState);
			break;
		}
		default:
		{
			DSP_CopyString("...",BatState);
			break;
		}
	}
	
	// GSM Sendeleistung aktualisieren
	switch(BS_GSMState)
	{
		case GSMLow:
		{
			DSP_CopyString("I..",GSMState);
			break;
		}
		case GSMMid:
		{
			DSP_CopyString("II.",GSMState);
			break;
		}
		case GSMHigh:
		{
			DSP_CopyString("III",GSMState);
			break;
		}
		case GSMNone: default:
		{
			DSP_CopyString("...",GSMState);
			break;
		}
	}	
	
	// Anzahl verbundener Knoten aktualisieren:
	NumNode = BS_NumNode;
	if(DSP_Time_Ms.tm_sec == 0)
	{
		if(DSP_CurrentPage == PageHome){DSP_ChangePage(PageHome);}
		if(DSP_CurrentPage == PageWettermonitor){DSP_ChangePage(PageWettermonitor);}
	}
}


// String Kopieren:
void DSP_CopyString(const char* str, char* strout)
{
	uint8_t length = strlen(str);
	for (uint8_t i = 0; i < length; i++)
	{
		*strout++=*str++;	
	}
}



void DSP_ChangePage(uint8_t ID)
{
	DSP_CurrentPage=ID;
	lcd_clear(); //Löschen
	lcd_home();
	switch(ID)
	{
		// Welcome:
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
		
		// TabHome:
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
			lcd_Write(down,1);
			break;
		}

		case PageHome:
		{
			lcd_set_cursor(0,2);
			CenterStringPGM(DStr_BSName,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			
			lcd_set_cursor(0,1);
			sprintf(LineTemp,"GSM %s          %d ",GSMState,NumNode); 
			lcd_Xstring(LineTemp,0);
			//lcd_set_cursor(0,2);
			
			lcd_set_cursor(0,4);
			sprintf(LineTemp,"IP: %s",ip_adresse);
			lcd_Xstring(LineTemp,0);
			
			lcd_set_cursor(0,3);
			sprintf(LineTemp,"  %s.%s.%s   %s:%s  ",Day,Month,Year,Hour,Minute);//das wird so nicht gehen, %d steht für decimal also eine zahl.... du übergibst nen string!!!
			lcd_Xstring(LineTemp,0);
			
			
			break;
		}

		// TabeFehler:		
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
			lcd_Write(down,1);
			break;
		}
		
		case PageFehlerliste_Oben_Oben:
		{
			lcd_set_cursor(0,1);
			CenterStringPGM(DStr_BSMenuFehlerliste,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,2);
			CenterStringPGM(DStr_BSZurueck,LineTemp,2);
			lcd_Xstring(LineTemp,0);
			DSP_FillErrorArray(0);//Fill array
			lcd_set_cursor(0,3);
			sprintf(LineTemp,"    %s",DSP_Fehler[0]);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			sprintf(LineTemp,"    %s",DSP_Fehler[1]);
			lcd_Xstring(LineTemp,0);
			if(DSP_Fehler[2] != 0){
				//Pfeil nach unten:
				lcd_set_cursor(0,4);
				lcd_string("v");
			}
			break;
		}

		case PageFehlerliste_Oben_Mitte:
		{
			lcd_set_cursor(0,1);
			CenterStringPGM(DStr_BSMenuFehlerliste,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,2);
			CenterStringPGM(DStr_BSZurueck,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,3);
			DSP_FillErrorArray(0);//Fill array
			sprintf(LineTemp,"    %s",DSP_Fehler[0]);
			CenterString(LineTemp,LineTemp,2);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			sprintf(LineTemp,"    %s",DSP_Fehler[1]);
			lcd_Xstring(LineTemp,0);
			if(DSP_Fehler[2] != 0){
				//Pfeil nach unten:
				lcd_set_cursor(0,4);
				lcd_string("v");
			}
			break;
		}

		case PageFehlerliste_Mitte:
		{
			lcd_set_cursor(0,1);
			CenterStringPGM(DStr_BSMenuFehlerliste,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,2);
			sprintf(LineTemp,"    %s",DSP_Fehler[Fehler_i]);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,3);
			sprintf(LineTemp,"    %s",DSP_Fehler[Fehler_j]);
			CenterString(LineTemp,LineTemp,2);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			sprintf(LineTemp,"    %s",DSP_Fehler[Fehler_k]);
			lcd_Xstring(LineTemp,0);
			if (Fehler_i != 0)
			{
				if(DSP_Fehler[Fehler_i-1] != 0){
					//Pfeil nach oben:
					lcd_set_cursor(0,1);
					lcd_string("^");
				}
			}
			if(Fehler_k != Anzahl_Fehler - 1){
				if(DSP_Fehler[Fehler_k+1] != 0){
					//Pfeil nach unten:
					lcd_set_cursor(0,4);
					lcd_string("v");
				}
			}
			break;
		}

		case PageFehlerliste_Unten_Mitte:
		{
			lcd_set_cursor(0,1);
			CenterStringPGM(DStr_BSMenuFehlerliste,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,2);
			sprintf(LineTemp,"    %s",DSP_Fehler[Fehler_i]);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,3);
			sprintf(LineTemp,"    %s",DSP_Fehler[Fehler_j]);
			CenterString(LineTemp,LineTemp,2);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			sprintf(LineTemp,"    %s",DSP_Fehler[Fehler_k]);
			lcd_Xstring(LineTemp,0);
			if (Fehler_i != 0)
			{
				if(DSP_Fehler[Fehler_i-1] != 0){
					//Pfeil nach oben:
					lcd_set_cursor(0,1);
					lcd_string("^");
				}
			}
			break;
		}
		
		case PageFehlerliste_Unten_Unten:
		{
			lcd_set_cursor(0,1);
			CenterStringPGM(DStr_BSMenuFehlerliste,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,2);
			sprintf(LineTemp,"    %s",DSP_Fehler[Fehler_i]);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,3);
			sprintf(LineTemp,"    %s",DSP_Fehler[Fehler_j]);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			sprintf(LineTemp,"    %s",DSP_Fehler[Fehler_k]);
			CenterString(LineTemp,LineTemp,2);
			lcd_Xstring(LineTemp,0);
			if (Fehler_i != 0)
			{
				if(DSP_Fehler[Fehler_i-1] != 0){
					//Pfeil nach oben:
					lcd_set_cursor(0,1);
					lcd_string("^");
				}
			}
			break;
		}
		
		case PageFehler:
		{
			lcd_set_cursor(0,1);
			CenterStringPGM(DStr_BSFehler,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,2);
			//
			lcd_set_cursor(0,3);
			//
			lcd_set_cursor(0,4);
			//
		}

		// TabEinstellungen:
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
			lcd_Write(up, 1);
			break;
		}

		case PageEinstellungen_Zurueck:
		{//+++
			lcd_set_cursor(0,1);
			CenterStringPGM(DStr_BSMenuEinstellungen,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,2);
			CenterStringPGM(DStr_BSZurueck,LineTemp,2);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,3);
			CenterStringPGM(DStr_BSSpeicherverwaltung,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			CenterStringPGM(DStr_BSIntervalle,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			//Pfeil nach unten:
			lcd_set_cursor(0,4);
			lcd_Write(down,1);
			break;
		}
		
		/*case PageEinstellungen_Namen:
		{
			lcd_set_cursor(0,1);
			CenterStringPGM(DStr_BSEinstellungen,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,2);
			CenterStringPGM(DStr_BSZurueck,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,3);
			CenterStringPGM(DStr_BSNamenVergeben,LineTemp,2);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			CenterStringPGM(DStr_BSSpeicherverwaltung,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			//Pfeil nach unten:
			lcd_set_cursor(0,2);
			lcd_string("v");
			break;
		}
*/
		case PageEinstellungen_Speicher:
		{//++
			lcd_set_cursor(0,1);
			CenterStringPGM(DStr_BSMenuEinstellungen,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,2);
			CenterStringPGM(DStr_BSZurueck,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,3);
			CenterStringPGM(DStr_BSSpeicherverwaltung,LineTemp,2);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			CenterStringPGM(DStr_BSIntervalle,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			//Pfeil nach unten:
			lcd_set_cursor(0,4);
			lcd_Write(down,1);
			break;
		}
		
		/*case PageEinstellungen_RF:
		{
			lcd_set_cursor(0,1);
			CenterStringPGM(DStr_BSEinstellungen,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,2);
			CenterStringPGM(DStr_BSSpeicherverwaltung,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,3);
			CenterStringPGM(DStr_BSRFverwaltung,LineTemp,2);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			CenterStringPGM(DStr_BSGSMverwaltung,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			//Pfeil nach oben:
			lcd_set_cursor(0,1);
			lcd_string("^");
			//Pfeil nach unten:
			lcd_set_cursor(0,2);
			lcd_string("v");
			break;
		}
*/
		/*case PageEinstellungen_GSM:
		{
			lcd_set_cursor(0,1);
			CenterStringPGM(DStr_BSEinstellungen,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,2);
			CenterStringPGM(DStr_BSRFverwaltung,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,3);
			CenterStringPGM(DStr_BSGSMverwaltung,LineTemp,2);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			CenterStringPGM(DStr_BSEnergiemanagement,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			//Pfeil nach oben:
			lcd_set_cursor(0,1);
			lcd_string("^");
			//Pfeil nach unten:
			lcd_set_cursor(0,2);
			lcd_string("v");
			break;
		}										

		case PageEinstellungen_Energie:
		{
			lcd_set_cursor(0,1);
			CenterStringPGM(DStr_BSEinstellungen,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,2);
			CenterStringPGM(DStr_BSGSMverwaltung,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,3);
			CenterStringPGM(DStr_BSEnergiemanagement,LineTemp,2);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			CenterStringPGM(DStr_BSEinheiten,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			//Pfeil nach oben:
			lcd_set_cursor(0,1);
			lcd_string("^");
			//Pfeil nach unten:
			lcd_set_cursor(0,2);
			lcd_string("v");
			break;
		}
*/
		
		case PageEinstellungen_Intervall:
		{
			lcd_set_cursor(0,1);
			CenterStringPGM(DStr_BSMenuEinstellungen,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,2);
			CenterStringPGM(DStr_BSSpeicherverwaltung,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,3);
			CenterStringPGM(DStr_BSIntervalle,LineTemp,2);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			CenterStringPGM(DStr_BSEinheiten,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			//Pfeil nach oben:
			lcd_set_cursor(0,2);
			lcd_Write(up,1);
			//Pfeil nach unten:
			lcd_set_cursor(0,4);
			lcd_Write(down,1);
			break;
		}	
		
		case PageEinstellungen_Einheit:
		{
			lcd_set_cursor(0,1);
			CenterStringPGM(DStr_BSMenuEinstellungen,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,2);
			CenterStringPGM(DStr_BSIntervalle,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,3);
			CenterStringPGM(DStr_BSEinheiten,LineTemp,2);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			CenterStringPGM(DStr_BSSyncWort,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			//Pfeil nach oben:
			lcd_set_cursor(0,2);
			lcd_Write(up,1);
			break;
		}			

		case PageEinstellungen_Sync:
		{
			lcd_set_cursor(0,1);
			CenterStringPGM(DStr_BSMenuEinstellungen,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,2);
			CenterStringPGM(DStr_BSIntervalle,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,3);
			CenterStringPGM(DStr_BSEinheiten,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			CenterStringPGM(DStr_BSSyncWort,LineTemp,2);
			lcd_Xstring(LineTemp,0);
			//Pfeil nach oben:
			lcd_set_cursor(0,2);
			lcd_Write(up,1);
			break;
		}
		
		case PageSet_Namen:
		{
			lcd_set_cursor(0,1);
			//
			lcd_set_cursor(0,2);
			//
			lcd_set_cursor(0,3);
			//
			lcd_set_cursor(0,4);
			//
			break;
		}
		
		case PageSet_Speicher:
		{
			lcd_set_cursor(0,1);
			CenterStringPGM(DStr_BSMenuSpeicherverwaltung,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			//
			lcd_set_cursor(0,2);
			CenterStringPGM(DStr_BSZurueck,LineTemp,ScrollPosition == 0 ? 2 : 0);
			lcd_Xstring(LineTemp,0);
			//
			lcd_set_cursor(0,3);
			if(ScrollPosition == 0)sprintf(LineTemp,"    LOESCHEN: NEIN");
			else sprintf(LineTemp,">>  LOESCHEN: %s",ScrollPosition == 1 ? "NEIN": "JA");
			lcd_Xstring(LineTemp,0);
			//
			lcd_set_cursor(0,4);
			//
			break;
		}
		
		case PageSet_RF:
		{
			lcd_set_cursor(0,1);
			//
			lcd_set_cursor(0,2);
			//
			lcd_set_cursor(0,3);
			//
			lcd_set_cursor(0,4);
			//
			break;
		}
		
		case PageSet_GSM:
		{
			lcd_set_cursor(0,1);
			//
			lcd_set_cursor(0,2);
			//
			lcd_set_cursor(0,3);
			//
			lcd_set_cursor(0,4);
			//
			break;
		}
		
		case PageSet_Energie:
		{
			lcd_set_cursor(0,1);
			//
			lcd_set_cursor(0,2);
			//
			lcd_set_cursor(0,3);
			//
			lcd_set_cursor(0,4);
			//
			break;
		}
		
		case PageSet_Einheit:
		{
			lcd_set_cursor(0,1);
			//
			lcd_set_cursor(0,2);
			//
			lcd_set_cursor(0,3);
			//
			lcd_set_cursor(0,4);
			//
			break;
		}
		
		case PageSet_Intervall:
		{
			lcd_set_cursor(0,1);
			//
			lcd_set_cursor(0,2);
			//
			lcd_set_cursor(0,3);
			//
			lcd_set_cursor(0,4);
			//
			break;
		}
		
		case PageSet_Sync:
		{
			lcd_set_cursor(0,1);
			CenterStringPGM(DStr_BSMenuSyncWort,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			//
			lcd_set_cursor(0,2);
			sprintf(LineTemp,"  Aktuell: %s",RF_Syncwords[RF_CurrentStatus.CurrentSyncword]);
			lcd_Xstring(LineTemp,0);
			
			lcd_set_cursor(0,3);
			sprintf(LineTemp,"  Neu    : %s",RF_Syncwords[ScrollPosition]);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			//
			break;
		}
		
		case PageSet_Speicher_Loeschen:
		{
			lcd_set_cursor(0,1);
			CenterStringPGM(DStr_BSMenuSpeicherverwaltung,LineTemp,0);
			lcd_Xstring(LineTemp,0); 
			lcd_set_cursor(0,2);
			CenterStringPGM(DStr_BSSpeicherLoeschen,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,3);
			if(ScrollPosition == 0)sprintf(LineTemp,">>  NEIN?");
			else sprintf(LineTemp,"    NEIN?");
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			if(ScrollPosition == 1)sprintf(LineTemp, ">>  JA?");
			else sprintf(LineTemp, "    JA?");
			lcd_Xstring(LineTemp,0);
			break;
		}	
				
	
		// TabWettermonitor:		
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
			lcd_Write(up, 1);
			break;
		}
		
		case PageWettermonitor:	
		{

			if(big_station)
			{
				lcd_set_cursor(0,4);
				sprintf(LineTemp," %s %3dkm/h    ",Weather_State, Wind_MS); //Weather_State muss immer 7 Zeichen beinhalten ("Sonnig ","Regen  ","Bewölkt")
				lcd_Xstring(LineTemp,0);
			}
			lcd_set_cursor(0,1);
			sprintf(LineTemp,"GSM %s          %d ",GSMState,NumNode);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,2);
			sprintf(LineTemp,"   **MS ID: %3d**   ",ID_MS);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,3);
			sprintf(LineTemp, " %d,%dC %dhPa %d%%",Temp_MS,Temp_10_MS, Druck_MS,Feuchte_MS);
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
		case PageMenuHome:
		{
			if(dir == 1)
			{
				DSP_ChangePage(PageMenuFehlerliste);
			}
			else
			{
				//
			}
			break;
		}
		
		case PageMenuFehlerliste:
		{
			if(dir == 1)
			{
				DSP_ChangePage(PageMenuEinstellungen);
			}
			else
			{
				DSP_ChangePage(PageMenuHome);
			}
			break;
		}
		
		case PageFehlerliste_Oben_Oben:
		{
			if(dir == 1)
			{
				DSP_ChangePage(PageFehlerliste_Oben_Mitte);
			}
			else
			{
				//-
			}
			break;
		}
		
		case PageFehlerliste_Oben_Mitte:
		{
			if(dir == 1)
			{
				DSP_ChangePage(PageFehlerliste_Mitte);
			}
			else
			{
				DSP_ChangePage(PageFehlerliste_Oben_Oben);
			}
			break;
		}
		
		case PageFehlerliste_Mitte:
		{
			if(dir == 1)
			{
				DSP_ChangePage(PageFehlerliste_Unten_Mitte);
			}
			else
			{
				DSP_ChangePage(PageFehlerliste_Oben_Mitte);
			}
			break;
		}
		
		case PageFehlerliste_Unten_Mitte:
		{
			if(dir == 1)
			{
				DSP_ChangePage(PageFehlerliste_Unten_Unten);
			}
			else
			{
				DSP_ChangePage(PageFehlerliste_Mitte);
			}
			break;
		}
		
		case PageFehlerliste_Unten_Unten:
		{
			if (dir == 1)
			{
				//-
			}
			else
			{
				DSP_ChangePage(PageFehlerliste_Unten_Mitte);
			}
			break;
		}
		
		
		case PageMenuEinstellungen:
		{
			if(dir == 1)
			{
				DSP_ChangePage(PageMenuWettermonitor);
			}
			else
			{
				DSP_ChangePage(PageMenuFehlerliste);
			}
			break;
		}
		
		case PageEinstellungen_Zurueck:
		{
			if(dir == 1)
			{
				DSP_ChangePage(PageEinstellungen_Speicher);
			}
			else
			{
				
			}
			break;
		}
		case PageEinstellungen_Speicher:
		{
			if(dir == 1)
			{
				DSP_ChangePage(PageEinstellungen_Intervall);
			}
			else
			{
				DSP_ChangePage(PageEinstellungen_Zurueck);
			}
			break;
		}
		case PageEinstellungen_Intervall:
		{
			if(dir == 1)
			{
				DSP_ChangePage(PageEinstellungen_Einheit);
			}
			else
			{
				DSP_ChangePage(PageEinstellungen_Speicher);
			}
			break;
		}
		case PageEinstellungen_Einheit:
		{
			if(dir == 1)
			{				
				DSP_ChangePage(PageEinstellungen_Sync);
			}
			else
			{

				DSP_ChangePage(PageEinstellungen_Intervall);
			}
			break;
		}
		case PageEinstellungen_Sync:
		{
			if(dir == 1)
			{
				
			}
			else
			{
				DSP_ChangePage(PageEinstellungen_Einheit);
			}
			break;
		}
		case PageSet_Sync:
		{			
			if(dir == 1)
			{
				if(ScrollPosition < 15)ScrollPosition++;
				else ScrollPosition = 0;
			}
			else
			{
				if(ScrollPosition > 0)ScrollPosition--;
				else ScrollPosition = 15;
			}
			DSP_ChangePage(PageSet_Sync);
			break;
		}
		case PageSet_Speicher:
		{
			if(dir == 1)
			{
				if(ScrollPosition < 2)ScrollPosition++;
			}
			else
			{
				if(ScrollPosition > 0)ScrollPosition--;
			}
			DSP_ChangePage(PageSet_Speicher);
			break;
		}
		case PageSet_Speicher_Loeschen:
		{
			if(dir == 1)
			{
				if(ScrollPosition < 1)ScrollPosition++;
			}
			else
			{
				if(ScrollPosition > 0)ScrollPosition--;
			}
			DSP_ChangePage(PageSet_Speicher_Loeschen);
			break;
		}
		
		
		/*
		// TabEinstellungen:
		PageEinstellungen_Zurueck = 31,
		PageEinstellungen_Namen = 32,---
		PageEinstellungen_Speicher = 33, Flash löschen?
		PageEinstellungen_RF = 34, ------
		PageEinstellungen_GSM = 35,-----
		PageEinstellungen_Energie = 36,----
		PageEinstellungen_Einheit = 37,+++
		PageEinstellungen_Intervall = 38,+++
		PageEinstellungen_Sync = 39,++
		PageSet_Namen = 40,
		PageSet_Speicher = 41,
		PageSet_RF = 42,
		PageSet_GSM = 43,
		PageSet_Energie = 44,
		PageSet_Einheit = 45,
		PageSet_Intervall = 46,
		PageSet_Sync = 47,
		// TabWettermonitor:
		*/
		case PageMenuWettermonitor:
		{
			if(dir == 1)
			{
				//
			}
			else
			{
				DSP_ChangePage(PageMenuEinstellungen);
			}
			break;
		}
		/*
		PageWettermonitor = 51,
		*/
		
		default: break;
	}
}

void DSP_SelectMenu(void)
{
	switch(DSP_CurrentPage)
	{
		// TabHome:
		case PageMenuHome:
		{
			DSP_ChangePage(PageHome);
			break;
		}
		
		case PageHome:
		{			
			DSP_ChangePage(PageMenuHome);
			break;
		}
		
		// TabeFehler:
		case PageMenuFehlerliste:
		{
			DSP_ChangePage(PageFehlerliste_Oben_Oben);
			break;
		}
		
		case PageFehlerliste_Oben_Oben:
		{
			DSP_ChangePage(PageMenuFehlerliste);
			break;
		}
		
		case PageFehlerliste_Oben_Mitte:
		{
			DSP_ChangePage(PageFehler);
			break;
		}
		
		case PageFehlerliste_Mitte:
		{
			DSP_ChangePage(PageFehler);
			break;
		}
		
		case PageFehlerliste_Unten_Mitte:
		{
			DSP_ChangePage(PageFehler);
			break;
		}
		
		case PageFehlerliste_Unten_Unten:
		{
			DSP_ChangePage(PageFehler);
			break;
		}
		
		case PageFehler:
		{
			// Zurückkehren zum Ausgangspunkt wäre schöner
			DSP_ChangePage(PageFehlerliste_Oben_Oben);
			break;
		}
		
		// TabEinstellungen:
		case PageMenuEinstellungen:
		{
			DSP_ChangePage(PageEinstellungen_Zurueck);
			break;
		}
		
		case PageEinstellungen_Zurueck:
		{
			DSP_ChangePage(PageMenuEinstellungen);
			break;
		}
		
		case PageEinstellungen_Namen:
		{
			DSP_ChangePage(PageSet_Namen);
			break;
		}
		
		case PageEinstellungen_Speicher:
		{
			ScrollPosition = 0;
			DSP_ChangePage(PageSet_Speicher);
			break;
		}
		
		case PageEinstellungen_RF:
		{
			DSP_ChangePage(PageSet_RF);
			break;	
		}
		
		case PageEinstellungen_GSM:
		{
			DSP_ChangePage(PageSet_GSM);
			break;
		}
		
		case PageEinstellungen_Energie:
		{
			DSP_ChangePage(PageSet_Energie);
			break;
		}
		
		case PageEinstellungen_Einheit:
		{
			DSP_ChangePage(PageSet_Einheit);
			break;
		}
		
		case PageEinstellungen_Intervall:
		{
			DSP_ChangePage(PageSet_Intervall);
			break;
		}
		
		case PageEinstellungen_Sync:
		{
			ScrollPosition = RF_CurrentStatus.CurrentSyncword;
			DSP_ChangePage(PageSet_Sync);
			break;
		}
		
// Start Provisorium
		case PageSet_Namen:
		{
			DSP_ChangePage(PageEinstellungen_Namen);
			break;
		}
		
		case PageSet_Speicher:
		{
			if(ScrollPosition==0)DSP_ChangePage(PageEinstellungen_Speicher);
			else
			{
				if(ScrollPosition == 2){ScrollPosition = 0; DSP_ChangePage(PageSet_Speicher_Loeschen);}
			}
			break;
		}
		
		case PageSet_RF:
		{
			DSP_ChangePage(PageEinstellungen_RF);
			break;
		}
		
		case PageSet_GSM:
		{
			DSP_ChangePage(PageEinstellungen_GSM);
			break;
		}
		
		case PageSet_Energie:
		{
			DSP_ChangePage(PageEinstellungen_Energie);
			break;
		}
		
		case PageSet_Einheit:
		{
			DSP_ChangePage(PageEinstellungen_Einheit);
			break;
		}
		
		case PageSet_Intervall:
		{
			DSP_ChangePage(PageEinstellungen_Intervall);
			break;
		}
		
		case PageSet_Sync:
		{
			RF_CurrentStatus.CurrentSyncword = ScrollPosition;
			RF_Set_Sync_Num(ScrollPosition);
			EEPROM_WriteByte(14,ScrollPosition);
			DSP_ChangePage(PageEinstellungen_Sync);
			break;
		}			
		
		case PageSet_Speicher_Loeschen:
		{
			
			if(ScrollPosition == 0)
			{
				DSP_ChangePage(PageEinstellungen_Speicher);
			}
			else
			{
				FS_FirstRun();
				DSP_ChangePage(PageHome);
			}
			break;
		}					
// Stop Provisorium

		// TabWettermonitor:
		case PageMenuWettermonitor:
		{
			DSP_ChangePage(PageWettermonitor);
			break;
		}
		
		case PageWettermonitor:
		{
			DSP_ChangePage(PageMenuWettermonitor);
			break;
		}		

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

void CenterStringPGM(const char* str,uint8_t* Temp,uint8_t arrows)
{
	char tt[20];
	uint8_t offset = 0;
	PGM_ReadStr(str,tt,0);
	uint8_t len = strlen(tt);
	uint8_t maxspace = 20 - (((arrows == 1) || (arrows == 2)) ? 2 : ((arrows == 3) ? 4 : 0));
	if(arrows & 0x02){*Temp++='>';*Temp++='>';}
	if(len > maxspace){offset = len - maxspace;}
	for (uint8_t i = 0; i < (maxspace-len)/2; i++)*Temp++=32;//Leerzeichen einfügen
	for (uint8_t i = offset; i < len; i++)*Temp++=tt[i];
	for (uint8_t i = 0; i < (maxspace-len)/2; i++)*Temp++=32;//Leerzeichen einfügen
	if(arrows & 0x01){*Temp++='<';*Temp++='<';}
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

void DSP_FillErrorArray(uint16_t index)
{
	FS_ErrorRecord_t ERROR;
	for (uint8_t i = 0; i < 3; i++)
	{
		FS_GetError(i,&ERROR);
		switch(ERROR.ID)
		{
			case ERRORID_LOCAL_BATTERYLOW: sprintf(DSP_Fehler[i],"Batterie Leer"); break;
			default: break;
		}
	}

}