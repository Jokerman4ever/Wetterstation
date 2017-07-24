/*
* server.c
*
* Created: 13.07.2017 11:34:44
*  Author: Saba-Sultana Saquib
*/
#include "Http/server.h"
#include "Storage/FileSys.h"
#include "GSM/com.h"
#include "string.h"
//FS_StationRecord_t record;


char client_rahmen1[]="<html>"
"<head>"
"<title>Wetterstation</title>"
"</head>"
"<body>"
"<h1 style=\"color:red;\">System Design Projekt 2017: Wetterstation</h1>"
"Regenaktivität:&nbsp;<b>%r1</b><br>"
"Windgeschwindigkeit:&nbsp;<b>%w1 km/h</b><br>"
"Lichtintensität:&nbsp;<b>%l1 Lux</b><br>"
"Temperatur:&nbsp;<b>%t1 °C</b><br>"
"Feuchtigkeit:&nbsp;<b>%f1 %</b><br>"
"Druck:&nbsp;<b>%d1 mbar</b><br>"
"<br>"
"<b>Anmerkung:</b><br>einfacher Prototyp zu Testzwecken, kein endgültiges Design!"
"</body>"
"</html>";
//F
//FS-station recode;
//fs_getrecodrd/FS.currenstastus, currentunix


void com_send_antwortclient(){
	uint16_t length = 0x00;
	uint16_t Counter = 0x00;
	length = strlen(client_rahmen1);
	//com_ausgabe(length);
	char wert_anfrage;
	uint8_t messung;
		
	//FS_GetRecords(FS_CurrentStatus.CurrentUnix,&record);
	//char string=
	while(Counter < length)
	{
		//com_ausgabe(client_rahmen1[Counter]);

		if (client_rahmen1[Counter]=='%')

		{
	//	com_send_string("bin in der if\r\n");
		Counter++;
			wert_anfrage=client_rahmen1[Counter];
			//com_send_string(&wert_anfrage);
			switch (wert_anfrage)
			{
			
				case 'l': {messung=20;	
		com_ausgabe('50');com_send_string("lich");break;}//record.LightStrength; break;}
				case 't': {messung=2; com_send_string("temp"); break;}//record.Temperature; break;}
	    		case 'd': {messung=30;com_send_string("druck"); break;} //record.Pressure; break;}
				case 'r': {messung=3;com_send_string("rain");break;} //record.RainState; break;}
				case 'w': {messung=3; com_send_string("wind"); break;} //record.Pressure; break;}
				case 'f': {messung=3;com_send_string("feuchte"); break;} //record.RainState; break;}
			
			}
		
        _delay_ms(2100);
		//	com_ausgabe(0x0A);
			//com_ausgabe(0x0D);
			Counter=Counter+2;// wird nicht klappen, da messung mehrere stellen haben kann
		}

		else
		{
			com_ausgabe(client_rahmen1[Counter]);
			Counter++;
        }


	}}


