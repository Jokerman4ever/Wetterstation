/*
* display.c
*
* Created: 20.04.2016 10:53:37
*  Author: Saba-Sultana SAquib
*/

#include <avr/io.h>
#include "display.h"
#include <util/delay.h>

//Festlegung der Namen der Segmente mit der jeweiligen Ausgabe (HIGH, LOW), (an, aus)

#define segmenta_an PORTE.OUTSET= PIN0_bm;
#define segmentb_an PORTE.OUTSET= PIN1_bm;
#define segmentc_an PORTE.OUTSET= PIN2_bm;
#define segmentd_an PORTE.OUTSET= PIN3_bm;
#define segmente_an PORTE.OUTSET= PIN4_bm;
#define segmentf_an PORTE.OUTSET= PIN5_bm;
#define segmentg_an PORTE.OUTSET= PIN6_bm;
#define segmenta_aus PORTE.OUTCLR= PIN0_bm;
#define segmentb_aus PORTE.OUTCLR= PIN1_bm;
#define segmentc_aus PORTE.OUTCLR= PIN2_bm;
#define segmentd_aus PORTE.OUTCLR= PIN3_bm;
#define segmente_aus PORTE.OUTCLR= PIN4_bm;
#define segmentf_aus PORTE.OUTCLR= PIN5_bm;
#define segmentg_aus PORTE.OUTCLR= PIN6_bm;
int wert=0;

void led_blinke(){

//segmente_an;
//_delay_ms(200);
segmente_an;
segmentf_an;
segmentd_an;

}
//Hier wird die Anzeige eingeschaltet, jenachdem welcher Wert für i übergeben wird,
//wird diejenige Zahl angeschaltet.
int anzeige_an(int i)
{
	switch (i)
	{
		case 0:
		segmenta_an;
		segmentb_an;
		segmentc_an;
		segmentd_an;
		segmente_an;
		segmentf_an;
		segmentg_aus;
		wert=0;
		break;
		
		case 1:
		segmenta_aus
		segmentb_an;
		segmentc_an;
		segmentd_aus;
		segmente_aus;
		segmentf_aus;
		segmentg_aus;
		wert=1;
		break;
		
		case 2:
		segmenta_an;
		segmentb_an;
		segmentc_aus;
		segmentd_an;
		segmente_an;
		segmentf_aus;
		segmentg_an;
		wert=2;
		break;
		
		case 3:
		segmenta_an;
		segmentb_an;
		segmentc_an;
		segmentd_an;
		segmente_aus;
		segmentf_aus;
		segmentg_an;
		wert=3;
		break;
		
		case 4:
		segmentb_an;
		segmentc_an;
		segmenta_aus;
		segmentd_aus;
		segmente_aus;
		segmentf_an;
		segmentg_an;
		wert=4;
		break;
		
		case 5:
		segmenta_an;
		segmentb_aus;
		segmentc_an;
		segmentd_an;
		segmente_aus;
		segmentf_an;
		segmentg_an;
		wert=5;
		break;
		
		case 6:
		segmenta_an;
		segmentb_aus;
		segmentc_an;
		segmentd_an;
		segmente_an;
		segmentf_an;
		segmentg_an;
		wert=6;
		break;
		
		case 7:
		segmenta_an;
		segmentb_an;
		segmentc_an;
		segmentd_aus;
		segmente_aus;
		segmentf_aus;
		segmentg_aus;
		wert=7;
		break;
		
		case 8:
		segmenta_an;
		segmentb_an;
		segmentc_an;
		segmentd_an;
		segmente_an;
		segmentf_an;
		segmentg_an;
	    wert=8;
		break;
		
		case 9:
		segmenta_an;
		segmentb_an;
		segmentc_an;
		segmentd_an;
		segmente_aus;
		segmentf_an;
		segmentg_an;
	    wert=9;
		break;
	}

}

// Hier wird die komplette Anzeige ausgeschaltet
int anzeige_aus()
{   segmenta_aus;
	segmentb_aus;
	segmentc_aus;
	segmenta_aus;
	segmentd_aus;
	segmente_aus;
	segmentf_aus;
	segmentg_aus;
	
}

// Hier wird das Display initialisiert
void disp_init(){
PORTE.DIR=0xFF;
 segmenta_an;
 segmentb_an;
segmentc_an;
segmentd_an;
 segmente_an;
segmentf_an;
 segmentg_an;
	/*PORTE.DIRSET= PIN0_bm;
	PORTE.DIRSET= PIN1_bm;
	PORTE.DIRSET= PIN2_bm;
	PORTE.DIRSET= PIN3_bm;
	PORTE.DIRSET= PIN4_bm;
	PORTE.DIRSET= PIN5_bm;
	PORTE.DIRSET= PIN6_bm;
	PORTE.DIRSET= PIN7_bm;*/
	
	
	
}

void hochzaehlen(){
	
	
	for(int i=0; i<10; i++)
	{
		anzeige_an(i);
		_delay_ms(1000);
	}
	
	
}

void anzeige_abfrage(){
	
	for(int s=0; s<wert; s++)
	{
		
		anzeige_an(wert);
		_delay_ms(1000);
		anzeige_aus();
		_delay_ms(1000);
	}
	
}

//Als zweite Option war es möglich die Zahlen einzeln aufzurufen. Aber
//hat sich später als ungünstig erwiesen
/*void null( )
{
segmentb_an;
segmentc_an;
segmenta_an;
segmentd_an;
segmente_an;
segmentf_an;
segmentg_aus;
}

void eins( )
{

segmentb_an;
segmentc_an;
segmenta_aus;
segmentd_aus;
segmente_aus;
segmentf_aus;
segmentg_aus;
};



void zwei( )
{	segmentb_an;
segmentc_aus;
segmenta_an;
segmentd_an;
segmente_an;
segmentf_aus;
segmentg_an;
}
void drei( )
{
segmentb_an;
segmentc_an;
segmenta_an;
segmentd_an;
segmente_aus;
segmentf_aus;
segmentg_an;
}
void vier( )
{	segmentb_an;
segmentc_an;
segmenta_aus;
segmentd_aus;
segmente_aus;
segmentf_an;
segmentg_an;
}
void fuenf( )
{
segmentb_aus;
segmentc_an;
segmenta_an;
segmentd_an;
segmente_aus;
segmentf_an;
segmentg_an;
}
void sechs( )
{
segmentb_aus;
segmentc_an;
segmenta_an;
segmentd_an;
segmente_an;
segmentf_an;
segmentg_an;

}
void sieben( )
{
segmentb_an;
segmentc_an;
segmenta_an;
segmentd_aus;
segmente_aus;
segmentf_aus;
segmentg_aus;
}
void acht( )
{
segmentb_an;
segmentc_an;
segmenta_an;
segmentd_an;
segmente_an;
segmentf_an;
segmentg_an;

}
void neun( )
{
segmentb_an;
segmentc_an;
segmenta_an;
segmentd_an;
segmente_aus;
segmentf_an;
segmentg_an;
}
*/
