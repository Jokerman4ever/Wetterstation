/*
 * ErrorList.h
 *
 * Created: 25.06.2017 15:03:44
 *  Author: Felix
 */ 


#ifndef ERRORLIST_H_
#define ERRORLIST_H_

typedef enum ERRORID {
	ERRORID_SENSOR_NOTRESPONDING=1,
	ERRORID_SENSOR_WRONGVALUE=2,//Sensor werte nicht im richtigen bereich
	ERRORID_SENSOR_TIMEOUT=3, //Sensorinterface timeout
	ERRORID_STATION_NOTRESPONDING=10,//Station hat sich seit 15 minuten nicht mehr gemeldet
	ERRORID_STATION_BADCONNECTION=11,//Station hat hohen packetverlust / hohe widerholraten
	ERRORID_STATION_BATTERYLOW=12,//Station meldet das die Batterie bald leer ist!
	ERRORID_GSM_BADCONNECTION=20,//GSM Modul hat schlechte verbindung zum Netz
	ERRORID_GSM_CARDNOTRESPONDING=21,//GSM SIM Karte reagiert nicht
	ERRORID_LOCAL_BATTERYLOW=30 //Batterie der Hauptstation Schwach
} ERRORID_t;


#endif /* ERRORLIST_H_ */