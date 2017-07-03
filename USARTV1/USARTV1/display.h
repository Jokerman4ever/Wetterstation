#include <avr/io.h>

#ifndef display_h_
#define display_h_
// DEFINITIONEN
// Display Initialisierung
void disp_init();
//Anschalten der Anzeige mit �bergabe eines Integers
int anzeige_an(int );
//Auschalten der kompletten Anzeige
int anzeige_aus();
//Abfrage der Anzeige
void anzeige_abfrage();
//Funktion f�r das einfach hochz�hlen der Zahlen
void hochzaehlen();

void led_blinke();



#endif