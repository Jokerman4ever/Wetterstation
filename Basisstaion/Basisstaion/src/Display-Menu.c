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
/*
uint8_t DSP_MenuSelection;
uint8_t DSP_CurrentPage;
uint8_t DSP_PlayID;
uint8_t* DSP_PlayName;
uint8_t DSP_PlayMenuSelection;
uint8_t DSP_PlaySpeed = 7;
*/

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
			lcd_Xstring()
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
		case PagePlay_Titles: case PageRemove_Titles:
		{
			if(dir && DSP_MenuSelection < FS_FileCount)DSP_MenuSelection++;
			else if(!dir && DSP_MenuSelection > 0)DSP_MenuSelection--;
			DSP_ChangePage(DSP_CurrentPage);//Update Display
			break;
		}
		case PagePlay:
		{
			if(dir)DSP_ChangePage(PageRecord);
			else DSP_ChangePage(PageRemove);
			break;
		}
		case PageRemove:
		{
			if(dir)DSP_ChangePage(PagePlay);
			break;
		}
		case PageRecord:
		{
			if(!dir)DSP_ChangePage(PagePlay);
			break;
		}
		case PagePlay_Active:
		{
			if(dir && DSP_MenuSelection < 4)DSP_MenuSelection++;
			else if(!dir && DSP_MenuSelection > 0)DSP_MenuSelection--;
			DSP_ChangePage(DSP_CurrentPage);//Update Display
			break;
		}
		case PagePlay_Active_Speed:
		{
			if(dir && DSP_PlaySpeed < 14){DSP_PlaySpeed++;TCD1.PER -=150; }
			else if(!dir && DSP_PlaySpeed > 0){DSP_PlaySpeed--;TCD1.PER +=150;}
			DSP_ChangePage(DSP_CurrentPage);//Update Display
			break;
		}
		case PagePlay_Active_Volume:
		{
			if(!dir && DP_Value < 0x3F){DP_MoveUp(4); }
			else if(dir && DP_Value > 0x00){DP_MoveDown(4);}
			DSP_ChangePage(DSP_CurrentPage);//Update Display
			break;
		}
		case PageRemove_Titles_Sure:
		{
			if(dir && DSP_MenuSelection < 1)DSP_MenuSelection++;
			else if(!dir && DSP_MenuSelection > 0)DSP_MenuSelection--;
			DSP_ChangePage(DSP_CurrentPage);//Update Display
			break;
		}
		default: break;
	}
}

void DSP_SelectMenu(void)
{
	switch(DSP_CurrentPage)
	{
		case PagePlay: DSP_MenuSelection=(FS_FileCount>0 ? 1 : 0); DSP_ChangePage(PagePlay_Titles); break;
		case PageRecord: DSP_ChangePage(PageRecord_Active); StartRecord(); break;
		case PagePlay_Titles:
		{
			if(DSP_MenuSelection == 0)DSP_ChangePage(PagePlay);
			else
			{
				DSP_MenuSelection = 1;
				//DSP_PlaySpeed = 7;
				DSP_ChangePage(PagePlay_Active);
				StartPlayback(DSP_PlayID);
			}
			break;
		}
		case PagePlay_Active:
		{
			switch(DSP_MenuSelection)
			{
				case 0: DSP_ChangePage(PagePlay_Active_Volume); break;//Volume Menu
				case 1: DSP_MenuSelection = DSP_PlayMenuSelection; StopPlayback(); DSP_ChangePage(PagePlay_Titles); break;//Stop Selected
				case 2: DSP_ChangePage(PagePlay_Active_Speed); break;//Speed Menu
				case 3: AudioTempFile.Reverse = (AudioTempFile.Reverse ? 0 : 1); DSP_ChangePage(DSP_CurrentPage); break; //Reverse
				case 4: AudioTempFile.FilePlaying = (AudioTempFile.FilePlaying == 1 ? 2 : 1); FS_SetReadPos(0); DSP_ChangePage(DSP_CurrentPage); break; //Loop
				default:break;
			}
			
			break;
		}
		case PageRemove_Titles:
		{
			if(DSP_MenuSelection == 0) DSP_ChangePage(PageRemove);
			else
			{
				DSP_MenuSelection = 0;
				DSP_ChangePage(PageRemove_Titles_Sure);
			}
			break;
		}
		case PagePlay_Active_Volume:DSP_ChangePage(PagePlay_Active);break;
		case PagePlay_Active_Speed: DSP_ChangePage(PagePlay_Active);break;
		case PageRemove: DSP_MenuSelection=(FS_FileCount>0 ? 1 : 0); DSP_ChangePage(PageRemove_Titles);break;
		case PageRemove_Titles_Sure:
		{
			if(!DSP_MenuSelection)DSP_ChangePage(PageRemove);
			else
			{
				FS_RemoveEntry(DSP_PlayID);//Remove File!
				lcd_clear();
				lcd_set_cursor(0,1);
				SelectString("File Removed",LineTemp);
				lcd_Xstring(LineTemp,0);
				_delay_ms(1500);
				DSP_ChangePage(PageRemove);
			}
			break;
		}
		case PageRecord_Active:
		{
			StopRecord();
			DSP_ChangePage(PageRecord_Finished);
			_delay_ms(1500);
			DSP_ChangePage(PagePlay);
			break;
		}
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