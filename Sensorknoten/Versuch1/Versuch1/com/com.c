/************************************************************************/
/*				Praktikum Drahtlose Sensorsysteme                       */
/*				Versuch Kommunikation									*/
/*				Datum: 01.06.2017                                       */
/*				Author: Dennis Michalik                                 */
/*				Matrikelnummer: 257934									*/
/************************************************************************/

#include "com.h"

// Deklaration einer FILE Variablen zur Nutzung der printf() Methode
static FILE mystdout = FDEV_SETUP_STREAM(ioputchar,NULL,_FDEV_SETUP_WRITE);

// Funktion zur Initialisierung des USART im Asynchron Modus mit einer Baurate von 9600 Bit/s, 8 Bit, 1 Stopbit, Kein Pritätsbit
com_init()
{
	USARTC1.BAUDCTRLB = 0x00;
	USARTC1.BAUDCTRLA = com_adjustbaudrate(9600);
	USARTC1.CTRLA = USART_RXCINTLVL_HI_gc;
	USARTC1.CTRLB = USART_TXEN_bm;
	//USARTC1.CTRLB = USART_TXEN_bm | USART_RXEN_bm;				// RXEN rausnehmen
	USARTC1.CTRLC = USART_CHSIZE_8BIT_gc;
	PORTC.DIR = PIN7_bm;
	stdout = &mystdout;
	PMIC.CTRL |= PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
	sei();
}

// Funktion zur Ausgabe eines Zeichens auf der COM-Schnittstelle
void com_sendchar(char sign)
{
	while (!(USARTC1.STATUS & USART_DREIF_bm));
	USARTC1.DATA = sign;
}

/*
* Funktion zur Bestimmung des Wertes im BAUDCTRLA Register und damit der Einstellung der Baudrate
* Parameter:
* uint32_t baudrate: Variable zur Übergabe der Baudrate, im Funktionsaufruf kann die Baudrate als Zahl übertragen werden z.B. com_adjustbaudrate(9600);
* Rückgabe: uint8_t -> Ein Byte welches dem Wert für das BAUDCTRLA Register entspricht. 
* Die Berechnung erfolgt über die Formel auf dem Datenblatt (Seite 238, Asynchronous Normal Speed)
*/
uint8_t com_adjustbaudrate(uint32_t baudrate)
{
	return ((F_CPU/(baudrate*(uint32_t)16))-1);
}

// Funktion zur Verwendung der printf() Methode
int ioputchar(char c, FILE *f)
{
	com_sendchar(c);
	return 0;
}

ISR(USARTC1_RXC_vect)
{
	printf("test\r\n");
	_delay_ms(2000);
}