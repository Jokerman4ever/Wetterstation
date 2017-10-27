/*
* server.c
*
* Created: 13.07.2017 11:34:44
*  Author: Saba-Sultana Saquib
*/

// Einbinden alle benötigter Header Dateien
#include "Http/server.h"
#include "Storage/FileSys.h"
#include "GSM/com.h"
#include "string.h"

extern volatile uint8_t ip_adresse[20];
//Variable, um Messdaten aufzurufen
FS_StationRecord_t record;

//Das Array in dem der HTML Code gespeichert ist.
uint8_t hhtp_header1[200]="GET /nic/update?hostname=wetter-fh.ddns.net&myip=%ip HTTP/1.1\r\n"
"Connection: keep-alive\r\n"
"Authorization: Basic V2V0dGVyc3RhdGlvbjE3OndldHRlcnN0YXRpb24=\r\n"
"Upgrade-Insecure-Requests: 1\r\n";
uint8_t http_header2[256]="User-Agent: Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/60.0.3112.90 Safari/537.36\r\n";
uint8_t http_header3[256]="Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/ *;q=0.8\r\n";
uint8_t http_header4[256]="Accept-Encoding: gzip, deflate\r\n"
"Accept-Language: de-DE,de;q=0.8,en-US;q=0.6,en;q=0.4\r\n";
uint8_t html_code1[]="<html>"
"<head>"
"<title>Wetter station</title>"
"</head>"
"<body>"
"<h1 style=\"color:red;\">System Design Project 2017: Wetter station</h1>";
uint8_t htmlcode2[]="Regenaktivitaet:&nbsp;<b>%r1</b><br>"
"Windgeschwindigkeit:&nbsp;<b>%w1</b><br>"
"Lichtintensitaet:&nbsp;<b>%l1</b><br>"
"Temperatur:&nbsp;<b>%t1</b><br>"
"Feuchtigkeit:&nbsp;<b>%f1</b><br>"
"Druck:&nbsp;<b>%d1</b><br>";
uint8_t htmlcode3[]="<br>"
"<b>Anmerkung:</b><br>einfacher Prototyp zu Testzwecken, kein endgueltiges Design!"
"</body>"
"</html>";

int8_t division;
uint8_t rest=-1;
int8_t stelle=0;
_Bool vorzeichen_check=0;
int16_t pruefwert;
int16_t divisor;
/*************************************************************************************************************************/
/*Bestimmung Divisor für die Umwandlung der einzelnen Ziffern des Messwertes in ASCII-Codierung                          */
/*Beispiel: Divident/Messwert =350 -> 100< 350 <100 => divisor=100                                                       */
/*************************************************************************************************************************/
uint16_t setze_devisor(uint16_t divident)
{
	//Falls der Divident (Messwert) 100< divident < 1000 ist, setze divisor auf 100
	if(divident>100 && divident<1000)
	{
		
		divisor=100;
	}
	//Falls der Divident (Messwert) 1000< divident < 10000 ist, setze divisor auf 1000
	else if (divident>1000 && divident <10000)
	
	{  
		divisor=1000;
	}
	//Falls der Divident (Messwert) divident > 10000 ist, setze divisor auf 10000
	else if(divident >10000)
	{
		divisor=10000;
	}
	//Falls der Divident (Messwert) 10< divident < 100 ist, setze divisor auf 10
	else if(divident>10 && divident <100)
	{
		divisor=10;
	}
	//Gebe "divisor" and die Aufrufende Funktion zurück.
	return divisor;
	
}
/***********************************************************************************************/
/*Die Messwerte sollen bei Anfrage des Clients über UART zum GSM Modul gesendet werden. Für die korrekte Darstellung wird das zusendende
unsigned Messwert Ziffernweise in ASCII-Codiert, in einem String gespeichert und schließlich ueber UART an das GSM Modul geschickt.
Beispiel: Messwert=351
          1.) Ermittlung Divisor: hier divisor= 100,
		  2.) Messwert/divisor= 351/100=3
		  3.) Dezimalwert für ASCII Zeichen errechnen 3+48= 51 => ASCII Zeichen: '3' 
		  4.) ASCII Zeichen in char-array abspeichern
		  5.) Ermittlung Rest: rest=351%100=51 -> Messwert=51
		  6.) Wiederhole Schritt 1-5, bis Messwert/divisor=0 ist
		  7.) Speichere den Rest false >0  ins Array hier 1+48
		  7.) Sende String an das GSM Modul                                                    */
/***********************************************************************************************/
void com_send_messwert(uint16_t messwert)
{   
	//Array für das Speichern der Ziffern des Messwertes
	char sende_messwert[20]="";
	//Setze "stelle" beim Funktionsaufruf auf null, um das Array "send_messwert" von Beginn an zu beschreiben
	stelle=0;
	//Überprüfe, ob es sich um einen negativen Messwert handelt
	if(messwert< 0 && vorzeichen_check==false)
	{   //Sende das Zeichen '-'
		com_ausgabe(0x2d);
		//Setze Vorzeichen_check Variable auf "true"
		vorzeichen_check=true;
		//Bilde den Betrag des Messwertes, um die Umcodierung verzunehmen
		messwert= -1* messwert;
		
	}
	// Falls das Vorzeichen noch nicht überprüft worden und der Messwert positiv ist.
	else if(vorzeichen_check==false)
	{   //Sende das Vorzeichen '+'
		com_ausgabe(0x2b);
		//Setze Vorzeichen_check Variable auf "true"
		vorzeichen_check=true;
		
	}
	
	//Bestimme den Divisor
	setze_devisor(messwert);
	
	//Solange der Messwert größer Zehn ist, speichere die einzelnen Ziffern der Dezimalzahl
	//ASCII-Codiert in einem Array
	while(messwert>10)
	{
		//Ermittle die erste Ziffer 
		division= messwert/divisor;
		//Addiere 48
		division=division+48;
		//speichere division an der "stelle" in "sende_messwert"
		sende_messwert[stelle]= division;
		//Inkrementiere "stelle"
		stelle++;
		//Ermittle den Rest der Zahl und setze in als neuen Messwert
		messwert= messwert%divisor;
		//Ermittle den Prüfwert, in dem der ´Divisor durch 10 geteilt wird => nächstkleinerer Divisor
		pruefwert=divisor/10;
		//Für den Fall, dass im ursprünglichen Messwert eine "0" vorkommt
		// wird überprüft, ob der Prüfwert größer als der Messwert ist.
		//Beispiel: Messwert=301 => messwert=301%100=1
		//Prüfwert=100/10=10 => prüfwert<messwert
		if(messwert<pruefwert)
		{
			// Schreibe eine '0' ins Array
			sende_messwert[stelle]='0';
			//Inkremtiere Stelle
			stelle++;
		}
		//Falls der Prüfwert=messwert ist
		else if(messwert==pruefwert)
		{    //nochmal überprüfen
			//Schreibe eine '1' in Array
			sende_messwert[stelle]='1';
			//Inkrementiere Stelle
			stelle++;
			
			
		}
		//Berechnen den neuen Divisor
		setze_devisor(messwert);
		
	}
	//Erhöhe den Messwert, der kleiner als 10 ist um 48
	messwert=messwert+48;
	//printf("%d",messwert);
	//Falls der Messwert ungleich 0 ist,
	if(messwert!=0)
	{
		sende_messwert[stelle]=messwert;
		
	}
    //Sende den string über UART zum GSM Modul
	com_send_string(sende_messwert);
}
/***********************************************************************************************************/
//In dieser Funktion werden die Zeichen des HTML-Code nacheinander über die Uart-Schnittstelle zum GSM
//Modul geschickt. Wird ein "Sonderzeichen" der Form "%x1" im HTML-Code entdeckt, wird der jeweilige 
//Messwert zum zugehörigen Sensor des jetzigen Zeitpunktes ermittelt und gesendet.
// Erklärungen der Sonderzeichen: %l1 -> light strength
//                                %t1 -> temperatur
//                                %d1 -> pressure
//                                %r1 -> rain state
//                                %w1 -> wind direction
//                                %f1 -> humidity                               
/***********************************************************************************************************/
void com_send_antwortclient(uint8_t senden_array[]){
	//Variablen für die Bestimmung des HTML-Code, ein Zaehler für
	//das Zaehlen der abgearbeiteten Zeichen	
	uint8_t length = 0x00;
	uint8_t Counter = 0x00;
	char wert_anfrage;
	int16_t messung;
	//Ermittlung der Lange des HTML-Code
	length = strlen(senden_array);
	//Solange der HTML-Code nicht komplett abgearbeitet wurde, wird die while-Schleife ausgeführt
	while(Counter < length)
	{  
		//Falls im Html-code an der Stelle des Counter ein "%" Zeichen entdeckt wird,
		//wird "Counter" um eins erhöht und es wird geschaut, welches Zeichen danach kommt.
		//Dieses Zeichen wird in die Variable "wer_anfrage" geschrieben.
		if (senden_array[Counter]=='%')
		{
			Counter++;
			wert_anfrage=senden_array[Counter];
			//Es wird geschaut was in "wert_anfrage" steht und dann wird der zugehörige Messwert
			//ermittelt.
			switch (wert_anfrage)
			{   
				//Lichtstärke 
				case 'l': {messung=record.LightStrength;break;} 
				//Temperature
				case 't': {messung= record.Temperature; break;}
				//Druck
				case 'd': {messung= record.Pressure;    break;}
				//Regenstatus
				case 'r': {messung= record.RainState;   break;}
				//Windstärke
				case 'w': {messung =record.WindLevel;    break;}
			    //Luftfeuchtigkeit
				case 'f': {messung= record.Humidity;   break;}
				case 'i':{  com_send_string(ip_adresse);break;}
			}
			//Sende den Wert der Messung Ziffernweise mit Vorzeichen an das GSM-Modul
			if(wert_anfrage!='i'){com_send_messwert(messung);}
			//Erhöhe den Counter um zwei, dmit die Zeichen x1 nach dem % Zeichen nicht ausgegeben werden
            Counter=Counter+2;
		}

		else
		{    //Wird kein Messwert angefordert, sende das Zeichen, an der Stelle von Counter des 
			//html_codes über UART und erhöhe den Wert von Counter um 1.
			com_ausgabe(senden_array[Counter]);
		
			Counter++;
		}


	}
}


