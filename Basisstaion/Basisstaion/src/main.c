#include <asf.h>
#include "time.h"
#include "Display/lcd-routines.h"
#include "Clock/Xdelay.h"
#include "Clock/clock.h"
#include "RF.h"
#include "Storage/FileSys.h"
#include "Storage/eeprom_driver.h"
#include "GSM/com.h"

void HandleClients(void);
void CheckFirstrun(void);

volatile uint8_t uart_str_complete = 0;
uint8_t daten_enmpfangen=false;

ISR(PORTE_INT0_vect)
{
	RF_HandleInterrupt();
}

ISR(TCC1_OVF_vect)
{
	RF_Update();
}

int main (void)
{
	uint8_t buffer[3];
	sysclk_init();
	clock_change_2MHZ();
	Flash_SPI_Init();
	EEPROM_FlushBuffer();
	EEPROM_DisableMapping();
	CheckFirstrun();
	FS_Init();
	PORTF.DIR = (1<<4);//JUST FOR TEST!!!!
	PORTF.OUTCLR = (1<<4);//JUST FOR TEST!!!!
	RF_Packet_t p = RF_CreatePacket(buffer,1,0x09,0);//JUST FOR TEST!!!!
	RF_Init(0x01);
	uint8_t val = RF_Get_Command(0x01);
	PMIC.CTRL = PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm;
	RF_Set_State(RF_State_Receive);
	sei();	
	
	//SERVER
	//com_init();
	//server_configuration();
	
	//------
	
	

	while(1)
	{
		if(RF_CurrentStatus.Acknowledgment == RF_Acknowledgments_State_Idle && RF_CurrentStatus.State != RF_State_Receive)RF_Set_State(RF_State_Receive);
		_xdelay_ms(10);
		HandleClients();
		
		//Brauchen wir das jetzt noch???
		if(uart_str_complete==1)
		{
			uart_str_complete=0;
		}

		
	}
}

void CheckFirstrun(void)
{
	if(EEPROM_ReadByte(1)==255)
	{
		EEPROM_WriteByte(1,1);
		FS_FirstRun();
	}
}


uint8_t Packet_buffer[6];
void HandleClients(void)
{
	if(RF_CurrentStatus.NewPacket)
	{
		RF_Packet_t p = RF_Get_Packet();
		if(p.Flags & RF_Packet_Flags_Time)
		{
			uint8_t index = RF_FindDevice(p.Sender);
			//wenn index == 0 ->gerät war nicht eingetragen!!!
			if(index>0)RF_UnregisterDevice(index);
			uint8_t id = RF_RegisterDevice(p.Sender);
			if(id > 0)
			{
				uint16_t sleep = RF_GetDeviceSleepTime(id);
				Packet_buffer[0] = (sleep>>8) & 0xff;
				Packet_buffer[1] = sleep & 0xff;
				p = RF_CreatePacket(Packet_buffer,2,p.Sender,RF_Packet_Flags_Time);
				_xdelay_ms(10);//wait till the Messstation is in Receive mode!
				RF_Send_Packet(p);
				while(RF_CurrentStatus.State == RF_State_Transmit){_delay_ms(1);}
				while(RF_CurrentStatus.Acknowledgment != RF_Acknowledgments_State_Idle){_delay_ms(1);}
				if(RF_CurrentStatus.AckResult == RF_Acknowledgments_Result_ERROR)
				{
					RF_UnregisterDevice(p.Sender);
				}
			}
			//Else -> ERROR keine timeslots mehr frei!
		}
		else
		{
			if(RF_FindDevice(p.Sender) > 0)
			{
				PORTF.OUTSET = (1<<4);//JUST FOR TEST!!!!
				_delay_ms(500);//JUST FOR TEST!!!!
				PORTF.OUTCLR = (1<<4);//JUST FOR TEST!!!!
				
				if(p.Flags & RF_Packet_Flags_Weather)
				{
					FS_StationRecord_t* r = FS_CreateStationRecordArray(p.Data);
					FS_WriteRecord(r);
				}
			}
		}
	}
}
