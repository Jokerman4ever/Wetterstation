/*
* usart.c
*
* Created: 25.05.2016 08:49:12
*  Author: Stud
*/// #include "usart_driver.h"


#include "com.h"
#include "string.h"
#include <avr/interrupt.h>
int lenght = 0x00;
#define UART_MAXSTRLEN 64
unsigned char nextChar;
int init_schritt=-3;
extern volatile uint8_t uart_str_complete;
extern uint8_t daten_enmpfangen;   // 1 .. String komplett empfangen
volatile uint8_t uart_str_count = 0;
volatile char uart_string[UART_MAXSTRLEN + 1]="";
extern uint8_t server_initialisierung= false;
uint8_t kommando_senden;


int Counter = 0x00;
uint8_t warte_ok=0;
uint8_t recBuffer[UART_MAXSTRLEN];
uint8_t waitForString=1;
void com_init(void)
{
	sysclk_enable_module(SYSCLK_PORT_F, SYSCLK_USART0);
	//PORTE.DIR = 0xFF;
	//PORTE.OUT = 0xFF;
	USARTF0.BAUDCTRLB = 0;
	USARTF0.BAUDCTRLA = 12;
	//USARTF0.CTRLA = USART_RXCINTLVL_HI_gc;
	USARTF0.CTRLB = USART_TXEN_bm | USART_RXEN_bm;
	USARTF0.CTRLC = USART_CHSIZE_8BIT_gc;
	waitForString=1;
	PORTF.DIR = (1<<3)|(1<<4);//TX(3) und RST(4) auf ausgang
	//WIrd hier nicht mehr benötigt (RESet GSM
	/*PORTF.OUTCLR = (1<<4);
	_delay_ms(200);//Modul reset
	PORTF.OUTSET = (1<<4);
	_delay_ms(3000);//Wait till Modul has finished startup*/
}



void send_string(char data[])
{
	uint8_t length = 0x00;
	uint8_t Counter = 0x00;
	
	length = strlen(data);
	
	while(Counter < length)
	{
		com_ausgabe(data[Counter]);
		//	printf("%c",data[Counter]);
		Counter++;
	}
	
	Counter = 0x00;
	com_ausgabe(0x0A);
	com_ausgabe(0x0D);
	/*while (!( USARTF0.STATUS & USART_DREIF_bm));
	USARTF0.DATA = 0x0A;
	while (!( USARTF0.STATUS & USART_DREIF_bm));
	USARTF0.DATA = 0x0D;*/
}
// Damit SABA zu hause testen kann
void interrupt_init()
{
	PMIC.CTRL |= PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;													// Interrupts (Highlevel, Mediumlevel und Lowlevel freigeben)
	sei();
}

void com_ausgabe(uint8_t data)
{
	while(!(USARTF0.STATUS & USART_DREIF_bm)); // Überprüfung ob fertig mit schreiben
	USARTF0.STATUS |= USART_TXCIF_bm;
	USARTF0.DATA = data;
	while(!(USARTF0.STATUS & USART_TXCIF_bm)); // Überprüfung ob fertig mit schreiben
}

uint8_t com_getChar(uint8_t* error)
{
	uint8_t t = 100;
	uint8_t nc = 0;
	*error = 0;
	//)_delay_us(5);
	while( !(USARTF0.STATUS & USART_RXCIF_bm) && t-- > 1) _delay_us(100);
	if(t <= 1){*error = 1; return 0;}
	nc = USARTF0.DATA;
	return nc;
}


uint8_t com_getString(uint8_t* buffer)
{
	uint8_t len=0;
	uint8_t nC=0;
	uint8_t trys=5;
	uint8_t error=0;
	while( !(USARTF0_STATUS & USART_RXCIF_bm)) _delay_us(50);
	while(trys-- > 0)
	{
		do 
		{
			nC = com_getChar(&error);
			if(error)break;
			if(nC != '\r')buffer[len++]=nC;
		} 
		while (nC != '\r' && len < UART_MAXSTRLEN);
		nC=0;
		//_delay_ms(10);
	}
	return len;
}

// Wird später beim Interrupt benötigt
ISR(USARTF0_RXC_vect)
{
	com_getString(recBuffer);
	waitForString=0;
	/*nextChar = USARTF0.DATA;

	//printf("hallo");
	//segmenta_aus;
	if( uart_str_complete == 0 )
	{
		// wenn uart_string gerade in Verwendung, neues Zeichen verwerfen
		//send_string("bin da");
		// Daten werden erst in uart_string geschrieben, wenn nicht String-Ende/max Zeichenlänge erreicht ist/string gerade verarbeitet wird
		if( nextChar != '\n' && nextChar != '\r' && uart_str_count < UART_MAXSTRLEN )
		{
			uart_string[uart_str_count] = nextChar;
			//	printf("%s\n\r", nextChar);
			uart_str_count++;
			//	printf("hallo");
			//printf("%s\n\r", &uart_string[uart_str_count]);
		}
		else
		{
			uart_string[uart_str_count] = '\0';
			//send_string(uart_string);
			uart_str_count = 0;
			uart_str_complete = 1;
			//daten_enmpfangen=false;
			//printf("%s\n\r", &uart_string);
			//printf("Hello, world!\n");
			//server_configuration()
			//server_configuration(uart_string);

			if(server_initialisierung==false)
			{
				warte_ok++;
				server_configuration_auswertung();
			}

			else{ empfangen_string(); }
			//
		}
	}*/
}

// Hier sind die einzelnen Schritte für die Serverkonfiguration 
void server_configuration(void)
{
	switch(init_schritt)
	{   case -3: {
		PORTF.OUTCLR = (1<<4);
		_delay_ms(200);//Modul reset
		PORTF.OUTSET = (1<<4);
		_delay_ms(3000);//Wait till Modul has finished startup
		break;

	}
		case -2: send_string("AT"); break;
		case -1:send_string("AT+IPR=9600"); break;

		case 0:send_string("AT+CSQ");break;

		case 1:send_string("AT+CREG?");
		//			printf("AT+CREG?\n\r");
		break;

		//case 2: send_string("AT+CGACT?") ; break;
		case 2: init_schritt++; case 3: send_string("AT+CMEE=1");  break;
		case 4: send_string("AT+CGATT=1"); break;
		case 5: send_string("AT+CSTT=\"internet.t-d1.de\"");  break;
		case 6: send_string("AT+CIICR"); break;
		case 7: send_string("AT+CIFSR");break;
		case 8: send_string("AT+CIPSTART=\"TCP\",\"74.124.194.252\",\"80\""); break;
		case 9: send_string("AT+CIPCLOSE=0");break;
		case 10: send_string("AT+CFUN=1"); break;
		case 11: send_string("AT+CPIN?"); break;
		case 12: send_string("AT+CIPSERVER=1,80"); break;
		case 13: send_string("AT+CIFSR"); break;
		case 14: send_string("AT+CIPSTATUS"); break;
	}
	uint8_t reclen= com_getString(recBuffer);
	//
	//while(waitForString)_delay_ms(1);
	server_configuration_auswertung(reclen);
	
}

uint8_t com_getNextMsg(uint8_t* str,uint8_t off,uint8_t len)
{
	for (uint8_t i = off; i < len; i++)
	{
		if(str[i] == '\n')return i;
	}
	return 0;
}

uint8_t com_StrCmp(uint8_t* str1,uint8_t off1,uint8_t len1,const uint8_t* str2)
{
	uint8_t len2 = strlen(str2);
	uint8_t max = len1 < len2 ? len1 : len2;
	for (uint8_t i = 0; i < max; i++)
	{
		if(str1[i+off1] != str2[i])return 0;
	}
	return 1;
}

uint8_t COM_check_string(uint8_t len, const char* antwort, uint8_t laenge_antwort)

{
    uint8_t index=0;
	while(index < len)
	{
		index = com_getNextMsg(recBuffer,index+1,len);
		if(index == 0)break;
		if(com_StrCmp(recBuffer,index+1,laenge_antwort,&antwort))
		{
			
			return 1;
		}

		
	/*	else if(com_StrCmp(recBuffer,index+1,5,"ERROR"))
		{
			init_schritt=-2;
			_delay_ms(5000);
			server_configuration();
		}*/
	}

}

void server_configuration_auswertung(uint8_t len)
{
	char mystring[]="OK";
	
	if(init_schritt < 11)
	{
	if(COM_check_string(len,"OK", 2))
	{

	init_schritt++;
	_delay_ms(5000);
	server_configuration();
	}
	else if(COM_check_string(len,"ERROR", 5))
	{
	init_schritt=-3;
	_delay_ms(5000);
	server_configuration();
	}
	}
	return;
	
	switch(init_schritt)
	{
		case 11:

		{
				if(COM_check_string(len,"SIM is ready", 12))
				{

					init_schritt++;
					_delay_ms(5000);
					server_configuration();
				}
				else if(COM_check_string(len,"ERROR", 5))
				{
					init_schritt=-3;
					_delay_ms(5000);
					server_configuration();
			    }
		break;
		}
		

		case 12 :
		{

			
	
	if(COM_check_string(len,"SERVER OK", 9))
	{

		init_schritt++;
		_delay_ms(5000);
		server_configuration();
	}
	else if(COM_check_string(len,"ERROR", 5))
	{
		init_schritt=-3;
		_delay_ms(5000);
		server_configuration();
	}
			break;
		}

	

		case 13 :
		{
	
	if(COM_check_string(len,"OK", 2))
	{

		init_schritt++;
		_delay_ms(5000);
		server_configuration();
	}
	else if(COM_check_string(len,"ERROR", 5))
	{
		init_schritt=-3;
		_delay_ms(5000);
		server_configuration();
	}
			
		break;
		}

	


		case 14:{

		/*if(!strcmp(mystring,uart_string))
		{
		printf("%s\n\r",&uart_string); }*/

	
	if(COM_check_string(len,"STATE: SERVER LISTENING", 23))
	{

		init_schritt++;
		_delay_ms(5000);
		server_configuration();
	}
	else if(COM_check_string(len,"ERROR", 5))
	{
		init_schritt=-3;
		_delay_ms(5000);
		server_configuration();
	}
		
		break;
	}
	
	}
	//printf("%s",&mystring);

}


void empfangen_string()
{
	
	//printf("hallo");

}


