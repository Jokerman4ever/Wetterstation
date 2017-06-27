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
#include "string.h"
enum PageID {
				PageWelcome = 1,
				PageMenuHome = 10,
				PageHome = 11,
				PageMenuFehlerliste = 20,
				PageFehlerliste = 21,
				PageMenuEinstellungen = 30,
				PageEinstellungen = 31,
				PageMenuWettermonitor = 40,
				PageWettermonitor = 41,
			};
	
void PGM_ReadStr(const uint8_t* str,char* dest,uint8_t start);
void CenterStringPGM(const uint8_t* str, uint8_t* Temp,uint8_t arrows);
void CenterString(char* str,char* Temp,uint8_t arrows);
void SelectString(char* str,uint8_t* Temp);
void SelectStringPGM(const uint8_t* str,uint8_t* Temp);
void DSP_ChangePage(uint8_t ID);
void DSP_ScrollMenu(uint8_t dir);
void DSP_SelectMenu(void);

#endif /*DISPLAYMENU_H_*/