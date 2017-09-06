// Ansteuerung eines HD44780 kompatiblen LCD im 4-Bit-Interfacemodus
// http://www.mikrocontroller.net/articles/AVR-GCC-Tutorial/LCD-Ansteuerung
//
// Die Pinbelegung ist über defines in lcd-routines.h einstellbar
 
#include <avr/io.h>
#include "lcd-routines.h"
#include "Clock/Xdelay.h"
#include <avr/pgmspace.h>
#include <ASF/common/services/clock/sysclk.h>
 
 uint8_t  GC_Char0[8] = {1,1,1,2,18,10,4,0};
 uint8_t  GC_Char1[8] = {0,17,10,4,10,17,0,0};
 
 // Schreibt ein Zeichen in den Character Generator RAM	 
 void lcd_generatechar(uint8_t code, const uint8_t *data)
 {
	 // Startposition des Zeichens einstellen
	 lcd_Write(LCD_SET_CGADR|(code<<3),0);
	 
	 // Bitmuster übertragen
	 for (uint8_t i=0; i<8; i++)
	 {
		 lcd_Write(data[i],0);
	 }
 }
 void lcd_WNibble(uint8_t data)
 {
	 LCD_PORT &= ~((1<<LCD_D7)|(1<<LCD_D6)|(1<<LCD_D5)|(1<<LCD_D4));//Port auf Low
	 LCD_PORT |= ((((data >> 3) & 0x01) << LCD_D7)) | ((((data >> 2) & 0x01) << LCD_D6)) |((((data >> 1) & 0x01) << LCD_D5)) | (((data & 0x01) << LCD_D4));
 }
 
 void lcd_Write(uint8_t data,uint8_t type)
 {
	 if(type) LCD_PORT |= (1<<LCD_RS);
	 else LCD_PORT &= ~(1<<LCD_RS);
	 uint8_t data2 = data;
	 data = data >> 4;
	 data = data & 0x0F;
	 //Remap...
	 lcd_WNibble(data);
	 lcd_enable();
	 data = data2 & 0x0F;
	 lcd_WNibble(data);
	 lcd_enable();
	 _xdelay_us(50);
 }
 /*
// sendet ein Datenbyte an das LCD
void lcd_data(unsigned char temp1)
{
   unsigned char temp2 = temp1;
 
   LCD_PORT |= (1<<LCD_RS);        // RS auf 1 setzen
 
   temp1 = temp1 >> 4;
   temp1 = temp1 & 0x0F;
   LCD_PORT &= 0xF0;
   LCD_PORT |= temp1;               // setzen
   lcd_enable();
   //_xdelay_us(100);//42
   temp2 = temp2 & 0x0F;
   LCD_PORT &= 0xF0;
   LCD_PORT |= temp2;               // setzen
   lcd_enable();
   
   _xdelay_us(50);
}
 
// sendet einen Befehl an das LCD
 
void lcd_command(unsigned char temp1)
{
   unsigned char temp2 = temp1;
 
   LCD_PORT &= ~(1<<LCD_RS);        // RS auf 0 setzen
 
   temp1 = temp1 >> 4;              // oberes Nibble holen
   temp1 = temp1 & 0x0F;            // maskieren
   LCD_PORT &= 0xF0;
   LCD_PORT |= temp1;               // setzen
   lcd_enable();
   //_xdelay_us(100);//42
   temp2 = temp2 & 0x0F;            // unteres Nibble holen und maskieren
   LCD_PORT &= 0xF0;
   LCD_PORT |= temp2;               // setzen
   lcd_enable();
   
   _xdelay_us(50);//42
}
 */
// erzeugt den Enable-Puls
void lcd_enable(void)
{
   // Bei Problemen ggf. Pause gemäß Datenblatt des LCD Controllers einfügen
   // http://www.mikrocontroller.net/topic/81974#685882
   LCD_PORT |= (1<<LCD_EN);
    _xdelay_us(25);                   // kurze Pause
   // Bei Problemen ggf. Pause gemäß Datenblatt des LCD Controllers verlängern
   // http://www.mikrocontroller.net/topic/80900
   
   LCD_PORT &= ~(1<<LCD_EN);
}
 
/**
 * Einstellen der Kontrastspannung des Displays in 13mV Schritten 
 */
void lcd_set_contrast(uint8_t contrast)
{
 DACB.CH0DATA = (contrast << 8);
}
 
// Initialisierung: 
// Muss ganz am Anfang des Programms aufgerufen werden.
void lcd_init(void)
{
   LCD_DDR |= (1<<LCD_D7) | (1<<LCD_D6) | (1<<LCD_D5) | (1<<LCD_D4) | (1<<LCD_RS) | (1<<LCD_EN);   // Port auf Ausgang schalten
   PORTB.DIRSET = (1<<LCD_RESET);
   PORTB.OUTSET = (1<<LCD_RESET); //Reset muss high sein!!!
	
	//Kontrast|DAC config
	PORTB.DIRSET = (1<<2);//DACs auf Ausgang
	sysclk_enable_module(SYSCLK_PORT_B, SYSCLK_DAC); //DAC Clock Enable
	DACB.CH0DATA = 0; // Output 0 Volts (apart from gain and offset error)
	DACB.CTRLB = DAC_CHSEL_DUAL_gc; // Dual channel operations  DAC_CHSEL_DUAL_gc;
	DACB.CTRLC = DAC_REFSEL_AVCC_gc | DAC_LEFTADJ_bm; // AVcc is the DAC reference voltage
	DACB.CTRLA = DAC_CH0EN_bm | DAC_ENABLE_bm; // Enable DAC and channel 0 DAC_CH0EN_bm |
	PORTB.OUT &= ~(1<<2);
	
	lcd_set_contrast(240); // Initale Kontrast einstellung
	
   // muss 3mal hintereinander gesendet werden zur Initialisierung
   
   lcd_WNibble(0x03);
   lcd_enable();
 
   _xdelay_ms(5);
   lcd_enable();
 
   _xdelay_ms(5);
   lcd_enable();
   
   _xdelay_ms(5);
 
   // 4 Bit Modus aktivieren 
   lcd_WNibble(0x02);
   lcd_enable();
   _xdelay_ms(5);
 
   // 4Bit / 2 Zeilen / 5x7
   lcd_Write(0x28,0); //0010 1100 0x28
   _xdelay_ms(5);
   // Display ein / Cursor aus / kein Blinken
   lcd_Write(0x0C,0); //0x0C 
   _xdelay_ms(5);
    //inkrement / kein Scrollen
   lcd_Write(0x06,0);
   _xdelay_ms(5);
   lcd_clear();
   _xdelay_ms(5);
   lcd_home();
   
   lcd_Write(0x0F,0);
}
 
// Sendet den Befehl zur Löschung des Displays
 
void lcd_clear(void)
{
   lcd_Write(CLEAR_DISPLAY,0);
   _xdelay_ms(10);
}
 
// Sendet den Befehl: Cursor Home
 
void lcd_home(void)
{
   lcd_Write(CURSOR_HOME,0);
   _xdelay_ms(10);
}
 
// setzt den Cursor in Zeile y (1..4) Spalte x (0..15)
 
void lcd_set_cursor(uint8_t x, uint8_t y)
{
  uint8_t tmp;
  switch (y) {
	case 1: tmp=0x00+0x00+x; break;    // 1. Zeile
	case 2: tmp=0x00+0x20+x; break;    // 2. Zeile
    case 3: tmp=0x00+0x40+x; break;    // 3. Zeile
    case 4: tmp=0x00+0x60+x; break;    // 4. Zeile
    default: return;                   // für den Fall einer falschen Zeile
  }
  lcd_Write(tmp,0);
}
 
// Schreibt einen String auf das LCD
 
void lcd_string(char *data)
{
    while(*data) {
        lcd_Write(*data,1);
        data++;
    }
}

void lcd_Xstring(uint8_t *add,uint8_t isPGM)
{
	uint8_t data;
	if(isPGM)data= pgm_read_byte(*add);
	else data = *add;
	while(data) 
	{
		if(data>127)
		{
			switch(data)
			{
				case 128: lcd_Write(LCD_GC_CHAR0,1); break;
				case 129: lcd_Write(LCD_GC_CHAR1,1); break;
				case 130: lcd_Write(LCD_GC_CHAR2,1); break;
				case 131: lcd_Write(LCD_GC_CHAR3,1); break;
				case 132: lcd_Write(LCD_GC_CHAR4,1); break;
				case 133: lcd_Write(LCD_GC_CHAR5,1); break;
				case 134: lcd_Write(LCD_GC_CHAR6,1); break;
				case 135: lcd_Write(LCD_GC_CHAR7,1); break;
				default: lcd_Write(data,1); break;
			}
			add++;
		}
		else
		{
			lcd_Write(data,1);
			add++;
		}
		if(isPGM)data= pgm_read_byte(*add);
		else data = *add;
	}
}

