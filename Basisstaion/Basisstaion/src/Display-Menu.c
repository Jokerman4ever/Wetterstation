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


#define DStr_BSName PSTR				(" MY WEATHER STATION ")
#define DStr_BSVersion PSTR				("   Version 1.0.0    ")
#define DStr_BSMenu PSTR				("      **MENU**      ")
#define DStr_BSHome PSTR				("    Home            ")
#define DStr_BSFehlerliste PSTR			("    Fehlerliste     ")
#define DStr_BSEinstellungen PSTR		("    Einstellungen   ")
#define DStr_BSWettermonitor PSTR		("    Wettermonitor   ")
#define DStr_BSMenuFehlerliste PSTR		("  **FEHLERLISTE**   ")
#define DStr_BSMenuEinstellungen PSTR	(" **Einstellungen**  ")
#define DStr_BSZurueck PSTR				("    Zurück          ")
#define DStr_BSNamenVergeben PSTR		("    Namen vergeben  ")
#define DStr_BSSpeicherverwaltung PSTR	("    Flashverwaltung ")
#define DStr_BSRFverwaltung PSTR		("    Funkverwaltung  ")
#define DStr_BSGSMverwaltung PSTR		("    GSM-Verwaltung  ")
#define DStr_BSEnergiemanagement PSTR	("    Energiemanage   ")
#define DStr_BSEinheiten PSTR			("    Einheit waehlen ")
#define DStr_BSIntervalle PSTR			("    Messintervalle  ")
#define DStr_BSSyncWort PSTR			("    Syncwort        ")
#define DStr_BSFehler PSTR				("    **Fehler   **   ")

uint8_t BatState[4] = "...";
uint8_t GSMState[4] = "...";
uint8_t NumNode = 0;
uint8_t Day[3] = "00";
uint8_t Month[3] = "00";
uint8_t Year[3] = "00";
uint8_t Minute[3] = "00";
uint8_t Hour[3] = "00";
// Fehlerliste:
uint8_t Fehler[20][50] = {};
uint8_t Anzahl_Fehler = 0;
uint8_t Fehler_i;
uint8_t Fehler_j;
uint8_t Fehler_k;

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
			lcd_string("v");
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
			lcd_set_cursor(0,3);
			sprintf(LineTemp,"    %s",Fehler[1]);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			sprintf(LineTemp,"    %s",Fehler[2]);
			lcd_Xstring(LineTemp,0);
			if(Fehler[3] != 0){
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
			sprintf(LineTemp,"    %s",Fehler[1]);
			CenterString(LineTemp,LineTemp,2);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			sprintf(LineTemp,"    %s",Fehler[2]);
			lcd_Xstring(LineTemp,0);
			if(Fehler[3] != 0){
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
			sprintf(LineTemp,"    %s",Fehler[Fehler_i]);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,3);
			sprintf(LineTemp,"    %s",Fehler[Fehler_j]);
			CenterString(LineTemp,LineTemp,2);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			sprintf(LineTemp,"    %s",Fehler[Fehler_k]);
			lcd_Xstring(LineTemp,0);
			if (Fehler_i != 0)
			{
				if(Fehler[Fehler_i-1] != 0){
					//Pfeil nach oben:
					lcd_set_cursor(0,1);
					lcd_string("^");
				}
			}
			if(Fehler_k != Anzahl_Fehler - 1){
				if(Fehler[Fehler_k+1] != 0){
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
			sprintf(LineTemp,"    %s",Fehler[Fehler_i]);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,3);
			sprintf(LineTemp,"    %s",Fehler[Fehler_j]);
			CenterString(LineTemp,LineTemp,2);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			sprintf(LineTemp,"    %s",Fehler[Fehler_k]);
			lcd_Xstring(LineTemp,0);
			if (Fehler_i != 0)
			{
				if(Fehler[Fehler_i-1] != 0){
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
			sprintf(LineTemp,"    %s",Fehler[Fehler_i]);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,3);
			sprintf(LineTemp,"    %s",Fehler[Fehler_j]);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			sprintf(LineTemp,"    %s",Fehler[Fehler_k]);
			CenterString(LineTemp,LineTemp,2);
			lcd_Xstring(LineTemp,0);
			if (Fehler_i != 0)
			{
				if(Fehler[Fehler_i-1] != 0){
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
			lcd_string("^");
			break;
		}

		case PageEinstellungen_Zurueck:
		{
			lcd_set_cursor(0,1);
			CenterStringPGM(DStr_BSEinstellungen,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,2);
			CenterStringPGM(DStr_BSZurueck,LineTemp,2);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,3);
			CenterStringPGM(DStr_BSNamenVergeben,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			CenterStringPGM(DStr_BSSpeicherverwaltung,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			//Pfeil nach unten:
			lcd_set_cursor(0,2);
			lcd_string("v");
			break;
		}
		
		case PageEinstellungen_Namen:
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

		case PageEinstellungen_Speicher:
		{
			lcd_set_cursor(0,1);
			CenterStringPGM(DStr_BSEinstellungen,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,2);
			CenterStringPGM(DStr_BSNamenVergeben,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,3);
			CenterStringPGM(DStr_BSSpeicherverwaltung,LineTemp,2);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			CenterStringPGM(DStr_BSRFverwaltung,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			//Pfeil nach oben:
			lcd_set_cursor(0,1);
			lcd_string("^");
			//Pfeil nach unten:
			lcd_set_cursor(0,2);
			lcd_string("v");
			break;
		}
		
		case PageEinstellungen_RF:
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

		case PageEinstellungen_GSM:
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

		case PageEinstellungen_Einheit:
		{
			lcd_set_cursor(0,1);
			CenterStringPGM(DStr_BSEinstellungen,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,2);
			CenterStringPGM(DStr_BSEnergiemanagement,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,3);
			CenterStringPGM(DStr_BSEinheiten,LineTemp,2);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			CenterStringPGM(DStr_BSIntervalle,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			//Pfeil nach oben:
			lcd_set_cursor(0,1);
			lcd_string("^");
			//Pfeil nach unten:
			lcd_set_cursor(0,2);
			lcd_string("v");
			break;
		}
		
		case PageEinstellungen_Intervall:
		{
			lcd_set_cursor(0,1);
			CenterStringPGM(DStr_BSEinstellungen,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,2);
			CenterStringPGM(DStr_BSEinheiten,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,3);
			CenterStringPGM(DStr_BSIntervalle,LineTemp,2);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			CenterStringPGM(DStr_BSSyncWort,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			//Pfeil nach oben:
			lcd_set_cursor(0,1);
			lcd_string("^");
			break;
		}				

		case PageEinstellungen_Sync:
		{
			lcd_set_cursor(0,1);
			CenterStringPGM(DStr_BSEinstellungen,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,2);
			CenterStringPGM(DStr_BSEinheiten,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,3);
			CenterStringPGM(DStr_BSIntervalle,LineTemp,0);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			CenterStringPGM(DStr_BSSyncWort,LineTemp,2);
			lcd_Xstring(LineTemp,0);
			//Pfeil nach oben:
			lcd_set_cursor(0,1);
			lcd_string("^");
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
			//
			lcd_set_cursor(0,2);
			//
			lcd_set_cursor(0,3);
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
			//
			lcd_set_cursor(0,2);
			//
			lcd_set_cursor(0,3);
			//
			lcd_set_cursor(0,4);
			//
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
			lcd_string("^");
			break;
		}
		
		case PageWettermonitor:	
		{
			lcd_set_cursor(0,1);
			sprintf(LineTemp,"[%s]-  GSM %s   %d ",BatState,GSMState,NumNode);
			lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,2);
			//sprintf(LineTemp,"**%s**",Name_Messstation);
			//CenterString(LineTemp,LineTemp,0);
			//lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,3);
			//sprintf(LineTemp," %d°C  %dhPa  %d% ",Temp_MS[i],Druck_MS[i],Feuchte_MS[i]);
			//lcd_Xstring(LineTemp,0);
			lcd_set_cursor(0,4);
			//sprintf(LineTemp," %s %dkm/h    ",Licht_MS[i],Wind_MS[i]); //Licht muss immer 7 Zeichen beinhalten ("Sonnig ","Regen  ","Bewölkt")
			//lcd_Xstring(LineTemp,0);
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
				//-
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
		
		/*
		// TabEinstellungen:
		PageMenuEinstellungen = 30,
		PageEinstellungen_Zurueck = 31,
		PageEinstellungen_Namen = 32,
		PageEinstellungen_Speicher = 33,
		PageEinstellungen_RF = 34,
		PageEinstellungen_GSM = 35,
		PageEinstellungen_Energie = 36,
		PageEinstellungen_Einheit = 37,
		PageEinstellungen_Intervall = 38,
		PageEinstellungen_Sync = 39,
		PageSet_Namen = 40,
		PageSet_Speicher = 41,
		PageSet_RF = 42,
		PageSet_GSM = 43,
		PageSet_Energie = 44,
		PageSet_Einheit = 45,
		PageSet_Intervall = 46,
		PageSet_Sync = 47,
		// TabWettermonitor:
		PageMenuWettermonitor = 50,
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
			DSP_ChangePage(PageEinstellungen_Speicher);
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
			DSP_ChangePage(PageEinstellungen_Sync);
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