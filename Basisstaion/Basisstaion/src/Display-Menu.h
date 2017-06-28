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

enum PageID {
				// Welcome:
				PageWelcome = 1,
				// TabHome:
				PageMenuHome = 10,
				PageHome = 11,
				// TabeFehler:
				PageMenuFehlerliste = 20,
				PageFehlerliste = 21,
				// TabEinstellungen:
				PageMenuEinstellungen = 30,
				PageEinstellungen = 31,
				// TabWettermonitor:
				PageMenuWettermonitor = 40,
				PageWettermonitor = 41,
			};
			
enum BatLvl {
				BatLow = 1,		// Batterie Basisstation leer
				BatMid = 2,		// Batterie Basisstation mittel
				BatHigh = 3,	// Batterie Basisstation voll
	};

enum GSMLvl {
				GSMLow = 1,		// GSM Sendeleistung niedrig
				GSMMid = 2,		// GSM Sendeleistung mittel
				GSMHigh = 3,	// GSM Sendeleistung hoch
		};

	
void PGM_ReadStr(const uint8_t* str,char* dest,uint8_t start);
void CenterStringPGM(const uint8_t* str, uint8_t* Temp,uint8_t arrows);
void CenterString(char* str,char* Temp,uint8_t arrows);
void SelectString(char* str,uint8_t* Temp);
void SelectStringPGM(const uint8_t* str,uint8_t* Temp);
void DSP_ChangePage(uint8_t ID);
void DSP_ScrollMenu(uint8_t dir);
void DSP_SelectMenu(void);
void DSP_Refresh(uint8_t BS_BatState, uint8_t BS_GSMState, uint8_t BS_NumNode);

#endif /*DISPLAYMENU_H_*/