/*
 * FileSys.c
 *
 * Created: 15.06.2017 12:01:19
 *  Author: Felix Mälk
 */ 

#include "FileSys.h"
#include "ErrorList.h"
static uint8_t FS_CheckAddress(uint8_t ID);
static uint32_t FS_CreateFileStart(void);
static uint32_t FS_GetFileLength(uint8_t ID);
static uint32_t FS_GetFileStart(uint8_t ID);
static uint8_t FS_CheckAddress(uint8_t ID);
static void FS_WriteFileInfo(uint8_t ID);
static uint8_t FS_LoadFileInfo(uint8_t ID);
static void FS_Reorganize(uint8_t ID);
static void FS_UpdateFileSys(void);
static void FS_SetAddress(uint8_t ID, uint8_t state);

static void FS_WriteRecordHW(FS_StationRecord_t* fs);
static void FS_ReadRecordHW(uint32_t record,FS_StationRecord_t* fs);
static FlashAddress FS_CreateNextAddress(void);

FS_Status_t FS_CurrentStatus;
FS_StationRecord_t FS_TempRecord;
FS_File_t FS_TempFile;
//////////////////////////////////////////////////////////////////////////
//						EEPROM-MAP
//FS_EEPROM_StartAddress
//+0 -> NextAddress for Records
//+4 -> Record Count
//+8 -> FullCircle - Indicates Record overflow status
//+9 -> NextErrorAddress
//+13 -> ErrorLogFull - Indicates Error overflow status
//+14 - 15 -> ----FREE------
//+16 - 17 -> Filetable  
//+32 - (32 + FS_MaxFileCount*8) -> FileInfos
//////////////////////////////////////////////////////////////////////////

#define FS_EEPROM_StartAddress 16
#define FS_EEPROM_Range 160

#define FS_StartAddress_Errors 4096
#define FS_StopAddress_Errors 8176
#define FS_MaxErrorCount 680

#define FS_StartAddress_Files 8192
#define FS_StopAddress_Files 131071UL
#define FS_MaxFileCount 16

#define FS_StartAddress_Records 131072UL

//Stop is 2^25 -> 256MBit - 68 damits aufgeht...
#define FS_StopAddress 33554364UL
//(StopAddress - StartAddress) / 16
#define FS_MaxRecordCount ((FS_StopAddress - FS_StartAddress_Records) / 16)

//Initialisiert das Dateisystem und den Flash-Speicher der im zugrunde liegt.
void FS_Init(void)
{
	FS_CurrentStatus.CurrentUnix = FS_GetUnix();
	//FS_CurrentStatus.LastFileIndex = FS_GetLastFileIndex(); // Funktion???
	uint32_t addr = EEPROM_ReadDWord(FS_EEPROM_StartAddress);
	FS_CurrentStatus.NextAddress = Flash_CreateAddress((addr>>24)&0xff,(addr>>16)&0xff,(addr>>8)&0xff,(addr)&0xff);
	FS_CurrentStatus.RecordCount = EEPROM_ReadDWord(FS_EEPROM_StartAddress+4);
	FS_CurrentStatus.RecordLogFull = EEPROM_ReadByte(FS_EEPROM_StartAddress+8);
	FS_CurrentStatus.NextErrorAddress = (uint16_t)EEPROM_ReadDWord(FS_EEPROM_StartAddress+9);
	FS_CurrentStatus.ErrorLogFull = EEPROM_ReadByte(FS_EEPROM_StartAddress+13);
	FS_CurrentStatus.LastSearchResult=-1;
	FS_UpdateFileSys();
}

//Wird einmalig ausgeführt und richtet das Dateisystem auf dem Flash-Speicher ein
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
	FS_CurrentStatus.RecordLogFull = 0;
	FS_CurrentStatus.ErrorLogFull = 0;
	FS_CurrentStatus.NextErrorAddress = FS_EEPROM_StartAddress;
	uint32_t add=FS_StartAddress_Records;
	//Sind das nu 4kb sectoren oder 64kb sectoren????
	for (; add<FS_StopAddress; add+=65536)
	{
		FlashAddress addr = Flash_CreateAddress(add>>24,add>>16,add>>8,add);
		Flash_sector_Erase(addr);
		while(Flash_isBusy()){_xdelay_us(10);}
	}
}

//Diese Funktion muss alle 10ms aufgerufen werden. Sie verwaltet die UNIX-Time mit der die Dateneinheiten richtig gespeichert werden.
void FS_Update(void)
{
	//Triggers every 10ms
	if(FS_CurrentStatus.UnixCounter++ > 99)
	{
		FS_CurrentStatus.UnixCounter=0;
		FS_CurrentStatus.CurrentUnix++;
	}
}

#pragma region ERROR SUPPORT
//Der Error support stellt funktionen für die Speicherung von systemweiten Fehlern bereit.


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

#pragma endregion ERROR SUPPORT

#pragma region FILE SUPPORT
//Der Filesupport stellt die unterstützung von Dateien bereit es können insgesamt 16 Dateien mit jeweils eine größe von 4KByte gespeichert werden
//Das Dateisystem verwaltet dabei alles voll automatisch. Es kann somit einfach eine Datei geschrieben/erweitert oder von jedem Punkt
//in der Datei gelesen werden.

//Erstellt einen neuen Speicherplatz für eine neue Datei
static uint32_t FS_CreateFileStart(void)
{
	if(FS_CurrentStatus.FileCount == 0) return FS_StartAddress_Files;
	uint32_t maxStart=0;
	uint8_t lastID=255;
	for (uint8_t i = 1; i < FS_MaxFileCount; i++)
	{
		if(FS_CheckAddress(i))
		{
			uint32_t start = FS_GetFileStart(i);
			if(start > maxStart)
			{
				maxStart = start;
				lastID = i;
			}
		}
	}
	if(lastID == 255)return 0; //no space!!!!
	maxStart = maxStart + FS_GetFileLength(lastID)+ (4096 -(maxStart + FS_GetFileLength(lastID))%4096);
	return maxStart;
}

//Gibt die Dateigröße einer Datei im Flash-Speicher zurück
static uint32_t FS_GetFileLength(uint8_t ID)
{
	if(FS_CheckAddress(ID)) return EEPROM_ReadDWord(((uint16_t)(ID*8 + FS_EEPROM_StartAddress + 32 + 4)));
	else return 0;
}

//Gibt die Addresse des ersten Bytes der Datei zurück
static uint32_t FS_GetFileStart(uint8_t ID)
{
	if(FS_CheckAddress(ID)) return EEPROM_ReadDWord((uint16_t)(ID*8 + FS_EEPROM_StartAddress + 32));
	else return 0;
}

//Überprüft ob die angegebene Speicherplatz-ID belegt oder frei ist
static uint8_t FS_CheckAddress(uint8_t ID)
{
	if(ID >FS_MaxFileCount)return(0);
	uint8_t bit = ID%8; //Bit to check
	uint8_t push=(1<<bit);
	uint8_t b = ID/8; //Block
	uint8_t Addblock = 0;
	Addblock = EEPROM_ReadByte((uint16_t)(b+FS_EEPROM_StartAddress+16));
	if(push & Addblock){ return(1); }
	else{ return(0); }
}

//Speichert die Metadaten einer Datei
static void FS_WriteFileInfo(uint8_t ID)
{
	uint16_t address = ID*8 + FS_EEPROM_StartAddress+32;
	EEPROM_WriteDWord((uint16_t)address,FS_TempFile.Start);
	EEPROM_WriteDWord((uint16_t)(address+4),FS_TempFile.Length);
	EEPROM_WriteByte((uint16_t)(address+8),FS_TempFile.ID);
	EEPROM_WriteByte((uint16_t)(address+9),FS_TempFile.Flag);
}

//Läd die Metadaten einer Datei
static uint8_t FS_LoadFileInfo(uint8_t ID)
{
	if(!FS_CheckAddress(ID))return(0);
	uint16_t address = ID*8 + FS_EEPROM_StartAddress+32;
	FS_TempFile.Start = EEPROM_ReadDWord((uint16_t)address);
	FS_TempFile.Length = EEPROM_ReadDWord((uint16_t)(address+4));
	FS_TempFile.ID = EEPROM_ReadByte((uint16_t)(address+8));
	FS_TempFile.Flag = EEPROM_ReadByte((uint16_t)(address+9));
	return(FS_TempFile.ID == ID);
}

//Updatet das Dateisystem in hinsicht auf freien speicherplatz und Anzahl an gespeicherten Dateien
//Wird bei jedem start des Systems ausgeführt
static void FS_UpdateFileSys(void)
{
	FS_CurrentStatus.FileSpaceAvailable = (FS_StopAddress_Files - FS_StartAddress_Files);
	FS_CurrentStatus.FileCount=0;
	for (uint8_t i = 1; i < FS_MaxFileCount; i++)
	{
		if(FS_CheckAddress(i))
		{
			FS_CurrentStatus.FileSpaceAvailable-= FS_GetFileLength(i);
			FS_CurrentStatus.FileCount++;
		}
	}
}

//Unused
static void FS_Reorganize(uint8_t ID)
{
	uint8_t lastID = ID;
	uint32_t lastStart;
	uint8_t Buffer[64];
	uint8_t length = 64;
	
	for (uint8_t currentID = ID+1; currentID < FS_CurrentStatus.FileCount; currentID++)
	{
		FS_LoadFileInfo(currentID);
		lastStart = FS_GetFileStart(lastID);
		do
		{
			if(length > FS_TempFile.Length - FS_TempFile.ReadPos) length = FS_TempFile.Length - FS_TempFile.ReadPos;
			
			uint32_t rpos = (FS_TempFile.Start+FS_TempFile.ReadPos);
			FlashAddress add = Flash_CreateAddress((rpos>>24) & 0xFF,(rpos>>16) & 0xFF,(rpos>>8) & 0xFF,rpos & 0xFF);
			Flash_read_Bytes(add, Buffer, length);
			
			rpos = (lastStart+FS_TempFile.ReadPos);
			add = Flash_CreateAddress((rpos>>24) & 0xFF,(rpos>>16) & 0xFF,(rpos>>8) & 0xFF,rpos & 0xFF);
			Flash_write_Bytes(add, Buffer, 0, length);
			
			FS_TempFile.ReadPos += 64;
			if(FS_TempFile.ReadPos % 4096 == 0)
			{
				rpos = (FS_TempFile.Start + 4096 * (FS_TempFile.ReadPos/4096-1));
				add = Flash_CreateAddress((rpos>>24) & 0xFF,(rpos>>16) & 0xFF,(rpos>>8) & 0xFF,rpos & 0xFF);
				Flash_sector_Erase(add);
			}
			
		} while (FS_TempFile.ReadPos != FS_TempFile.Start+FS_TempFile.Length);
		
		uint32_t spos = (FS_TempFile.Start + 4096 * (FS_TempFile.ReadPos/4096));
		FlashAddress sadd = Flash_CreateAddress((spos>>24) & 0xFF,(spos>>16) & 0xFF,(spos>>8) & 0xFF,spos & 0xFF);
		
		Flash_sector_Erase(sadd);
		FS_WriteFileInfo(lastID);
		lastID++;
	}
	FS_SetAddress(FS_CurrentStatus.FileCount,0);
}

//Setzt einen Speicherplatz als frei/belegt
static void FS_SetAddress(uint8_t ID, uint8_t state)
{
	uint8_t bit = ID%8; //Bit to check
	uint8_t b = ID/8; //Block
	uint8_t Addblock = 0;
	Addblock = EEPROM_ReadByte((uint16_t)(b+FS_EEPROM_StartAddress + 16));
	if(state){ Addblock |= (1<<bit); }
	else{ Addblock &= ~(1<<bit); }
	EEPROM_WriteByte((uint16_t)(b+FS_EEPROM_StartAddress + 16),Addblock);
}

//Reserviert Speicherplatz im Flash-Speicher für einen neue Datei
uint8_t FS_CreateNewEntry(uint32_t length)
{
	if(length>FS_CurrentStatus.FileSpaceAvailable) return(255);//If 0 create empty file!
	for(int c=1; c < FS_MaxFileCount; c++)
	{
		if(!FS_CheckAddress(c))
		{
			uint32_t start = FS_CreateFileStart();
			if(start == 0)return(255);//Error Creating start address!
			FS_TempFile.Start = start;
			FS_TempFile.Length=0;
			FS_TempFile.ID = c;
			FS_WriteFileInfo(c);
			FS_SetAddress(c,1);
			FS_CurrentStatus.FileCount++;
			return(c);
		}
	}
	return(255); //No Filespace
}

//Löscht eine Datei und deren Metadaten aus dem Flash-Speicher
void FS_RemoveEntry(uint8_t ID)
{
	if(!FS_CheckAddress(ID))return;
	FS_LoadFileInfo(ID);
	uint16_t remMax = (FS_TempFile.Length > 4096 ? (FS_TempFile.Length%4096 == 0 ? FS_TempFile.Length/4096 : FS_TempFile.Length/4096 + 1) : 1);
	
	for (uint16_t i = 0; i < remMax; i++)
	{
		FlashAddress add = Flash_CreateAddress((FS_TempFile.Start>>24) & 0xff,(FS_TempFile.Start>>16) & 0xff, (FS_TempFile.Start>>8) & 0xff, FS_TempFile.Start & 0xff);
		Flash_sector_Erase(add);
		FS_TempFile.Start += 4096;
	}
	//FS_Reorganize(ID);
	FS_UpdateFileSys();
}

//Schreibt teile der Datei in den Flash-Speicher. Voll automatisch an die richtige Addresse.
void FS_WriteFile(uint8_t ID, uint8_t *buffer,uint8_t offset,uint8_t length)
{
	if(FS_TempFile.ID!=ID) FS_LoadFileInfo(ID);//Reload FileInfo into buffer
	uint32_t add = FS_TempFile.Start + FS_TempFile.Length;
	FlashAddress addr = Flash_CreateAddress((add>>24)&0xFF,(add>>16)&0xFF,(add>>8) & 0xFF,add & 0xFF);
	Flash_write_Bytes(addr,buffer,offset,length);
	FS_TempFile.Length += length;
	FS_CurrentStatus.FileSpaceAvailable -=length;
}

//Setzt die lese position in der aktuellen Datei
void FS_SetReadPos(uint32_t position)
{
	if(position>FS_TempFile.Length)return;
	FS_TempFile.ReadPos=position;
}

//Ließt eine Datei aus dem Speicher
uint8_t FS_ReadFile(uint8_t ID,uint8_t *buffer,uint8_t length)
{
	if(FS_TempFile.ID !=ID)FS_LoadFileInfo(ID);
	if(length>FS_TempFile.Length - FS_TempFile.ReadPos) length = FS_TempFile.Length - FS_TempFile.ReadPos;
	if(length > 0)
	{
		uint32_t rpos = (FS_TempFile.ReadPos+FS_TempFile.Start);
		FlashAddress add = Flash_CreateAddress((rpos>>24) & 0xFF,(rpos>>16) & 0xFF,(rpos>>8) & 0xFF,rpos & 0xFF);
		Flash_read_Bytes(add,buffer,length);
		FS_TempFile.ReadPos += length;
	}
	return length;
}

#pragma endregion FILE SUPPORT

#pragma region RECORD SUPPORT
//Der Record-Support stellt Funktionen für die Speicherung von Messdateneinheiten bereit. Er verwaltet das Speichermanagement und beinhaltet
//suchfunktionen die es ermöglichen einfach und schnell Dateneinheiten aus dem Flash-Speicher zu lesen.

//Schreibt eine Dateneinheit mit den gemessenen Daten einer Messstationen in den Flash-Speicher
void FS_WriteRecord(FS_StationRecord_t* fs)
{
	FS_WriteRecordHW(fs);
	FS_CurrentStatus.NextAddress = FS_CreateNextAddress();
	uint32_t add = ((uint32_t)FS_CurrentStatus.NextAddress.High<<24) | ((uint32_t)FS_CurrentStatus.NextAddress.Mid<<16) | ((uint32_t)FS_CurrentStatus.NextAddress.Low<<8) | (uint32_t)FS_CurrentStatus.NextAddress.XLow;
	EEPROM_WriteDWord(FS_EEPROM_StartAddress,add);
	EEPROM_WriteDWord(FS_EEPROM_StartAddress+4,FS_CurrentStatus.RecordCount);
}

//Speichert eine Dateneinheit an den nächsten freien Speicherort
static void FS_WriteRecordHW(FS_StationRecord_t* fs)
{
	uint8_t buffer[16];
	buffer[0]=(fs->Unix>>24)& 0xff;buffer[1]=(fs->Unix>>16)& 0xff;buffer[2]= (fs->Unix>>8)& 0xff;buffer[3]=fs->Unix & 0xff;
	buffer[4]=fs->ID & 0xff;
	buffer[5]=fs->LightStrength;
	buffer[6]=fs->RainState;
	buffer[7]=fs->WindLevel;
	buffer[8]=fs->WindDirection;
	buffer[9]=(fs->Temperature>>8) & 0xff; buffer[10]=fs->Temperature & 0xff;
	buffer[11]=(fs->Humidity>>8) & 0xff; buffer[12]=fs->Humidity & 0xff;
	buffer[13]=(fs->Pressure>>8)& 0xff;buffer[14]=fs->Pressure & 0xff;
	buffer[15]=fs->Flags;
	Flash_write_Bytes(FS_CurrentStatus.NextAddress,buffer,0,16);
}

//Interne Funktion die eine Dateneinheit mit angegebenen Record-Index aus dem Flash-Speicher ließt
static void FS_ReadRecordHW(uint32_t record,FS_StationRecord_t* fs)
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
	fs->Temperature = ((uint32_t)buffer[9]<<8) |((uint32_t)buffer[10]);
	fs->Humidity = ((uint32_t)buffer[11]<<8) |((uint32_t)buffer[12]);
	fs->Pressure = ((uint32_t)buffer[13]<<8) |((uint32_t)buffer[14]);
	fs->Flags = buffer[15];
	fs->Position = record;
}

//Interne funktion die die verwaltung der Speicheraddressen im Flash-Speicher übernimmt
static FlashAddress FS_CreateNextAddress(void)
{
	uint32_t add = ((uint32_t)FS_CurrentStatus.NextAddress.High<<24) | ((uint32_t)FS_CurrentStatus.NextAddress.Mid<<16) | ((uint32_t)FS_CurrentStatus.NextAddress.Low<<8) | (uint32_t)FS_CurrentStatus.NextAddress.XLow;
	add+= 16;
	if(!FS_CurrentStatus.RecordLogFull)FS_CurrentStatus.RecordCount++;
	if(add > FS_StopAddress){add = FS_StartAddress_Records; FS_CurrentStatus.RecordLogFull=1; EEPROM_WriteByte(FS_EEPROM_StartAddress+8,1);}// REPORT ERROR -> FILE SYSTEM OVERFLOW!!! FullCircleMode activated!
	FlashAddress f = Flash_CreateAddress((add>>24) & 0xff,(add>>16) & 0xff,(add>>8) & 0xff,add & 0xff);
	return f;
}

//Sucht eine Dateneinheit mit angegebener UNIX aus dem Flash-Speicher
uint8_t FS_FindRecord(uint32_t unix,uint32_t* recordOut)
{
	uint32_t first=0;
	uint32_t last=FS_CurrentStatus.RecordCount-1;
	uint8_t found=0;
	uint32_t midpoint;
	if(!FS_CurrentStatus.RecordLogFull)
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

//Gibt für einen bestimmten Record-Index die UNIX-Time zurück 
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

//Setzt die automatische suche zurück um wieder mit der Funktion FS_GetRecords eine neue Suchanfrage zu starten
void FS_ResetRecordSearch(void)
{
	FS_CurrentStatus.LastSearchResult=-1;
}

//Voll automatische suchfunktion die Dateneinheiten nacheinander von hinten nach vorne zurück gibt
//also von n -> 0
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
}

//Unused
uint32_t FS_GetUnix(void)
{
	return 0;
}

//Setzt die aktuelle UNIX im Dateisystem sodass neue eintrage richtig gespeichert werden können
void FS_SetUnix(uint32_t unix)
{
	FS_CurrentStatus.CurrentUnix = unix;
}

//Erstellt eine neue Dateneinheit aus den einen Messwerten der Messstation
FS_StationRecord_t* FS_CreateStationRecord(uint16_t temp,uint16_t pres,uint16_t humid, uint8_t light,uint8_t rain,uint8_t windlvl,uint8_t winddir)
{
	FS_TempRecord.Temperature = temp;
	FS_TempRecord.Pressure = pres;
	FS_TempRecord.Humidity = humid;
	FS_TempRecord.LightStrength = light;
	FS_TempRecord.RainState = rain;
	FS_TempRecord.WindLevel = windlvl;
	FS_TempRecord.WindDirection = winddir;
	return &FS_TempRecord;
}

//Erstellt eine neue Dateneinheit aus einem Array mit den Messwerten der Messstation
FS_StationRecord_t* FS_CreateStationRecordArray(uint8_t* buffer)
{
	FS_TempRecord.Temperature = (buffer[1]<<8) | buffer[0];
	FS_TempRecord.Humidity = (buffer[3]<<8) | buffer[2];
	FS_TempRecord.Pressure = (buffer[5]<<8) | buffer[4];

	FS_TempRecord.LightStrength = buffer[6];
	FS_TempRecord.RainState = buffer[7];
	FS_TempRecord.WindLevel = buffer[8];
	FS_TempRecord.WindDirection = buffer[9];
	return &FS_TempRecord;
}

#pragma endregion RECORD SUPPORT