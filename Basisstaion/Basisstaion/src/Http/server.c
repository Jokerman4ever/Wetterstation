/*
* server.c
*
* Created: 13.07.2017 11:34:44
*  Author: Saba-Sultana Saquib
*/
#include "Http/server.h"
#include "Storage/FileSys.h"
#include "GSM/com.h"
FS_StationRecord_t record;


char client_rahmen1[]="<html>"
"<head>"
"<title>Wetterstation</title>"
"</head>"
"<body>"
"<h1 style=\"color:red;\">System Design Projekt 2017: Wetterstation</h1>"
"Regenaktivität:&nbsp;<b>%s</b><br>"
"Windgeschwindigkeit:&nbsp;<b>%s km/h</b><br>"
"Lichtintensität:&nbsp;<b>%s Lux</b><br>"
"Temperatur:&nbsp;<b>%s °C</b><br>"
"Feuchtigkeit:&nbsp;<b>%s %</b><br>"
"Druck:&nbsp;<b>%s bar</b><br>"
"<br>"
"<b>Anmerkung:</b><br>einfacher Prototyp zu Testzwecken, kein endgültiges Design!"
"</body>"
"</html>";
//F
//FS-station recode;
//fs_getrecodrd/FS.currenstastus, currentunix


uint8_t client_anfrage_auswertung(){

FS_GetRecords(FS_CurrentStatus.CurrentUnix,&record);
	//Rahmen
/*	com_send_string("<html>"
	"<head>"
	"<title>Wetterstation</title>"
	"</head>"
	"<body>"
	"<h1 style=\"color:red;\">System Design Projekt 2017: Wetterstation</h1>");
	com_send_antwortclient("Regenaktivität:&nbsp;<b>%d</b><br>",&record.RainState);
	com_send_antwortclient("Windgeschwindigkeit:&nbsp;<b>%d km/h</b><br>",&record.WindLevel);
	com_send_antwortclient("Lichtintensität:&nbsp;<b>%d Lux</b><br>",&record.LightStrength);
	com_send_antwortclient("Temperatur:&nbsp;<b>%d °C</b><br>",&record.Temperature);
	com_send_antwortclient("Feuchtigkeit:&nbsp;<b>%s %</b><br>",&record.Humidity);
	com_send_antwortclient("Druck:&nbsp;<b>%s bar</b><br>",&record.Pressure);
    com_send_string("<br>"
    "<b>Anmerkung:</b><br>einfacher Prototyp zu Testzwecken, kein endgültiges Design!"
    "</body>"
    "</html>");*/
}


