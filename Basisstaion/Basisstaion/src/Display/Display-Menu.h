/*
 * Display_Menu.h
 *
 * Created: 02.01.2017 11:14:43
 *  Author: Felix
 */ 


#ifndef DISPLAYMENU_H_
#define DISPLAYMENU_H_

#include <asf.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include "time.h"
#include "string.h"
#include "Storage/FileSys.h"

enum PageID {
				// Welcome:
				PageWelcome = 1,
				PageGSMInit = 2,
				// TabHome:
				PageMenuHome = 10,
				PageHome = 11,
				// TabeFehler:
				PageMenuFehlerliste = 20,
				PageFehlerliste_Oben_Oben = 21,
				PageFehlerliste_Oben_Mitte = 22,
				PageFehlerliste_Mitte = 23,
				PageFehlerliste_Unten_Mitte = 24,
				PageFehlerliste_Unten_Unten = 25,
				PageFehler = 26,				
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
				PageSet_Speicher_Loeschen = 48,
				// TabWettermonitor:
				PageMenuWettermonitor = 50,
				PageWettermonitor = 51,

			};
			
enum BatLvl {
				BatLow = 1,		// Batterie Basisstation leer
				BatMid = 2,		// Batterie Basisstation mittel
				BatHigh = 3,	// Batterie Basisstation voll
	};

enum GSMLvl {
				GSMNone = 0,
				GSMLow = 1,		// GSM Sendeleistung niedrig
				GSMMid = 2,		// GSM Sendeleistung mittel
				GSMHigh = 3,	// GSM Sendeleistung hoch
		};

	
void PGM_ReadStr(const char* str,char* dest,uint8_t start);
void CenterStringPGM(const char* str, uint8_t* Temp,uint8_t arrows);
void CenterString(char* str,char* Temp,uint8_t arrows);
void SelectString(char* str,uint8_t* Temp);
void SelectStringPGM(const uint8_t* str,uint8_t* Temp);
void DSP_ChangePage(uint8_t ID);
void DSP_ScrollMenu(uint8_t dir);
void DSP_SelectMenu(void);
void DSP_Refresh(uint8_t BS_BatState, uint8_t BS_GSMState, uint8_t BS_NumNode);
void DSP_CopyString(const char* str, char* strout);
void DSP_Refresh_Timer_Init(void);
void DSP_Update_Weatherdata(FS_StationRecord_t* data);
void DSP_FillErrorArray(uint16_t id);
extern uint8_t DSP_CurrentPage;
#endif /*DISPLAYMENU_H_*/