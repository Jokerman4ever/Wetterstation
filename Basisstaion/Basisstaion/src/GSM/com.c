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
int init_schritt=1;
int8_t alter_schritt=2;
char ip_zeichen;
volatile uint8_t uart_str_count = 0;
volatile uint8_t uart_string[UART_MAXSTRLEN + 1]="";
int8_t ip_laenge;
_Bool konfiguration_erfolgreich= false;
volatile uint8_t ip_adresse[20]="";
int8_t ip_counter=0;
int Counter = 0x00;
extern char hhtp_header[];
void com_init(void)
{
	sysclk_enable_module(SYSCLK_PORT_F, SYSCLK_USART0); //Clock für USART0 setzen
	USARTF0.BAUDCTRLB = 0;//Einstellung Baudrate
	USARTF0.BAUDCTRLA = 12; //Einnstellung Baudrate
	USARTF0.CTRLA = USART_RXCINTLVL_HI_gc; //Interruptfreigabe auf Pin Rx
	USARTF0.CTRLB = USART_TXEN_bm | USART_RXEN_bm; //Freigabe von TX und RX für das Empfangen und senden
	USARTF0.CTRLC = USART_CHSIZE_8BIT_gc;//Größer der Zeichen
	//USARTF0.CTRLC=USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_DISABLED_gc | USART_CHSIZE_8BIT_gc;
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
	for (int8_t i = 0; i < 32; i++)
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
{  //printf("hallo");
	//com_empfangen();
	//Schreibe, das in den Rx-Pin anliegende Zeichen, in die Variable "next char" 
	nextChar = USARTF0.DATA;
	//Solange das String, was Zeichenweise empfangen wird, nicht komplett ist,
	//Schreibe des jeweilige Zeichen in dem String "uart_string" an die Stelle
	//uart_str_count

		//
		
	    //printf("%c",nextChar);
		//
	//if( uart_str_complete == 0 )
	//{
	//if( nextChar != '\n' && nextChar != '\r' && uart_str_count < UART_MAXSTRLEN )
		{
		uart_string[uart_str_count] = nextChar;
        uart_str_count++;
			//
		//printf("%c",nextChar);	
			
		//}
		//Falls alle Zeichen Empfangen worden sind, beende den String durch das Zeichen "\0".
		//Setze den Zaehler "uart_str_count wieder zurück und gebe durch
		//den String durch "uart_string_complete=1" für die weitere Verarbeitung frei.
	//else
		//{   
			//uart_string[uart_str_count] = '\0';
			//printf("%s\r\n",uart_string);
			
			//uart_str_count = 0;
			//uart_str_complete = 1;
		//}
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
	
		//Senden der Befehle
		//Echo ausschalten mit ATE 0
		
		case 0: com_send_string("AT+CFUN=1,1"); break; //Resets the Modul
		case 1: com_send_string("ATE 0"); break; //Auschalten des Echos ATE 1-> einschalten
		case 2: com_send_string("AT"); break;
		case 3: com_send_string("AT+IPR=9600"); break;
		case 4:com_send_string("AT+CSQ"); break;
		case 5:com_send_string("AT+CREG?");  break;
		case 6: com_send_string("AT+CGATT=1"); break; 
		case 7:com_send_string("AT+CSTT=\"internet.t-d1.de\"");  break;
		case 8:com_send_string("AT+CIICR"); break;
		case 9:com_send_string("AT+CIFSR");break;
		case 10:com_send_string("AT+CIPSTART=\"TCP\",\"8.23.224.120\",\"80\"");
		case 11: com_send_string("AT+CIPSEND");
		case 12: com_send_antwortclient(hhtp_header);com_ausgabe(0x1A); _delay_ms(50000);
		case 13:com_send_string("AT+CIPSERVER=1,80"); break;
		case 14:  com_send_string("AT+CIPSTATUS"); break;
	}
	/*if(init_schritt == 0)
	{
		return;
	}*/
	_delay_ms(8500);
	uart_string[uart_str_count]='\0';
	if(konfiguration_erfolgreich==false)
	{
		server_configuration_auswertung(uart_string);
	}
	//com_empfangen();
	//init_schritt++;

	
}

/*void com_empfangen(){
	




if (init_schritt<11)
{
printf("%s hallo",uart_string);
	init_schritt++;
	uart_str_count=0;
	for(int i=0; i<com_strlen(uart_string);i++)
	{
	uart_string[i]=' ';}
	server_configuration();}

	
}*/
/********************************************************************************************************************/
//In dieser Funktion werden die Anworten nach einem Konfigurationsschritt des GSM-Moduls ausgewertet. War
//der Schritt erfolgreich, wird der Zaehler "int_schritt" erhöht und der nächste Befehlt wird in der Funktion
//server_configuration() gesendet
/********************************************************************************************************************/
void server_configuration_auswertung(uint8_t antwort[])
{
	//printf("%s",uart_string);
	alter_schritt=init_schritt;
	//if(len <= 2){_xdelay_ms(5000); init_schritt--; return;}		
	switch(init_schritt)
	{ 
		//Antwort auf dem Befehlt "AT+CREG?
		//Ist die Signalstärke ungleich 0,0, war der Schritt erfolgreich und der 
		//Zaehler "init_schritt" wird um eins erhöht
		case 5:
		{

			for (int8_t i = 0; i < UART_MAXSTRLEN; i++)
			{ 
				if(antwort[i] == ':')
				{ 

					i+=2;
					if(antwort[i] != '0')
					{   
						i=UART_MAXSTRLEN;
						init_schritt++;
					   
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
		case 0: case 1: case 2: case 3: case 6: case 7: case 8: case 13: 
		{  //
		int offsets;
			//Falls die Antwort "OK" ist, erhöhe den "init_schritt" um eins 
		for (int8_t i = 0; i < UART_MAXSTRLEN; i++)
		{
			if(antwort[i] == 'O')
			{
				
				
				offsets=i;
				i=UART_MAXSTRLEN;
							
			}
		}
			if(com_StrCmp(antwort,offsets,2,"OK")==true)
			{
				
				init_schritt++;
			
			}
		
			/*else
			{
				init_schritt=0;
			}*/
						
			
			break;
		}
		//Befeht "AT+CIIFSR"
		//Bei diesem Befehl wird die IP-Adresse des GSM-Moduls empfangen
		case 9:
		{   //Falls die Antwort "ERROR" beginne die Konfiguration von Beginn an
			
			//Hier wird die IP-Adresse in ein char-Array gespeichert
		int offsets;
		char ip;
		//Falls die Antwort "OK" ist, erhöhe den "init_schritt" um eins
		for (int8_t i = 0; i < UART_MAXSTRLEN; i++)
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
			
			
			//printf("%s ich bin die antwort  ",uart_string);
				 ip_laenge= com_strlen(antwort);
				//printf(" Laenge:%d Laenge", ip_laenge);
				_delay_ms(8000);
				
					for (int8_t i = 0; i < ip_laenge; i++)
					{
						if(antwort[i] == '3')
						{
							
							//printf("%c",antwort[i]);
							ip_counter=i;
							//ip_adresse[ip_counter]=antwort[i];
							i=UART_MAXSTRLEN;
							
						}
					}
			for (ip_counter; ip_counter < ip_laenge; ip_counter++)
				
				{  
				//	printf(" ich bin ip_counter %d",ip_counter);
					//printf(" ich bin ip_laenge %d",ip_laenge);
					ip_zeichen=uart_string[ip_counter];
					if(ip_zeichen!= '\r')
					{
					uart_string[ip_counter]=ip_zeichen;
				//	printf("%d counter",ip_counter);
					//printf("%c",ip_adresse[ip_counter]);
					
					}
					
					//else
					//{
						//ip_counter=ip_laenge;
					//}
				}
				printf("%s",uart_string);
			    init_schritt++;
			
			
			break;
		}
        case 10:
		 {
			int offsets;
			//Falls die Antwort "OK" ist, erhöhe den "init_schritt" um eins
			for (int8_t i = 0; i < UART_MAXSTRLEN; i++)
			{
				if(antwort[i] == 'C' ||antwort[i]=='Á')
				{
					
					
					offsets=i;
					i=UART_MAXSTRLEN;
					
				}
			}
			if(com_StrCmp(antwort,offsets,10,"CONNECT OK")==true ||com_StrCmp(antwort,offsets,15,"ALREADY CONNECT")==true)
			{
				
				init_schritt++;
				
			}
			

			else
			{
				init_schritt=0;
			}
			
			
			break;
		} 
		 
		case 14:
		{	//Falls die Konfigration erfolgreich war, wird die
			//Konfiguration abgeschlossen
			int offsets;
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
	//
	//printf("%d",init_schritt);
	//printf("%d",alter_schritt);
	if((alter_schritt!=init_schritt)|| (init_schritt==2))
	{   
		//printf("%s",uart_string);
		
		uart_str_count=0;
		for(int i=0; i<com_strlen(uart_string);i++)
		{
		uart_string[i]=' ';}
	    server_configuration();
	}
    else 
	{
		uart_str_count=0;
		init_schritt=2;
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
