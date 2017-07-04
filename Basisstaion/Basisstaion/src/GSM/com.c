/*
* usart.c
*
* Created: 25.05.2016 08:49:12
*  Author: Stud
*/// #include "usart_driver.h"


#include "com.h"
//#include "display.h"
#include <avr/io.h>
#include <stdio.h>
#include <stdbool.h>
//#include "coms.h"
#include "string.h"
#include <avr/interrupt.h>
int lenght = 0x00;
#define UART_MAXSTRLEN 20
unsigned char nextChar;
// USART
int init_schritt=-2;
extern volatile uint8_t uart_str_complete;
extern uint8_t daten_enmpfangen;   // 1 .. String komplett empfangen
volatile uint8_t uart_str_count = 0;
volatile char uart_string[UART_MAXSTRLEN + 1]="";


extern uint8_t server_initialisierung= false;
uint8_t kommando_senden;


int Counter = 0x00;
uint8_t warte_ok=0;


void com_init(void)
{
	//PORTE.DIR = 0xFF;
	//PORTE.OUT = 0xFF;
	USARTF0.BAUDCTRLB = 0;
	USARTF0.BAUDCTRLA = 12;
	USARTF0.CTRLA = USART_RXCINTLVL_HI_gc;
	USARTF0.CTRLB = USART_TXEN_bm | USART_RXEN_bm;
	USARTF0.CTRLC = USART_CHSIZE_8BIT_gc;
	PORTF.DIR = 0x08;
}

void com_baud()
{
	
	//USART_Baudrate_Set(&USART, 12, 0);
	
	USARTF0.BAUDCTRLB=0;
	USARTF0.BAUDCTRLA=12;
}


void char_ausgabe(char s)
{
	while (!( USARTF0.STATUS & USART_DREIF_bm));
	USARTF0.DATA = s;
}

void send_string(char data[])
{
	uint8_t length = 0x00;
	uint8_t Counter = 0x00;
	
	length = strlen(data);
	
	while(Counter < length)
	{
		while (!(USARTF0.STATUS & USART_DREIF_bm));
		USARTF0.DATA = data[Counter];
		//	printf("%c",data[Counter]);
		Counter++;
	}
	
	Counter = 0x00;
	while (!( USARTF0.STATUS & USART_DREIF_bm));
	USARTF0.DATA = 0x0A;
	while (!( USARTF0.STATUS & USART_DREIF_bm));
	USARTF0.DATA = 0x0D;
}

void interrupt_init()
{
	PMIC.CTRL |= PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;													// Interrupts (Highlevel, Mediumlevel und Lowlevel freigeben)
	sei();
}


ISR(USARTF0_RXC_vect)
{
	nextChar = USARTF0.DATA;

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
	}
}

void server_configuration(void)
{



		switch(init_schritt)

		{
			case -2: send_string("AT"); break;
			case -1:send_string("AT+IPR=9600"); break;

			case 0:

			send_string("AT+CSQ");
			//printf("AT+CSQ\n\r");
			break;

			case 1:
			send_string("AT+CREG?");
			//			printf("AT+CREG?\n\r");
			break;

			case 2: send_string("AT+CGACT?") ; break;
			case 3: send_string("AT+CMEE=1");  break;
			case 4: send_string("AT+CGATT=1"); break;
			case 5: send_string("AT+CSTT=\"internet.t-d1.de\"");  break;
			case 6: send_string("AT+CIICR"); break;
			case 7: send_string("AT+CIFSR");break;
			case 8: send_string("AT+CIPSTART=\"TCP\",\"74.124.194.252\",\"80\"\n\r"); break;
			case 9: send_string("AT+CIPCLOSE=0");break;
			case 10: send_string("AT+CFUN=1"); break;
			case 11: send_string("AT+CPIN?"); break;
			case 12: send_string("AT+CIPSERVER=1,80"); break;
			case 13: send_string("AT+CIFSR"); break;
			case 14: send_string("AT+CIPSTATUS"); break;




		}
}

void server_configuration_auswertung(void)
{
			char mystring[]="OK";

			switch(init_schritt){
				case -2:
				if(!strcmp(mystring, uart_string))
				{ 	printf("%s\n\r",&uart_string);
					init_schritt++;
					printf("%d\n\r",init_schritt);
					warte_ok=0;
					server_configuration();
					
				}

				

				else if(warte_ok==5)
				 {
					//warte_ok++;
					warte_ok=0;
					//printf("hallo");
					server_configuration();
				}
				case -1:
				{
					if(!strcmp(mystring, uart_string))
					{ 	
						printf("%s\n\r",&uart_string);
						init_schritt++;
						printf("%d\n\r",init_schritt);
						warte_ok=0;
						server_configuration();
					}
					else if(warte_ok==5) 
					{
						//warte_ok++;
						warte_ok=0;
						//printf("hallo");
						server_configuration();
					}
					break;
				}
				case 0:
				{ 
					//BEFEHL AT+CSQ
					if(!strcmp(mystring, uart_string))
					{ 	printf("%s\n\r",&uart_string);
						init_schritt++;
						printf("%d\n\r",init_schritt);
						warte_ok=0;
						server_configuration();
					}
					else if(warte_ok==5) 
					{
						//warte_ok++;
						warte_ok=0;
						//printf("hallo");
						server_configuration();
					}
					break;
				}
				//BEFehl AT+CREG?
				case 1:
				{
					/*if(!strcmp("+CREG: 0,1",uart_string))
					{
					printf("%s\n\r",&uart_string); }*/
					if (!strcmp(mystring, uart_string))
					{ 	printf("%s\n\r",&uart_string);
						init_schritt++;
						printf("%d\n\r",init_schritt);
						warte_ok=0;
						server_configuration();
					}
					else if(warte_ok==5) 
					{
						//warte_ok++;
						warte_ok=0;
						server_configuration();
					}
					break;
				}
				case 2: //Befehl AT+CGACT?
				
				/*if(!strcmp("+CGACT: 1,0",uart_string))
				{
				printf("%s\n\r",&uart_string); }

				else if(!strcmp("+CGACT: 2,0",uart_string))
				{
				printf("%s\n\r",&uart_string); }

				else if(!strcmp("+CGACT: 3,0",uart_string))
				{
				printf("%s\n\r",&uart_string); }*/
				if (!strcmp(mystring, uart_string))
				{ 	printf("%s\n\r",&uart_string);
					init_schritt++;
					printf("%d\n\r",init_schritt);
					warte_ok=0;
					server_configuration();
				}

				else if(warte_ok==5) {
					//warte_ok++;
					warte_ok=0;
				server_configuration();}
				break;

				case 3: //BEFEHL AT+CMEE=!

				if (!strcmp(mystring, uart_string))
				{ 	printf("%s\n\r",&uart_string);
					init_schritt++;
					printf("%d\n\r",init_schritt);
					warte_ok=0;
					server_configuration();
				}
				else if(warte_ok==5) {
					//warte_ok++;
					warte_ok=0;
				server_configuration();}
				break;


				case 4: //Befehl ATCGATT=1

				if (!strcmp(mystring, uart_string))
				{ 	printf("%s\n\r",&uart_string);
					init_schritt++;
					warte_ok=0;
					printf("%d\n\r",init_schritt);
					server_configuration();
				}

				else if(warte_ok==5) {
					//	warte_ok++;
					warte_ok=0;
				server_configuration();}
				break;

				case 5 :
				//Befehl AT+CSST="interne.t-d1.de
				if (!strcmp(mystring, uart_string))
				{ 	printf("%s\n\r",&uart_string);
					init_schritt++;
					warte_ok=0;
					printf("%d\n\r",init_schritt);
					server_configuration();
				}

				else if(warte_ok==5) {
					//warte_ok++;
					warte_ok=0;
				server_configuration();}

				break;

				


				
				case 6 ://AT+CIIR

				if (!strcmp(mystring, uart_string))
				{ 	printf("%s\n\r",&uart_string);
					init_schritt++;
					warte_ok=0;
					printf("%d\n\r",init_schritt);
					server_configuration();
				}

				else if(warte_ok==5) {
					//warte_ok++;
					warte_ok=0;
				server_configuration();}
				break;

				case 7: //AT CIFSR
				printf("%s\n\r",&uart_string);
				printf("Schritt %d\n\r",strlen(&uart_string));

				if (strlen(&uart_string)==13)
				{ 	printf("%s\n\r",&uart_string);
					init_schritt++;
					warte_ok=0;
					printf("%d\n\r",init_schritt);
					server_configuration();
				}

				else if(warte_ok==5) {
					//warte_ok++;
					warte_ok=0;
				server_configuration();}
				//printf("hallo");
				/*if (!strcmp("ERROR", uart_string))
				{
				//init_schritt++;
				//warte_ok=0;
				/
				server_configuration();
				}

				else  {
				printf("%s\n\r wieso",uart_string);
				init_schritt++;
				printf("%d\n\r",init_schritt);
				server_configuration();}*/
				break;

				case 8 :

				if (!strcmp(mystring, uart_string))
				{ 	printf("%s\n\r",&uart_string);
					init_schritt++;
					warte_ok=0;
					printf("%d\n\r",init_schritt);
					server_configuration();
				}

				/*else if(!strcmp("TCP connection success",&uart_string))

				{printf("%s\n\r",&uart_string);}

				
				else if(!strcmp("CONNECT OK",&uart_string))

				{printf("%s\n\r",&uart_string);}*/
				

				else if(warte_ok==5) {
					warte_ok=0;
				server_configuration();}
				break;

				case 9:

				if (!strcmp("CLOSE OK", uart_string))
				{ 	printf("%s\n\r",&uart_string);
					init_schritt++;
					warte_ok=0;
					printf("%d\n\r",init_schritt);
					server_configuration();
				}

				else if(warte_ok==5) {
					warte_ok=0;
				server_configuration();}
				break;

				
				case 10:

				if (!strcmp(mystring, uart_string))
				{ 	printf("%s\n\r",&uart_string);
					init_schritt++;
					warte_ok=0;
					printf("%d\n\r",init_schritt);
					server_configuration();
				}

				else if(warte_ok==5) {
					warte_ok=0;
				server_configuration();}
				break;

				case 11:

				/*	if(!strcmp("+CPIN: READY",uart_string))
				{

				printf("%s\n\r",&uart_string); }

				else if(!strcmp(mystring,uart_string))
				{

				printf("%s\n\r",&uart_string); }*/
				if (!strcmp("SIM is ready", uart_string))
				{ 	printf("%s\n\r",&uart_string);
					init_schritt++;
					warte_ok=0;
					printf("%d\n\r",init_schritt);
					server_configuration();
				}

				else if(warte_ok==5) {
					warte_ok=0;
				server_configuration();}
				break;

				case 12 :

				/*	if (!strcmp(mystring, uart_string))
				{ 	printf("%s\n\r",&uart_string);}*/

				if(!strcmp("SERVER OK",uart_string)){
					init_schritt++;
					warte_ok=0;
					printf("%s\n\r",&uart_string);
					printf("%d\n\r",init_schritt);
					server_configuration();
				}

				else if(warte_ok==5) {
					warte_ok=0;
				server_configuration();}
				break;

				case 13 :

				if (strlen(uart_string)==13)
				{ 	printf("%s\n\r",&uart_string);
					init_schritt++;
					warte_ok=0;
					printf("%d\n\r",init_schritt);
					server_configuration();
				}

				else if(warte_ok==5) {
					warte_ok=0;
				server_configuration();}
				break;


				case 14:

				/*if(!strcmp(mystring,uart_string))
				{
				printf("%s\n\r",&uart_string); }*/
				if (!strcmp("STATE: SERVER LISTENING", uart_string))
				{ 	printf("%s\n\r",&uart_string);
					//init_schritt++;
					warte_ok=0;
					server_initialisierung=true;
					printf("%d\n\r",init_schritt);
					//server_configuration();
				}

				else if(warte_ok==5) {
					warte_ok=0;
				server_configuration();}
				break;
			}
			//printf("%s",&mystring);

}


void empfangen_string()
{
					
	printf("hallo");

}


		