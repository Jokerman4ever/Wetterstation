// Ansteuerung eines HD44780 kompatiblen LCD im 4-Bit-Interfacemodus
// http://www.mikrocontroller.net/articles/AVR-GCC-Tutorial/LCD-Ansteuerung
/*
Anschluss:
LCD		Bezeichnung			ATMega8
1		Vss					GND
2		Vcc					+5V
3		Vee					mit Poti an GND (Kontrast)
4		RS					PA6 --> PD0
5		RW					GND
6		EN					PA7 --> PD1
7		DB0					nv
8		DB1					nv
9		DB2					nv
10		DB3					nv
11		DB4					PA0 --> PD2
12		DB5					PA1 --> PD3
13		DB6					PA2 --> PD4
14		DB7					PA3 --> PD5
15		A					Hintergrundbeleuchtung Vorwiederstand (ca. 330Ohm)
16		K					Hintergrundbeleuchtung GND
*/
//
//void lcd_data(unsigned char temp1);
void lcd_string(char *data);
//void lcd_command(unsigned char temp1);
void lcd_set_contrast(uint8_t contrast);
void lcd_enable(void);
void lcd_init(void);
void lcd_home(void);
void lcd_clear(void);
void lcd_set_cursor(uint8_t x, uint8_t y);
void lcd_Write(uint8_t data,uint8_t type);
void lcd_WNibble(uint8_t data);
void lcd_generatechar(uint8_t code, const uint8_t *data);
void lcd_Xstring(uint8_t *data,uint8_t isPGM);
// Hier die verwendete Taktfrequenz in Hz eintragen, wichtig!
// LCD Befehle
 
#define CLEAR_DISPLAY 0x01
#define CURSOR_HOME   0x02
 
// Pinbelegung für das LCD, an verwendete Pins anpassen

#define LCD_PORT      PORTB.OUT
#define LCD_DDR       PORTB.DIR
#define LCD_D7        4 
#define LCD_D6        5 
#define LCD_D5        6 
#define LCD_D4        7 
#define LCD_EN        1 
#define LCD_RS        0 
#define LCD_RESET	  3
// DB4 bis DB7 des LCD sind mit PD0 bis PD3 des AVR verbunden 

// Set CG RAM Address --------- 0b01xxxxxx  (Character Generator RAM)
#define LCD_SET_CGADR           0x40

#define LCD_GC_CHAR0            0
#define LCD_GC_CHAR1            1
#define LCD_GC_CHAR2            2
#define LCD_GC_CHAR3            3
#define LCD_GC_CHAR4            4
#define LCD_GC_CHAR5            5
#define LCD_GC_CHAR6            6
#define LCD_GC_CHAR7            7

////////////////////////////////////////////////////////////////////////////////
extern uint8_t  GC_Char0[8];
extern uint8_t  GC_Char1[8];