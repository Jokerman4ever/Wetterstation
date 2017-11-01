/*
* com.c
*
* Created: 26.06.2017 08:49:12
*  Author: Saba-Sultana Saquib
*/

#include "com.h"
#include "string.h"
#include "Http/server.h"
#include "DIsplay/lcd-routines.h"
#include "Storage/FileSys.h"
#include <avr/interrupt.h>
//Variable für das Empfangen eines Zeichens
unsigned char nextChar;
//Variable für die Konfigurationsschritte
int8_t init_schritt=0;
int8_t alter_schritt=2;
//Variablen für das Speichern des Empfangenen Strings
uint8_t uart_str_count = 0;
uint8_t uart_string[UART_MAXSTRLEN + 1]="";
//Variablen für das Abspeichern der Ip und der Signalstaerke
char ip_zeichen;
//IP Adresse
volatile uint8_t ip_adresse[20]="";
//Signalstaerke
uint8_t COM_RSI[5]="";
_Bool konfiguration_erfolgreich= false;
uint8_t signalstaerke_stelle=0;
//Abschnitte des http_header
extern uint8_t hhtp_header1[];
extern uint8_t http_header2[];
extern uint8_t http_header3[];
extern uint8_t http_header4[];
uint8_t offsets=0;
uint8_t laenge_antwort=0;

void com_init(void)
{
	sysclk_enable_module(SYSCLK_PORT_F, SYSCLK_USART0); //Clock für USART0 setzen
	USARTF0.BAUDCTRLB = 0;//Einstellung Baudrate
	USARTF0.BAUDCTRLA = 12; //Einnstellung Baudrate
	USARTF0.CTRLA = USART_RXCINTLVL_HI_gc; //Interruptfreigabe auf Pin Rx
	USARTF0.CTRLB = USART_TXEN_bm | USART_RXEN_bm; //Freigabe von TX und RX für das Empfangen und senden
	USARTF0.CTRLC = USART_CHSIZE_8BIT_gc;//Größer der Zeichen
	USARTF0.CTRLC=USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_DISABLED_gc | USART_CHSIZE_8BIT_gc; //Modus und Anzahl Databits
	//waitForString=1;
	PORTF.DIRSET = (1<<3)|(1<<4); //Initialisiere die Pins von Rx und Tx
	PORTF.OUTSET = (1<<4); //setze den TX Pin als Ausgang
}

uint8_t com_hasData(void)
{
	return((USARTF0_STATUS & USART_RXCIF_bm));
}

uint8_t com_strlen(uint8_t* data) //
{   //Solange im Datenstring keine "0" steht
	//erhhöhe i, ansonsten gebe den Wert von i zurück
	for (int8_t i = 0; i < 256; i++)
	{
		if(data[i] == 0)return i;
	}
}


void com_send_string(uint8_t* data) //Funktion mit Übergabe der zu sendenten Daten
{
	uint8_t length = 0; //lokale Variable für die Laenge des Strings
	uint8_t counter = 0;//lokale Variable für einen Zaehler
	length = com_strlen(data); //Ermittle die Laenge der Daten
	
	//Solange counter kleiner ist als die Laenge, sende das Zeichen der übergebeben Daten an der Stelle
	//des Counter
	while(counter < length)    
	{
		com_ausgabe(data[counter]); 
		counter++;
	}
	
}

//Sobald die kompletten Daten abgearbeitet sind, sende ein "\r"
//com_ausgabe(0x0D);//carriage return

// Damit SABA zu hause testen kann
void interrupt_init(void)
{
	PMIC.CTRL |= PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;													// Interrupts (Highlevel, Mediumlevel und Lowlevel freigeben)
	sei();
}

void com_ausgabe(uint8_t data)
{
	while(!(USARTF0.STATUS & USART_DREIF_bm)); 
	USARTF0.STATUS |= USART_TXCIF_bm;
	USARTF0.DATA = data;
	while(!(USARTF0.STATUS & USART_TXCIF_bm));
}

uint8_t com_getChar(uint8_t* error)
{
	uint8_t t = 100;
	uint8_t nc = 0;
	*error = 0;
	//)_delay_us(5);
	//while( !(USARTF0.STATUS & USART_RXCIF_bm) && t-- > 1)// _xdelay_us(5);
	if(t <= 1)
	{*error = 1; return 0;}
	nc = USARTF0.DATA;
	return nc;
}

uint8_t com_getString(uint8_t* buffer)
{
	uint8_t leni=0;
	uint8_t nC=0;
	uint8_t error=0;
	//while( !(USARTF0_STATUS & USART_RXCIF_bm)) _xdelay_us(50);
	while(!error)
	{
		nC = com_getChar(&error);
		if(error)break;
		if(nC != '\r')buffer[leni++]=nC;
	}
	return leni;
}

// ISR Routine für den Empfang
ISR(USARTF0_RXC_vect)
{   
	//Schreibe das anliegende Zeichen in die Variable "nextChar"
	nextChar = USARTF0.DATA;
	//Schreibe das Zeichen in das Array uart_string an der Stelle "uart_str_count"
	uart_string[uart_str_count] = nextChar;
	//Erhoehe den Zaehler fuer die Angabe der Stelle im Array "uart_string"
    uart_str_count++;	
}

/******************************************************************************************/
//In dieser Funktion, werden die einzelnen Befehle für die Konfiguration des GSM-Moduls
//gesendet.
/******************************************************************************************/
void server_configuration()
{

	switch(init_schritt)
	{   
		
		case 0: com_send_string("ATE 1\r"); break; //Auschalten des Echos ATE 1-> einschalten
		case 1: com_send_string("AT+CFUN=1,1\r"); break; //Resets the Modul
		case 2: com_send_string("AT\r"); break;
		case 3: com_send_string("AT+IPR=9600\r"); break;
		
		case 4:com_send_string("AT+CREG?\r");  break;
		case 5: com_send_string("AT+CGATT=1\r"); break; 
		case 6:com_send_string("AT+CSTT=\"internet.t-d1.de\"\r");  break;
		case 7:com_send_string("AT+CIICR\r"); break;
		case 8:com_send_string("AT+CIFSR\r");break;
	//	case 10:
		//{
			//com_send_string("AT+CIPSTART=\"TCP\",\"8.23.224.120\",\"80\"\r");//_delay_ms(60000);
			//break;
		//}
		
		case 9: 
		{
			/*com_send_string("AT+CIPSEND\r"); 
			com_send_antwortclient(hhtp_header1);
			com_send_antwortclient(http_header2);
			com_send_antwortclient(http_header3);
			com_send_antwortclient(http_header4);
			com_ausgabe(0x1A);
			_delay_ms(30000);*/
			com_send_string("AT+CIPSERVER=1,80\r");
			_delay_ms(10000);
			break;
			}
			case 10:{com_send_string("AT+CSQ\r"); break;}

	//	case 12:  com_send_string("AT+CIPSTATUS\r"); break;
	}

	_delay_ms(10000);
	uart_string[uart_str_count]='\0';
	if(konfiguration_erfolgreich==false)
	{
		server_configuration_auswertung(uart_string);
	}


	
}

/********************************************************************************************************************/
//In dieser Funktion werden die Anworten nach einem Konfigurationsschritt des GSM-Moduls ausgewertet. War
//der Schritt erfolgreich, wird der Zaehler "int_schritt" erhöht und der nächste Befehlt wird in der Funktion
//server_configuration() gesendet
/********************************************************************************************************************/
void server_configuration_auswertung(uint8_t antwort[])
{
	com_send_string(antwort);
		
		//lcd_set_cursor(0,0);

		//lcd_Xstring(antwort,0);
	alter_schritt=init_schritt;
			
switch(init_schritt)
	{ 
		//Antwort auf dem Befehlt "AT+CREG?
		//Ist die Signalstärke ungleich 0,0, war der Schritt erfolgreich und der 
		//Zaehler "init_schritt" wird um eins erhöht
		case 10:
		{

		
	signalstaerke_zwischenspeichern(antwort);
			
			break;
		}
		case 4:
		//Antwort auf den Befehlt AT+CREG?
		//Ist in der Antwort die Zahl 0,1 vorhanden
		//war der Schritt erfolgreich
		{   
			for (int8_t i = 0; i < UART_MAXSTRLEN; i++)
			{
				if(antwort[i] == '0')
				{
					for(int8_t c = i; c < UART_MAXSTRLEN; c++)
					{
						if(antwort[c] == ',')
						{
							c++;
							if(antwort[c] == '1')
							{   c=UART_MAXSTRLEN;
							    i=UART_MAXSTRLEN;
								init_schritt++;
								
								
							}
						}
						
					}
				}
				//Falls der Konfigurationsschritt nicht erfolgreich war,
				//wird die Konfigurations von vorne begonnen.
				
			}
			_xdelay_ms(2000);
	
			break;
		}
		//Bei den folgenden Befehlen wird als Antwort ein "OK" erwartet und daraufhin
		//überrüft: "ATE 0"; "AT"; "AT+CGATT=1"; "AT+CSTT="internet.t-d1.de""; "AT+CIICR"; "AT+CIPSERVER=1,80"
		case 0: case 1: case 2: case 3: case 5: case 6: case 7: case 9: 
		{ 
			//Suche im Empfangenen String nach einem "O"
			laenge_antwort=com_strlen(antwort); 
			for (uint8_t i = 0; i < laenge_antwort; i++)
			{
				if(antwort[i] == 'O')
				{
					offsets=i;
					i=laenge_antwort;
				}
			}
			//Schau, ob beginnend an der Stelle des O's ein "OK" im Array steht.
			if(com_StrCmp(antwort,offsets,2,"OK")==true)
			{   //Falls es sich um den letzten Konfigurationsschritt handelt,
				//setze die Variable "konfiguration_erfolgreich" auf true
				if(init_schritt==10)
				{
					konfiguration_erfolgreich=true;
					
				}
				//Andernfalls Erhoehe init_schritt um eins, um den naechsten Konfigurationsschritt durchzufuehren
				init_schritt++;
			}
			
			break;
			}
			
			lcd_set_cursor(0,0);
			lcd_Xstring("Konfiguration fertig",0);
		//Befeht "AT+CIIFSR"
		//Bei diesem Befehl wird die IP-Adresse des GSM-Moduls empfangen
		case 8:
		{   //Falls die Antwort "ERROR" beginne die Konfiguration von Beginn an
		
			//Hier wird die IP-Adresse in ein char-Array gespeichert
	
		
		//Falls die Antwort "OK" ist, erhöhe den "init_schritt" um eins
		/*for (int8_t i = 0; i < UART_MAXSTRLEN; i++)
		{
			if(antwort[i] == 'E')
			{
				
				offsets=i;
				i=UART_MAXSTRLEN;
				
			}
		}
			if(com_StrCmp(antwort,offsets,5,"ERROR"))
			{
				init_schritt=0;
			}
			*/
			
			//printf("%s ich bin die antwort  ",uart_string);
		ip_adresse_zwischenspeichern(antwort);
		
			    init_schritt++;
			
			
			break;
		}
    /*    case 10:
		 {
			 printf("ich bin im case 10");
			 printf("%s",antwort);
			uint8_t offsets;
			//Falls die Antwort "OK" ist, erhöhe den "init_schritt" um eins
			for (uint8_t i = 0; i < UART_MAXSTRLEN; i++)
			{
				if(antwort[i] == 'C' ||antwort[i]=='A')
				{
					
					
					offsets=i;
					i=UART_MAXSTRLEN;
					
				}
			}
			if(com_StrCmp(antwort,offsets,10,"CONNECT OK")==true ||com_StrCmp(antwort,offsets,15,"ALREADY CONNECT")==true)
			{
				printf("ich bin bei CONNECT OK");
				init_schritt++;
				
			}
			

			else
			{
				init_schritt=0;
			}
			
			
			break;
		} 
		 */
		case 12:
		{	//Falls die Konfigration erfolgreich war, wird die
			//Konfiguration abgeschlossen
			
			printf("ich bin im letzten CASE");
			//Falls die Antwort "OK" ist, erhöhe den "init_schritt" um eins
			for (uint8_t i = 0; i < UART_MAXSTRLEN; i++)
			{
				if(antwort[i] == 'S' && antwort[i+1]=='T')
				{
					offsets=i;
					i=UART_MAXSTRLEN;
					i++;
				}
			}
			//STATE: SERVER LISTENING
			if(com_StrCmp(antwort, offsets,23,"STATE: SERVER LISTENING"==true ))
			{
				konfiguration_erfolgreich==true;
				printf("Konfiguration abgeschlossen");
				return;
			}
			
			else {init_schritt=2;}
				break;
		}

}	//Falls der vorherige Konfigurationsschritt erfolgreich war,
	//rufe die Funktion für das Senden der Befehle auf
    //Falls die Auswertung des Konfigurationsschritt erfolgreich war, wird der Zaehler inkrementiert
	//Hat diese stattgefunden, wird die Funktion, um den nächsten AT-Befehl zu senden aufgerufen.
	//Und durch zurücksetzen von "uart_str_count", kann das Array für das Speichern der Antwort wieder
	//von Beginn verwendet werden. Die Beschreibung des Arrays erfolgt in der ISR-Routine
	if((alter_schritt!=init_schritt)|| (init_schritt==2))
	{   uart_str_count=0;
		server_configuration();
	}
    else 
	{
	//Falls der Konfigurationsschritt nicht erfolgreich war, beginnt die Konfiguration wieder von 
	//Beginn an.
		uart_str_count=0;
		init_schritt=0;
		server_configuration();
	}
}
	

/*uint8_t com_getNextMsg(uint8_t* str,uint8_t off,uint8_t len)
{
	for (uint8_t i = off; i < len; i++)
	{
		if(str[i] == '\n')return i;
	}
	return 0;
}*/
/******************************************************************************************/
//In dieser Funktion werden  Strings miteinander verglichen

uint8_t com_StrCmp(uint8_t* str1,uint8_t off1,uint8_t len1,const char* str2)
{
	uint8_t len2 = strlen(str2);
	uint8_t max = len1 < len2 ? len1 : len2;
	for (uint8_t i = 0; i < max; i++)
	{
		if(str1[i+off1] != str2[i])return 0;
	}
	return 1;
}
uint8_t* str1="+CSQ: 12,0 OK";
com_StrCmp(str1,10,2,"OK");

/*uint8_t com_check_string(uint8_t len, const char* antwort, uint8_t laenge_antwort)
{
    uint8_t index=0;
	while(index < len)
	{
		index = com_getNextMsg(recBuffer,index+1,len);
		if(index == 0)break;
		if(com_StrCmp(recBuffer,index+1,laenge_antwort,antwort))
		{
			return 1;
		}
	}
	return 0;

}
*/

void signalstaerke_zwischenspeichern(uint8_t antwort[]){
	
		for (int8_t i = 0; i < UART_MAXSTRLEN; i++)
		{
			if(antwort[i] == ':')
			{

				i+=2;
				if(antwort[i] != '0')
				
				{
				
					for(int ziffer=i;ziffer<(i+4); ziffer++)
					{
						COM_RSI[signalstaerke_stelle]=antwort[ziffer];
						
						signalstaerke_stelle++;
					}
					i=UART_MAXSTRLEN;
					init_schritt++;
					
				}

				
			}
			
			

		}
		
		com_send_string(COM_RSI);
		
}
void ip_adresse_zwischenspeichern(uint8_t antwort_ip[]){
	char ip;
	uint8_t ip_laenge;
	
	uint8_t ip_counter=0;
	uint8_t stell_ip=0;
	ip_laenge=com_strlen(antwort_ip);
	for(uint8_t i=0; i<ip_laenge; i++)
	{
		
	
	if(antwort_ip[i]=='3'){
		
		ip_counter=i;
		i=ip_laenge;
	}
	}
	
	for(uint8_t s=ip_counter; s<ip_laenge; s++ )
	
	{
		if(antwort_ip[s]!='\r'){		
		ip_adresse[stell_ip]=antwort_ip[s];
		stell_ip++;
		}
		
		else
		
		{
			antwort_ip[s]='\0';
			s=ip_laenge;
			
			
		}
		
		
				
	}
	
	lcd_set_cursor(0,0);

	lcd_Xstring("hallo",0);
	lcd_Xstring(ip_adresse,0);
}