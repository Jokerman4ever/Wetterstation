/*
* usart.c
*
* Created: 25.05.2016 08:49:12
*  Author: Stud
*/// #include "usart_driver.h"


#include "com.h"
#include "string.h"
#include "Http/server.h"
#include "Storage/FileSys.h"
#include <avr/interrupt.h>
unsigned char nextChar;
int init_schritt=-2;
int8_t alter_schritt=0;
extern volatile uint8_t uart_str_complete;
extern uint8_t daten_enmpfangen;   // 1 .. String komplett empfangen
volatile uint8_t uart_str_count = 0;
volatile uint8_t uart_string[UART_MAXSTRLEN + 1]="";
extern uint8_t server_initialisierung= false;
uint8_t kommando_senden;
FS_StationRecord_t record;
uint8_t ip_adresse;

int Counter = 0x00;
uint8_t warte_ok=0;
uint8_t recBuffer[UART_MAXSTRLEN];
uint8_t waitForString=1;
void com_init(void)
{
//	sysclk_enable_module(SYSCLK_PORT_F, SYSCLK_USART0);
	//PORTF.DIR = 0xFF;
	//PORTF.OUT = 0xFF;
	USARTF0.BAUDCTRLB = 0;
	USARTF0.BAUDCTRLA = 12;//9600baud
	USARTF0.CTRLA = USART_RXCINTLVL_HI_gc;
	USARTF0.CTRLB = USART_TXEN_bm | USART_RXEN_bm;
	USARTF0.CTRLC = USART_CHSIZE_8BIT_gc;
	waitForString=1;
	PORTF.DIRSET = (1<<3)|(1<<4);//TX(3) und RST(4) auf ausgang
	//WIrd hier nicht mehr ben�tigt (RESet GSM
	/*PORTF.OUTCLR = (1<<4);
	_delay_ms(200);//Modul reset
	PORTF.OUTSET = (1<<4);
	_delay_ms(3000);//Wait till Modul has finished startup*/
}

uint8_t com_hasData(void)
{
	return((USARTF0_STATUS & USART_RXCIF_bm));
}

com_strlen(uint8_t* data)
{
	for (uint8_t i = 0; i < 32; i++)
	{
		if(data[i] == 0)return i;
	}
}


void com_send_string(uint8_t* data)
{
	uint8_t length = 0x00;
	uint8_t counter = 0x00;
	length = com_strlen(data);
	
	while(counter < length)
	{
		com_ausgabe(data[counter]);
		counter++;
	}
	com_ausgabe(0x0A);
	com_ausgabe(0x0D);
}



// Damit SABA zu hause testen kann
void interrupt_init(void)
{
	PMIC.CTRL |= PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;													// Interrupts (Highlevel, Mediumlevel und Lowlevel freigeben)
	sei();
}

void com_ausgabe(uint8_t data)
{
	while(!(USARTF0.STATUS & USART_DREIF_bm)); // �berpr�fung ob fertig mit schreiben
	USARTF0.STATUS |= USART_TXCIF_bm;
	USARTF0.DATA = data;
	while(!(USARTF0.STATUS & USART_TXCIF_bm)); // �berpr�fung ob fertig mit schreiben
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

// Wird sp�ter beim Interrupt ben�tigt
ISR(USARTF0_RXC_vect)
{
	waitForString=0;
	nextChar = USARTF0.DATA;

	//printf("hallo");
	//segmenta_aus;
	if( uart_str_complete == 0 )
	{
		// wenn uart_string gerade in Verwendung, neues Zeichen verwerfen
		//send_string("bin da");
		// Daten werden erst in uart_string geschrieben, wenn nicht String-Ende/max Zeichenl�nge erreicht ist/string gerade verarbeitet wird
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
			server_configuration_auswertung(uart_string);
		
		}
	}
}
// Hier sind die einzelnen Schritte f�r die Serverkonfiguration 
void server_configuration()
{
	for (uint8_t i = 0; i <UART_MAXSTRLEN; i++)
	{
		recBuffer[i]=0;
	}
	switch(init_schritt)
	{   
		case -3:
		 {
			PORTF.OUTCLR = (1<<4);
		//	_xdelay_ms(200);//Modul reset
			PORTF.OUTSET = (1<<4);
			//_xdelay_ms(3000);//Wait till Modul has finished startup
			break;
		}
		case -2: com_send_string("AT"); break;
		//case -1:com_send_string("AT+IPR=38400"); break;
		case -1:com_send_string("AT+CSQ"); break;
		case 0:com_send_string("AT+CREG?");  break;
		//case 2: send_string("AT+CGACT?") ; break;
		//case 3: com_send_string("AT+CMEE=1");  break;
		case 1: com_send_string("AT+CGATT=1"); break;
		case 2: com_send_string("AT+CSTT=\"internet.t-d1.de\"");  break;
		case 3: com_send_string("AT+CIICR"); break;
		case 4: com_send_string("AT+CIFSR");break;
		//case 8: com_send_string("AT+CIPSTART=\"TCP\",\"74.124.194.252\",\"80\""); break;
		//case 9: com_send_string("AT+CIPCLOSE=0");break;
		//case 10: com_send_string("AT+CFUN=1"); break;
		//case 11: com_send_string("AT+CPIN?"); break;
		case 5: com_send_string("AT+CIPSERVER=1,80"); break;
		//case 13: com_send_string("AT+CIFSR"); break;
		case 6: com_send_string("AT+CIPSTATUS"); break;
	}
	if(init_schritt == -3)
	{
		return;
	}
	//uint8_t reclen= com_getString(recBuffer);
	
	//while(waitForString)_delay_ms(1);
	//server_configuration_auswertung(reclen,&step);
	
}

void server_configuration_auswertung(uint8_t antwort[])
{

alter_schritt=init_schritt;
	//if(len <= 2){_xdelay_ms(5000); init_schritt--; return;}		
	switch(init_schritt)
	{
		case -1:
		{

		//com_send_string(antwort);
			for (uint8_t i = 0; i < UART_MAXSTRLEN; i++)
			{ 
				if(antwort[i] == ':')
				{ 
				//com_send_string("hallo");
				//com_ausgabe(antwort[i]);
					i+=2;
					if(antwort[i] != '0')
					{
						init_schritt++;
					//	 com_ausgabe(init_schritt);
						// com_ausgabe(alter_schritt);
						server_configuration();
						return;
					}

					else {init_schritt--;
					server_configuration();
				}
				}
			}
			break;
		}
		case 0:
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
							{
							    
								init_schritt++;
							//	com_ausgabe(init_schritt);
							//	com_ausgabe(alter_schritt);
								server_configuration();
								return;
							}

							
							else {init_schritt=-2;
							//	server_configuration();
							}
						}
					}
				}
			}
			_xdelay_ms(2000);
			break;
		}
		case -2: case 1: case 2: case 3:
		{
			if(!strcmp("OK", antwort))
			{
				init_schritt++;
			//	_xdelay_ms(2000);
			}
			else if(!strcmp("ERROR", antwort))
			{
			init_schritt=-2;
				//_xdelay_ms(2000);
			}
			break;
		}
		case 4:
		{
			if(!strcmp("ERROR", antwort))
			{
				init_schritt=-2;
			}
			else
			{
				/*for (uint8_t i = 0; i < len; i++)
				{
					
				}
				
				len= ip_adresse;*/
				init_schritt++;
				ip_adresse=1;
				//server_configuration();
			}
			break;
		}
		case 5:
		{
			if(!strcmp("OK", uart_string))
			{
			init_schritt++;
			//	_xdelay_ms(5000);
			}
			else if(!strcmp("ERROR", uart_string))
			{
			init_schritt=-2;
				//_xdelay_ms(5000);
				//return;
			}
			break;
		}
		case 6:
		{
			if(!strcmp("STATE: SERVER LISTENING", antwort))
			{
			init_schritt++;
				//_xdelay_ms(5000);
			}
			else if(!strcmp("ERROR", antwort))
			{
				init_schritt=-2;
				//_xdelay_ms(5000);
				return;
			}
			break;
		}
	//	init_schritt--;
	}
	if(alter_schritt!=init_schritt|| init_schritt==-2)
	{
	server_configuration();
	}
	if(init_schritt==7)
	{
	com_send_string("SERVER ist konfiguriert");
	return;
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

