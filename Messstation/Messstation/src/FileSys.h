/*
 * FileSys.h
 *
 * Created: 15.06.2017 12:36:44
 *  Author: Stud
 */ 


#ifndef FILESYS_H_
#define FILESYS_H_

#include <avr/io.h>
#include "Flash.h"
#include "eeprom_driver.h"



typedef struct FS_Status
{
	uint32_t CurrentUnix;
	uint8_t UnixCounter;
	FlashAddress NextAddress;
	uint32_t RecordCount;
	int32_t LastSearchResult;
	uint8_t FullCircle;//Is 1 if the Flash is completely full and is refilling itself from the start
	uint16_t NextErrorAddress;
	uint8_t ErrorLogFull;//Is 1 if the Errorlog is completely full, the user needs to read the errors and clean them
	uint32_t FileSpaceAvailable;
	uint8_t FileCount;
}FS_Status_t;

typedef struct FS_File
{
	uint32_t Start;
	uint16_t Length;
	uint8_t ID;
	uint8_t Flag;//Unused
	uint32_t ReadPos;
}FS_File_t;

typedef struct FS_StationRecord
{
	uint32_t Unix;
	uint32_t Position;
	uint8_t ID;
	uint16_t Temperatur;
	uint16_t Pressure;
	uint8_t LightStrength;
	uint8_t RainState;
	uint8_t WindLevel;
	uint8_t WindDirection;
	uint16_t Humidity;
	uint8_t Flags; // Fillbyte to get 16bytes!!! could be empty ...
}FS_StationRecord_t;

typedef struct FS_ErrorRecord
{
	uint32_t Unix;
	uint8_t ID;
	uint8_t Flags;
}FS_ErrorRecord_t;


void FS_Init(void);
void FS_FirstRun(void);
void FS_WriteRecord(FS_StationRecord_t fs);
void FS_WriteRecordHW(FS_StationRecord_t fs);
void FS_ReadRecordHW(uint32_t record,FS_StationRecord_t* fs);
void FS_Update(void);
uint8_t FS_FindRecord(uint32_t unix,uint32_t* recordOut);
FlashAddress FS_CreateNextAddress(void);
uint32_t FS_GetRecordUnix(uint32_t record);
uint8_t FS_GetRecords(uint32_t unix,FS_StationRecord_t* fs);
uint32_t FS_GetUnix(void);
void FS_ResetRecordSearch(void);

void FS_ClearErrors(void);
void FS_AddError(uint32_t unix,uint8_t id,uint8_t flag);
void FS_GetError(uint16_t eid,FS_ErrorRecord_t* record);
uint16_t FS_GetLastErrorID(void);


uint8_t FS_CreateNewEntry(uint32_t length);
void FS_RemoveEntry(uint8_t ID);
void FS_WriteFile(uint8_t ID, uint8_t *buffer,uint8_t offset,uint8_t length);
void FS_SetReadPos(uint32_t position);
uint8_t FS_ReadFile(uint8_t ID,uint8_t *buffer,uint8_t length);

#endif /* FILESYS_H_ */