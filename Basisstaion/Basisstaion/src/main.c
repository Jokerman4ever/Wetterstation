#include <asf.h>
#include "time.h"
#include "Display/lcd-routines.h"
#include "Display/Display-Menu.h"
#include "Clock/Xdelay.h"
#include "Clock/clock.h"
#include "RF.h"
#include "Storage/FileSys.h"
#include "Storage/eeprom_driver.h"
#include "GSM/com.h"
#include "Http/server.h"
#include "time.h"
#include "ErrorList.h"
void HandleClients(void);
void CheckFirstrun(void);
//Für GSMS neu hinzugefügt
extern int8_t init_schritt;
extern int8_t alter_schritt;
extern uint8_t uart_str_count;
extern uint8_t html_code1[];
extern uint8_t html_code2[];
extern uint8_t html_code3[];
extern  char uart_string[UART_MAXSTRLEN + 1];
volatile uint8_t uart_str_complete = 1;

//extern int8_t init_schritt;
//extern  char uart_string[UART_MAXSTRLEN + 1];
//volatile uint8_t uart_str_complete = 0;
//uint8_t daten_enmpfangen=false;
uint8_t Packet_buffer[10];
uint8_t EEPROM_SyncWord;
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

ISR(TCC0_OVF_vect)
{
	DSP_Refresh(0,0,RF_CurrentStatus.Registerd_Devices);
	SystemTick();
}

ISR(PORTA_INT0_vect)
{
	if(PORTA.IN & 0x02)
	{
		DSP_ScrollMenu(0);
	}
	else
	{
		DSP_ScrollMenu(1);
	}
}

ISR(PORTA_INT1_vect)
{
	DSP_SelectMenu();
}

uint8_t cont =0;//Nur für den contrast kann nachher wieder gelöscht werden!!!!
uint8_t val;
int main(void)
{
	uint8_t buffer[3];
	sysclk_init();
	//XDELAY_ISFAST = 0;
	//clock_change_32MHZ();
	clock_change_2MHZ();
	lcd_init();
	DSP_ChangePage(PageWelcome);
	DSP_Refresh_Timer_Init();
	_delay_ms(750);
	
	PORTA.INTCTRL = PORT_INT0LVL_HI_gc | PORT_INT1LVL_HI_gc;
	PORTA.INT0MASK = (1<<2); //Drehen
	PORTA.INT1MASK = (1<<3); //Tasten
	PORTA.PIN2CTRL = PORT_ISC_FALLING_gc;
	PORTA.PIN3CTRL = PORT_ISC_FALLING_gc;
	
	
	Flash_SPI_Init();
	EEPROM_FlushBuffer();
	//EEPROM_DisableMapping();
	CheckFirstrun();
	FS_Init();
	RF_Packet_t p = RF_CreatePacket(buffer,1,16,0);//JUST FOR TEST!!!!
	RF_Init(0x01, EEPROM_SyncWord);
	val = RF_Get_Command(0x01);
	PMIC.CTRL = PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm;
	RF_Set_State(RF_State_StandBy);

	DSP_ChangePage(PageHome);

	sei();

	Set_Unix_Time(1509599593);
	
	com_init();
	//SERVER
	//Ist jetzt in der unstable branch!
	/*com_init();
	for (int8_t com_initstep = -3; com_initstep < 7;com_initstep++)
	{
		server_configuration(&com_initstep);
	}*/
int offset=0;
		server_configuration();
	while(1)
	{
		
		for(int i=0; i<UART_MAXSTRLEN; i++)
		{
			

			
			int offset;
			if(uart_string[i]=='G')
			{
					lcd_set_cursor(0,1);
					lcd_Xstring("ich kriege ein G",0);
					_delay_ms(500);
					offset=i;
					i=UART_MAXSTRLEN;
				com_send_string("AT+CIPSEND\r");				// Änderung
				_delay_ms(500);
				com_send_antwortclient(html_code1);
				com_send_antwortclient(html_code2);
				com_send_antwortclient(html_code3);
				_delay_ms(500);			
				com_ausgabe(0x1A);								// Änderung				
			}
		}
		if(com_StrCmp(uart_string,offset,3,"GET")==1)
		{   lcd_set_cursor(0,2);
			lcd_Xstring("ich kriege ein GET",0);
			_delay_ms(500);
			com_send_antwortclient(html_code1);
			com_send_antwortclient(html_code2);
			com_send_antwortclient(html_code3);
			
			for(int s= 0; s< com_strlen(uart_string);s++)
			{
				uart_string[s]=' ';
				uart_str_count=0;
			}
		}

	

		/*if(uart_str_complete==1)
		{

			uart_str_complete=0;
			if(com_StrCmp(uart_string,0,2,"GET")==1)
			{
				com_send_antwortclient();
			}
		}*/
		//AUSKOMMENTIERT
		/*RF_Send_Packet(p);
		_xdelay_ms(1000);
		*/
		if(RF_CurrentStatus.Acknowledgment == RF_Acknowledgments_State_Idle && RF_CurrentStatus.State != RF_State_Receive)RF_Set_State(RF_State_Receive);
		_xdelay_ms(5);
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
		}
	}
}

void CheckFirstrun(void)
{
	if(EEPROM_ReadByte(1)==255)
	{
		EEPROM_WriteByte(1,1);
		FS_FirstRun();
		EEPROM_WriteByte(14,0);
	}
	EEPROM_SyncWord = EEPROM_ReadByte(14);
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
					DSP_Update_Weatherdata(r);
					FS_WriteRecord(r);			
				}
			}
		}
	}
}
