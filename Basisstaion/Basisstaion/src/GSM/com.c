/*
* com.c
*
* Created: 26.06.2017 08:49:12
*  Author: Saba-Sultana Saquib
*/

#include "com.h"
#include "string.h"
#include "Http/server.h"
#include "Storage/FileSys.h"
#include <avr/interrupt.h>
unsigned char nextChar;
int init_schritt=-3;
int8_t wait=0;
int anzahl_sonderzeichen=0;
int8_t alter_schritt=-3;
extern volatile uint8_t uart_str_complete;
extern uint8_t daten_enmpfangen;   // 1 .. String komplett empfangen
volatile uint8_t uart_str_count = 0;
volatile uint8_t uart_string[UART_MAXSTRLEN + 1]="";
extern uint8_t server_initialisierung= false;

char ip_adresse[]="";

int Counter = 0x00;
uint8_t warte_ok=0;
uint8_t recBuffer[UART_MAXSTRLEN];
uint8_t waitForString=1;
void com_init(void)
{
	sysclk_enable_module(SYSCLK_PORT_F, SYSCLK_USART0); //Clock für USART0 setzen
	USARTF0.BAUDCTRLB = 0;//Einstellung Baudrate
	USARTF0.BAUDCTRLA = 12; //Einnstellung Baudrate
	USARTF0.CTRLA = USART_RXCINTLVL_HI_gc; //Interruptfreigabe auf Pin Rx
	USARTF0.CTRLB = USART_TXEN_bm | USART_RXEN_bm; //Freigabe von TX und RX für das Empfangen und senden
	USARTF0.CTRLC = USART_CHSIZE_8BIT_gc;//Größer der Zeichen
	waitForString=1;
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
	for (uint8_t i = 0; i < 32; i++)
	{
		if(data[i] == 0)return i;
	}
}


void com_send_string(uint8_t* data) //Funktion mit Übergabe der zu sendenten Daten
{
	uint8_t length = 0; //lokale Variable für die Laenge des Strings
	uint8_t counter = 0;//lokale Variable für eine Zaehler
	length = com_strlen(data); //Ermittle die Laenge der Daten
	
	//Solange counter kleiner ist als die Laenge, sende das Zeichen der übergebeben Daten an der Stelle
	//des Counter
	while(counter < length)
	{
		com_ausgabe(data[counter]);
		counter++;
	}
	//Sobald die kompletten Daten abgearbeitet sind, sende ein "\r"
	com_ausgabe(0x0D);//carriage return
}



// Damit SABA zu hause testen kann
void interrupt_init(void)
{
	PMIC.CTRL |= PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
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
	//Schreibe, das in den Rx-Pin anliegende Zeichen, in die Variable "next char"
	nextChar = USARTF0.DATA;
	//Solange das String, was Zeichenweise empfangen wird, nicht komplett ist,
	//Schreibe das jeweilige Zeichen in dem String "uart_string" an die Stelle
	//uart_str_count
	if( uart_str_complete == 0 )
	{
		if( nextChar != '\n' && nextChar != '\r' && uart_str_count < UART_MAXSTRLEN )
		{
			uart_string[uart_str_count] = nextChar;
			uart_str_count++;
			
		}
		//Falls alle Zeichen Empfangen worden sind, beende den String durch das Zeichen "\0".
		//Setze den Zaehler "uart_str_count wieder zurück und gebe durch
		//den String durch "uart_string_complete=1" für die weitere Verarbeitung frei.
		else
		{
			uart_string[uart_str_count] = '\0';
			uart_str_count = 0;
			uart_str_complete = 1;
		}
	}
}
/******************************************************************************************/
//In dieser Funktion, werden die einzelnen Befehle für die Konfiguration des GSM-Moduls
//gesendet.
/******************************************************************************************/
void server_configuration()
{

	switch(init_schritt)
	{
		//In diesem Case wird das GSM-Modul zunächst reseted
		case 0:
		{
			PORTF.OUTCLR = (1<<4);
			//	_xdelay_ms(200);//Modul reset
			PORTF.OUTSET = (1<<4);
			//_xdelay_ms(3000);//Wait till Modul has finished startup
			break;
		}
		//Senden der Befehle
		case 1: com_send_string("ATE 0"); break;
		case 2: com_send_string("AT"); break;
		case 3:com_send_string("AT+CSQ"); break;
		case 4:com_send_string("AT+CREG?");  break;
		case 5: com_send_string("AT+CGATT=1"); break;
		case 6: com_send_string("AT+CSTT=\"internet.t-d1.de\"");  break;
		case 7: com_send_string("AT+CIICR"); break;
		case 8: com_send_string("AT+CIFSR");break;
		case 9: com_send_string("AT+CIPSERVER=1,80"); break;
		case 10: com_send_string("AT+CIPSTATUS"); break;
	}
	if(init_schritt == 0)
	{
		return;
	}

	
}
/********************************************************************************************************************/
//In dieser Funktion werden die Anworten nach einem Konfigurationsschritt des GSM-Moduls ausgewertet. War
//der Schritt erfolgreich, wird der Zaehler "int_schritt" erhöht und der nächste Befehlt wird in der Funktion
//server_configuration() gesendet
/********************************************************************************************************************/
void server_configuration_auswertung(uint8_t antwort[])
{

	alter_schritt=init_schritt;
	//if(len <= 2){_xdelay_ms(5000); init_schritt--; return;}
	switch(init_schritt)
	{
		//Antwort auf dem Befehlt "AT+CREG?
		//Ist die Signalstärke ungleich 0,0, war der Schritt erfolgreich und der
		//Zaehler "init_schritt" wird um eins erhöht
		case 3:
		{

			for (uint8_t i = 0; i < UART_MAXSTRLEN; i++)
			{
				if(antwort[i] == ':')
				{

					i+=2;
					if(antwort[i] != '0')
					{
						i=UART_MAXSTRLEN;
						init_schritt++;
						return;
					}

					
				}

			}
			break;
		}
		case 4:
		//Antwort auf den Befehlt AT+CREG?
		//Ist in der Antwort die Zahl 0,1 vorhanden
		//war der Schritt erfolgreich
		{
			for (uint8_t i = 0; i < UART_MAXSTRLEN; i++)
			{
				if(antwort[i] == ':')
				{
					for(uint8_t c = i; c < UART_MAXSTRLEN; c++)
					{
						if(antwort[c] == ',')
						{
							c++;
							if(antwort[c] == '1')
							{   c=UART_MAXSTRLEN;
								i=UART_MAXSTRLEN;
								init_schritt++;
								return;
							}
						}
						
					}
				}
				//Falls der Konfigurationsschritt nicht erfolgreich war,
				//wird die Konfigurations von vorne begonnen.
				else
				{
					init_schritt=2;
				}
			}
			_xdelay_ms(2000);
			break;
		}
		//Bei den folgenden Befehlen wird als Antwort ein "OK" erwartet und daraufhin
		//überrüft: "ATE 0"; "AT"; "AT+CGATT=1"; ""AT+CSTT="internet.t-d1.de""; "AT+CIICR"; "AT+CIPSERVER=1,80"
		case 1: case 2: case 5: case 6: case 7: case 9:
		{   //Falls die Antwort "OK" ist, erhöhe den "init_schritt" um eins
			if(!strcmp("OK", antwort))
			{
				init_schritt++;
				_xdelay_ms(2000);
			}
			//Falls die Antwort "ERRRO" ist, beginne die Konfiguration vom Beginn an
			else if(!strcmp("ERROR", antwort))
			{
				init_schritt=2;
				_xdelay_ms(2000);
			}
			
			_delay_ms(1000);
			break;
		}
		//Befeht "AT+CIIFSR"
		//Bei diesem Befehl wird die IP-Adresse des GSM-Moduls empfangen
		case 8:
		{   //Falls die Antwort "ERROR" beginne die Konfiguration von Beginn an
			if(!strcmp("ERROR", antwort))
			{
				init_schritt=-2;
			}
			else
			//Hier wird die IP-Adresse in ein char-Array gespeichert
			{
				uint8_t ip_laenge= com_strlen(antwort);
				for (uint8_t i = 0; i < antwort; i++)
				{
					ip_adresse[i]=antwort[i];
					
				}
				
				init_schritt++;
			}
			break;
		}
		
		case 10:
		{	//Falls die Konfigration erfolgreich war, wird die
			//Konfiguration abgeschlossen
			if(!strcmp("STATE: SERVER LISTENING", antwort))
			{
				server_initialisierung=true;
				//_xdelay_ms(5000);
			}
			//andernfalls beginnt sie wieder von vorne
			else if(!strcmp("ERROR", antwort))
			{
				init_schritt=-2;
				//_xdelay_ms(5000);
				return;
			}
			break;
		}

	}
	//Falls der vorherige Konfigurationsschritt erfolgreich war,
	//rufe die Funktion für das Senden der Befehle auf
	if((alter_schritt!=init_schritt)|| (init_schritt==2))
	{
		server_configuration();
	}

}

uint8_t com_getNextMsg(uint8_t* str,uint8_t off,uint8_t len)
{
	for (uint8_t i = off; i < len; i++)
	{
		if(str[i] == '\n')return i;
	}
	return 0;
}

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

uint8_t com_check_string(uint8_t len, const char* antwort, uint8_t laenge_antwort)
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
