#include <asf.h>
#include "time.h"
#include "Display/lcd-routines.h"
#include "Clock/Xdelay.h"
#include "Clock/clock.h"
#include "RF.h"
#include "Storage/FileSys.h"
#include "Storage/eeprom_driver.h"
#include "GSM/com.h"
#include "Http/server.h"
void HandleClients(void);
void CheckFirstrun(void);

//extern int8_t init_schritt;
//extern  char uart_string[UART_MAXSTRLEN + 1];
//volatile uint8_t uart_str_complete = 0;
//uint8_t daten_enmpfangen=false;
uint8_t Packet_buffer[10];

//int8_t com_initstep = -3;

ISR(PORTE_INT0_vect)
{
	RF_HandleInterrupt();
}

ISR(TCC1_OVF_vect)
{
	RF_Update();
	FS_Update();
}
uint8_t val;
int main(void)
{
	uint8_t buffer[3];
	sysclk_init();
	clock_change_2MHZ();
	//Flash_SPI_Init();
	//EEPROM_FlushBuffer();
	//EEPROM_DisableMapping();
	CheckFirstrun();
	//FS_Init();
	RF_Packet_t p = RF_CreatePacket(buffer,1,16,0);//JUST FOR TEST!!!!
	RF_Init(0x01, 0);
	val = RF_Get_Command(0x01);
	PMIC.CTRL = PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm;
	RF_Set_State(RF_State_StandBy);

	sei();
	
	//com_init();
	//SERVER
	//Ist jetzt in der unstable branch!
	/*com_init();
	for (int8_t com_initstep = -3; com_initstep < 7;com_initstep++)
	{
		server_configuration(&com_initstep);
	}*/
	

	while(1)
	{

		/*if(uart_str_complete==1)
		{

			uart_str_complete=0;
			if(com_StrCmp(uart_string,0,2,"GET")==1)
			{
				com_send_antwortclient();
			}
		}*/
		//AUSKOMMENTIERT
		RF_Send_Packet(p);
		_xdelay_ms(1000);
		
		/*if(RF_CurrentStatus.Acknowledgment == RF_Acknowledgments_State_Idle && RF_CurrentStatus.State != RF_State_Receive)RF_Set_State(RF_State_Receive);
		_xdelay_us(500);
		HandleClients();	
		if(com_hasData())
		{
			uint8_t len = com_getString(Packet_buffer);
			if(len>1)
			{
				if(com_StrCmp(Packet_buffer,0,10,"TIME"))
				{
					uint32_t time = ((uint32_t)Packet_buffer[4]<<24)|((uint32_t)Packet_buffer[5]<<16)|((uint32_t)Packet_buffer[6]<<8)|(uint32_t)Packet_buffer[7];
					FS_SetUnix(time);
					com_send_string("OK");
				}
				if(com_StrCmp(Packet_buffer,0,10,"RST"))
				{
					FS_FirstRun();
					FS_Init();
					com_send_string("OK");
				}
			}
		}*/
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



void HandleClients(void)
{
	if(RF_CurrentStatus.NewPacket)
	{
		RF_Packet_t p = RF_Get_Packet();
		if(p.Flags & RF_Packet_Flags_Time || RF_FindDevice(p.Sender) == 0)
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
				if(p.Flags & RF_Packet_Flags_Weather)
				{
					//Nur zur darstellung auf dem PC mittels usart interface des GSM-Moduls
					/*
					com_ausgabe(p.Sender);
					com_ausgabe(p.ID);
					for (uint8_t i = 0; i < 10; i++)
					{
						com_ausgabe(p.Data[i]);
					}*/
					
					FS_StationRecord_t* r = FS_CreateStationRecordArray(p.Data);
					r->Unix = FS_CurrentStatus.CurrentUnix;
					r->ID = p.Sender;
					FS_WriteRecord(r);
				}
			}
		}
	}
}
