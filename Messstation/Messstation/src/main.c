#include <asf.h>
#include <util/delay.h>
#include "i2c.h"
#include "Sensoren/DHT.h"
#include "Sensoren/Bmp.h"
#include "Sensoren/BH1750.h"
#include "RF.h"
#include "LED.h"
#include "ADC.h"
#include <avr/interrupt.h>
#include "com.h"
#include "Sensoren/BME280.h"

//Prototypen
static void ConnectToBasestation(uint32_t time);
static void alarm(uint32_t time);


#define TEST
//#define NOLOGON


#define  RF_RECEIVE_ID 1
#define SLEEPTIME 5	//Wake up every x seconds
#define SLEEPCOUNT ((SLEEPTIME / 1.1) - 1) //Calculate Value for RTC

ISR(PORTE_INT0_vect)
{
	RF_HandleInterrupt();
}

ISR(TCC1_OVF_vect)
{
	RF_Update();
}

typedef struct MeassurmentData_t
{
	int16_t temperature;
	uint16_t humidity;
	uint16_t pressure;
	
	uint8_t lux;
	uint8_t rainstate;
	uint8_t windlevel;
	uint8_t winddirection;
} MeassurmentData_t;


uint8_t id, syncw_num;

//One Time Step -> 1.1s
//Addiert eins für den nächsten Wake-Up
static void alarm(uint32_t time)
{
	/* Since the current time will give alarm when rolling over to
	 * next time unit, we just call with that one.
	 * This is safe to here since it's called from a time unit roll
	 * over.
	 */
	rtc_set_alarm(time + SLEEPCOUNT);
	
	uint16_t bat;
	bool wind_con;
	
	RF_Packet_t packet;
	MeassurmentData_t meas_data;
	
	PMIC.CTRL = PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm;
		
	PORTA.OUTTGL = 0x01;
	
	//Read Sensor Data
	i2c_enable();
	BME280_read(&meas_data.pressure, &meas_data.temperature, &meas_data.humidity);
	i2c_disable();
	
	com_enable();
	/* Wind-/Regensensor here */
	com_putc(0xFF);
	wind_con = com_getc_timeout(&meas_data.windlevel);
	if(wind_con) 
	{
		com_getc_timeout(&meas_data.lux);
		com_getc_timeout(&meas_data.rainstate);
		meas_data.rainstate = 255 - meas_data.rainstate;
	}
	com_disable();
	
	bat = ADCA_GetValue(ADC_CH0, ADC_CH_MUXPOS_PIN2_gc); //Batteriestatus lesen...
		
	//Send Data
	RF_Wakeup();
	
	if((id != ((PORTA.IN & 0xF0) >> 4)) || (syncw_num != (PORTB.IN & 0x0F)))
	{
		id = (PORTA.IN & 0xF0) >> 4;
		syncw_num = PORTB.IN & 0x0F;
		
		RF_Set_Address(16+id);//Set Device local address!
		RF_CurrentStatus.LocalDeviceAdd = 16+id;
		
		RF_Set_Sync_Num(syncw_num);
	}
	
	if(wind_con){ packet = RF_CreatePacket((uint8_t *)&meas_data, 10, RF_RECEIVE_ID, RF_Packet_Flags_Weather | RF_Packet_Flags_AllSensors); }
	else{ packet = RF_CreatePacket((uint8_t *)&meas_data, 6, RF_RECEIVE_ID, RF_Packet_Flags_Weather); }
	RF_Send_Packet(packet);
	
	
	while(RF_CurrentStatus.State == RF_State_Transmit){_delay_ms(1);}
	while(RF_CurrentStatus.Acknowledgment != RF_Acknowledgments_State_Idle){_delay_ms(1);}
	
	#ifndef NOLOGON
	RF_Set_State(RF_State_Receive);
	_delay_ms(50);
	
	//Check for new Timeslot
	if(RF_CurrentStatus.NewPacket) 
	{
		RF_Packet_t r = RF_Get_Packet();
		if(r.Flags & RF_Packet_Flags_Time)
		{
			uint16_t sleep = (r.Data[0]<<8 | r.Data[1]);
			uint8_t ms_sleep = (sleep % 100) - 10;
			
			//Sleep to new Timeslot
			for (uint16_t i = 0; i < ms_sleep; i++)	{_delay_ms(10); }
			#ifdef TEST
			rtc_set_alarm_relative(sleep / 1100);
			#endif
			#ifndef TEST
			rtc_set_alarm_relative(sleep/ 110);
			#endif
		}
	}
	
	if(RF_CurrentStatus.PacketsLost > 3)
	{
		rtc_set_callback(ConnectToBasestation);
	}
	#endif
	
	RF_Sleep();
	PMIC.CTRL = PMIC_LOLVLEN_bm; //Only RTC IRQ
}

int main (void)
{
	sysclk_init();
/*	osc_enable(OSC_ID_RC32MHZ);
	while(!osc_is_ready(OSC_ID_RC32MHZ));
	sysclk_set_source(SYSCLK_SRC_RC32MHZ);
	//sysclk_set_prescalers();
	osc_disable(OSC_ID_RC2MHZ);
*/
	
	_delay_ms(1000);

//Initialisiere Interfaces	
	ADC_Init();
	i2c_init();
	com_init();

//Initialisiere Sensoren	
	i2c_enable();
	BME280_init();
	i2c_disable();
	
//Initialisiere RF-Modul
	
	PORTA.DIRCLR = 0xF0;
	PORTA.PIN4CTRL |= PORT_OPC_PULLUP_gc;
	PORTA.PIN5CTRL |= PORT_OPC_PULLUP_gc;
	PORTA.PIN6CTRL |= PORT_OPC_PULLUP_gc;
	PORTA.PIN7CTRL |= PORT_OPC_PULLUP_gc;
	
	PORTB.DIRCLR = 0x0F;
	PORTB.PIN0CTRL |= PORT_OPC_PULLUP_gc;
	PORTB.PIN1CTRL |= PORT_OPC_PULLUP_gc;
	PORTB.PIN2CTRL |= PORT_OPC_PULLUP_gc;
	PORTB.PIN3CTRL |= PORT_OPC_PULLUP_gc;
	
	id = (PORTA.IN & 0xF0) >> 4;
	syncw_num = PORTB.IN & 0x0F;

	RF_Init(16+id, syncw_num);
	#ifdef TEST
		uint8_t val = RF_Get_Command(0x01);
	#endif	
	
	//RF_Set_State(RF_State_Receive);
	RF_Sleep();
	
	PMIC.CTRL = PMIC_LOLVLEN_bm; //Enable Interrupt for RTC
	
//Initialisiere RTC & Sleepmanager
	sleepmgr_init();
	rtc_init();
	
	#ifdef NOLOGON
		rtc_set_callback(alarm);
	#endif
	#ifndef NOLOGON
		rtc_set_callback(ConnectToBasestation);  //LOG INTO BASE
	#endif
	cpu_irq_enable();

	/* We just initialized the counter so an alarm should trigger on next
	 * time unit roll over.
	 */
	rtc_set_alarm_relative(0);
	
	while (1)
	{
		sleepmgr_enter_sleep();	//Go to sleep
	}
}


static void ConnectToBasestation(uint32_t time)
{
	uint8_t trys = 10;
	uint8_t NPtrys = 10;
	
	
	PMIC.CTRL = PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm;
	RF_Wakeup();
	
	if((id != ((PORTA.IN & 0xF0) >> 4)) || (syncw_num != (PORTB.IN & 0x0F)))
	{
		id = (PORTA.IN & 0xF0) >> 4;
		syncw_num = PORTB.IN & 0x0F;
		
		RF_Set_Address(16+id);//Set Device local address!
		RF_CurrentStatus.LocalDeviceAdd = 16+id;
		
		RF_Set_Sync_Num(syncw_num);
	}
	
	RF_Packet_t p = RF_CreatePacket(&trys,0,RF_RECEIVE_ID,RF_Packet_Flags_Time);
	while(trys-- > 0)
	{
		NPtrys = 10;
		p = RF_CreatePacket(&trys,0,RF_RECEIVE_ID,RF_Packet_Flags_Time);
		RF_Send_Packet(p);
		while(RF_CurrentStatus.State == RF_State_Transmit){_delay_ms(1);}
		while(RF_CurrentStatus.Acknowledgment != RF_Acknowledgments_State_Idle){_delay_ms(1);}
		RF_Set_State(RF_State_Receive);//Wait for Time Packet
		while(!RF_CurrentStatus.NewPacket && NPtrys-- > 0){_delay_ms(50);}
		if(NPtrys > 0 && RF_CurrentStatus.NewPacket)
		{
			//Time Packet Received
			RF_Packet_t r = RF_Get_Packet();
			if(r.Flags & RF_Packet_Flags_Time)
			{
				uint16_t sleep = (r.Data[0]<<8 | r.Data[1]);
				
				RF_Sleep();
				PMIC.CTRL = PMIC_LOLVLEN_bm; //Enable Interrupt for RTC
				
				for (uint16_t i = 0; i < sleep % 100; i++)	{ _delay_ms(10); }
				
				rtc_set_callback(alarm);
				
				#ifdef TEST
					rtc_set_alarm_relative(sleep / 1100);
				#endif
				#ifndef TEST
					rtc_set_alarm_relative(sleep/ 110);
				#endif
				
				return;
			}
		}
	}
	
	RF_Sleep();
	PMIC.CTRL = PMIC_LOLVLEN_bm; //Enable Interrupt for RTC
	rtc_set_alarm_relative(SLEEPCOUNT); //Retry after 5min
}
