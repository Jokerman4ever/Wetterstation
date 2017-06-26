/*
 * FileSys.c
 *
 * Created: 15.06.2017 12:01:19
 *  Author: Stud
 */ 
#include "FileSys.h"
#include "ErrorList.h"

FS_Status_t FS_CurrentStatus;
FS_StationRecord_t FS_TempRecord;
#define FS_EEPROM_StartAddress 16
#define FS_EEPROM_Range 24

#define FS_StartAddress_Errors 4096
#define FS_StopAddress_Errors 8176
#define FS_MaxErrorCount 680

#define FS_StartAddress_Files 8192
#define FS_StopAddress_Files 131000UL

#define FS_StartAddress_Records 131072UL

//Stop is 2^24 -> 128MBit - 68 damits aufgeht...
#define FS_StopAddress 16777148UL
//(StopAddress - StartAddress) / 16
#define FS_MaxRecordCount ((FS_StopAddress - FS_StartAddress_Records) / 16)

//Fügt dem Fehlerspeicher einen Fehler hinzu.
void FS_AddError(uint32_t unix,ERRORID_t id,uint8_t flag)
{
	uint16_t add = FS_CurrentStatus.NextErrorAddress;
	if(add > FS_StopAddress_Errors)return;
	uint8_t buffer[6];
	buffer[0]=(unix>>24)& 0xff;buffer[1]=(unix>>16)& 0xff;buffer[2]= (unix>>8)& 0xff;buffer[3]=unix & 0xff;
	buffer[4]=id & 0xff;
	buffer[5]=flag & 0xff;
	Flash_write_Bytes(Flash_CreateAddress(0,0,(add<<8) & 0xff,add & 0xff),buffer,0,6);
	add +=6;
	if(add>FS_StopAddress_Errors){FS_CurrentStatus.ErrorLogFull=1;EEPROM_WriteByte(FS_EEPROM_StartAddress+13,1);}
	FS_CurrentStatus.NextErrorAddress=add;
	EEPROM_WriteDWord(FS_EEPROM_StartAddress+9,add);
}

//Löscht alle Fehler aus dem Speicher
void FS_ClearErrors(void)
{
	Flash_sector_Erase(Flash_CreateAddress(0,0,FS_StartAddress_Errors<<8 & 0xff,FS_StartAddress_Errors & 0xff));
	FS_CurrentStatus.NextErrorAddress = FS_StartAddress_Errors;
	FS_CurrentStatus.ErrorLogFull=0;
}

//Gibt den Fehler anhand einer FehlerID zurück.
//Die letzte FehlerID kann mit FS_GetLastErrorID() ermittelt werden!
//eid-> (0 - FS_MaxErrorCount)
void FS_GetError(uint16_t eid,FS_ErrorRecord_t* record)
{
	uint16_t add = FS_StartAddress_Errors + eid*6;
	uint8_t buffer[6];
	Flash_read_Bytes(Flash_CreateAddress(0,0,(add>>8)&0xff,add&0xff),buffer,6);
	record->Unix = ((uint32_t)buffer[0]<<24) |((uint32_t)buffer[1]<<16) |((uint32_t)buffer[2]<<8) |((uint32_t)buffer[3]);
	record->ID = buffer[3];
	record->Flags = buffer[4];
}

//Gibt die FehlerID zurück, die zuletzt eingetragen wurden.
uint16_t FS_GetLastErrorID(void)
{
	return ((FS_CurrentStatus.NextErrorAddress - FS_StartAddress_Errors) /6);
}




void FS_Init(void)
{
	FS_CurrentStatus.CurrentUnix = FS_GetUnix();
	//FS_CurrentStatus.LastFileIndex = FS_GetLastFileIndex(); // Funktion???
	uint32_t addr = EEPROM_ReadDWord(FS_EEPROM_StartAddress);
	FS_CurrentStatus.NextAddress = Flash_CreateAddress((addr>>24)&0xff,(addr>>16)&0xff,(addr>>8)&0xff,(addr)&0xff);
	FS_CurrentStatus.RecordCount = EEPROM_ReadDWord(FS_EEPROM_StartAddress+4);
	FS_CurrentStatus.FullCircle = EEPROM_ReadByte(FS_EEPROM_StartAddress+8);
	FS_CurrentStatus.NextErrorAddress = (uint16_t)EEPROM_ReadDWord(FS_EEPROM_StartAddress+9);
	FS_CurrentStatus.ErrorLogFull = EEPROM_ReadByte(FS_EEPROM_StartAddress+13);
	FS_CurrentStatus.LastSearchResult=-1;
}

void FS_FirstRun(void)
{
	//Clear our buffer
	for (uint8_t i=0; i<FS_EEPROM_Range; i++)
	{
		EEPROM_WriteByte(i+FS_EEPROM_StartAddress,0);
	}
	EEPROM_WriteDWord(FS_EEPROM_StartAddress,FS_StartAddress_Records);//NextAddress
	EEPROM_WriteDWord(FS_EEPROM_StartAddress+4,0);//Record Count
	EEPROM_WriteByte(FS_EEPROM_StartAddress+8,0);//FullCircle
	EEPROM_WriteDWord(FS_EEPROM_StartAddress+9,0);//ErrorNextaddress
	EEPROM_WriteByte(FS_EEPROM_StartAddress+13,0);//ErrorLogFull
	FS_CurrentStatus.NextAddress = Flash_CreateAddress(((uint32_t)FS_StartAddress_Records>>24)&0xff,((uint32_t)FS_StartAddress_Records>>16)&0xff,((uint32_t)FS_StartAddress_Records>>8)&0xff,(uint32_t)FS_StartAddress_Records &0xff);
	FS_CurrentStatus.RecordCount = 0;
	FS_CurrentStatus.FullCircle = 0;
	FS_CurrentStatus.ErrorLogFull = 0;
	FS_CurrentStatus.NextErrorAddress = FS_EEPROM_StartAddress;
	uint32_t add=FS_StartAddress_Records;
	//Sind das nu 4kb sectoren oder 64kb sectoren????
	for (; add<FS_StopAddress; add+=4096)
	{
		FlashAddress addr = Flash_CreateAddress(add>>24,add>>16,add>>8,add);
		Flash_sector_Erase(addr);
		while(Flash_isBusy()){_delay_us(10);}
	}
}

//bei fullcircle muss der Sector vor der beschreibung gelöscht werden!!!
//macht das probleme??? wenn daten nicht genau aufgehen ja!
void FS_WriteRecord(FS_StationRecord_t fs)
{
	FS_WriteRecordHW(fs);
	FS_CurrentStatus.NextAddress = FS_CreateNextAddress();
	uint32_t add = ((uint32_t)FS_CurrentStatus.NextAddress.High<<24) | ((uint32_t)FS_CurrentStatus.NextAddress.Mid<<16) | ((uint32_t)FS_CurrentStatus.NextAddress.Low<<8) | (uint32_t)FS_CurrentStatus.NextAddress.XLow;
	EEPROM_WriteDWord(FS_EEPROM_StartAddress,add);
	EEPROM_WriteDWord(FS_EEPROM_StartAddress+4,FS_CurrentStatus.RecordCount);
}

void FS_WriteRecordHW(FS_StationRecord_t fs)
{
	uint8_t buffer[16];
	buffer[0]=(fs.Unix>>24)& 0xff;buffer[1]=(fs.Unix>>16)& 0xff;buffer[2]= (fs.Unix>>8)& 0xff;buffer[3]=fs.Unix & 0xff;
	buffer[4]=fs.ID & 0xff;
	buffer[5]=fs.LightStrength;
	buffer[6]=fs.RainState;
	buffer[7]=fs.WindLevel;
	buffer[8]=fs.WindDirection;
	buffer[9]=(fs.Temperatur>>8) & 0xff; buffer[10]=fs.Temperatur & 0xff;
	buffer[11]=(fs.Humidity>>8) & 0xff; buffer[12]=fs.Humidity & 0xff;
	buffer[13]=(fs.Pressure>>8)& 0xff;buffer[14]=fs.Pressure & 0xff;
	buffer[15]=fs.Flags;
	Flash_write_Bytes(FS_CurrentStatus.NextAddress,buffer,0,16);
}

void FS_ReadRecordHW(uint32_t record,FS_StationRecord_t* fs)
{
	uint32_t addr = FS_StartAddress_Records+record*16;
	FlashAddress add = Flash_CreateAddress((addr>>24)&0xff,(addr>>16)&0xff,(addr>>8)&0xff,(addr)&0xff);
	uint8_t buffer[16];
	Flash_read_Bytes(add,buffer,16);
	fs->Unix = ((uint32_t)buffer[0]<<24) |((uint32_t)buffer[1]<<16) |((uint32_t)buffer[2]<<8) |((uint32_t)buffer[3]);
	fs->ID = buffer[4];
	fs->LightStrength = buffer[5];
	fs->RainState = buffer[6];
	fs->WindLevel = buffer[7];
	fs->WindDirection = buffer[8];
	fs->Temperatur = ((uint32_t)buffer[9]<<8) |((uint32_t)buffer[10]);
	fs->Humidity = ((uint32_t)buffer[11]<<8) |((uint32_t)buffer[12]);
	fs->Pressure = ((uint32_t)buffer[13]<<8) |((uint32_t)buffer[14]);
	fs->Flags = buffer[15];
	fs->Position = record;
}


void FS_Update(void)
{
	//Triggers every 10ms
	if(FS_CurrentStatus.UnixCounter++ > 99)
	{
		FS_CurrentStatus.UnixCounter=0;
		FS_CurrentStatus.CurrentUnix++;	
	}
}



uint8_t FS_FindRecord(uint32_t unix,uint32_t* recordOut)
{
	uint32_t first=0;
	uint32_t last=FS_CurrentStatus.RecordCount-1;
	uint8_t found=0;
	uint32_t midpoint;
	if(!FS_CurrentStatus.FullCircle)
	{
		while(first<=last && !found)
		{
			midpoint = (first + last)/2;
			uint32_t NUnix= FS_GetRecordUnix(midpoint);
			if(NUnix==0)return 0; //midpoint is out of range... abort search
			if(abs(NUnix-unix) < 300)found = 1;
			else
			{
				if(unix < NUnix)last = midpoint-1;
				else first = midpoint+1;
			}
		}
	}
	else
	{
		//Wenn wir im fullcircle sind dann klappt das nicht mehr!!!! 
		//Weil hier schon datensätze überschrieben wurden und die größte unix mitten im speicher stehen kann
		//hier muss nun eine doppel search ausgeführt werden die den oberen teil der liste und den unteren teil seperat behandelt
		//hierfür muss der circle punkt genau bestimmt werde(das heißt die position des letzten eintrages (warscheinlich berechenbar durch die NextAddress))
		//Zu anfang festlegen, ob die unix im oberen oder unteren teil der liste ist... dann suchen 
	}
	if(found)*recordOut=midpoint;
	return found;
}

FlashAddress FS_CreateNextAddress(void)
{
	//FS_FileIndex_t lastindex = FS_LoadFileIndex(FS_CurrentStatus.LastFileIndex);
	uint32_t add = ((uint32_t)FS_CurrentStatus.NextAddress.High<<24) | ((uint32_t)FS_CurrentStatus.NextAddress.Mid<<16) | ((uint32_t)FS_CurrentStatus.NextAddress.Low<<8) | (uint32_t)FS_CurrentStatus.NextAddress.XLow;
	add+= 16;
	if(!FS_CurrentStatus.FullCircle)FS_CurrentStatus.RecordCount++;
	if(add > FS_StopAddress){add = FS_StartAddress_Records; FS_CurrentStatus.FullCircle=1; EEPROM_WriteByte(FS_EEPROM_StartAddress+8,1);}// REPORT ERROR -> FILE SYSTEM OVERFLOW!!! FullCircleMode activated!
	FlashAddress f = Flash_CreateAddress((add>>24) & 0xff,(add>>16) & 0xff,(add>>8) & 0xff,add & 0xff);
	return f;
}

uint32_t FS_GetRecordUnix(uint32_t record)
{
	uint8_t buf[4];
	uint32_t addr;
	if(record<FS_MaxRecordCount)
	{
		addr = FS_StartAddress_Records+(record*16);
		FlashAddress add = Flash_CreateAddress((addr>>24)&0xff,(addr>>16)&0xff,(addr>>8)&0xff,(addr)&0xff);
		Flash_read_Bytes(add,buf,4);
		return (((uint32_t)buf[0]<<24)|((uint32_t)buf[1]<<16)|((uint32_t)buf[2]<<8)|((uint32_t)buf[3]));
	}
	else return 0;
}

void FS_ResetRecordSearch(void)
{
	FS_CurrentStatus.LastSearchResult=-1;
}

uint8_t FS_GetRecords(uint32_t unix,FS_StationRecord_t* fs)
{
	if(FS_CurrentStatus.LastSearchResult==-1)
	{
		uint32_t record = 0;
		uint8_t suc = FS_FindRecord(unix,&record);
		if(suc)
		{
			FS_CurrentStatus.LastSearchResult = fs->Position;
			FS_ReadRecordHW(record,fs);
			return 1;
		}
		return 0;
	}
	else
	{
		FS_ReadRecordHW(FS_CurrentStatus.LastSearchResult-1,fs);
		if(FS_CurrentStatus.LastSearchResult-1 == 0)FS_CurrentStatus.LastSearchResult=(FS_CurrentStatus.RecordCount-1);
		else FS_CurrentStatus.LastSearchResult--;
		return 1;
	}
	//von welcher unix ab rückwerts, an welcher pos sind wir grade, fs is der ausgabe record
}

uint32_t FS_GetUnix(void)
{
	//MACHN DENNIS
	return 0;
}