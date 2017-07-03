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
#include "com.h"11

//Prototypen
void ConnectToBasestation(void);

#define TEST

#define  RF_RECEIVE_ID 1
#define SLEEPTIME 10	//Wake up every x seconds
#define SLEEPCOUNT ((SLEEPTIME / 1.1) - 1) //Calculate Value for RTC

ISR(PORTE_INT0_vect)
{
	RF_HandleInterrupt();
}

float power;
uint8_t val;

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


uint8_t data[12];

//One Time Step -> 1.1s
//Addiert eins f�r den n�chsten Wake-Up
static void alarm(uint32_t time)
{
	/* Since the current time will give alarm when rolling over to
	 * next time unit, we just call with that one.
	 * This is safe to here since it's called from a time unit roll
	 * over.
	 */
	rtc_set_alarm(time + SLEEPCOUNT);
	
	
	int16_t tf;
	uint16_t bat;
	bool wind_con = true;
	
	RF_Packet_t packet;
	MeassurmentData_t meas_data;
	
	PMIC.CTRL = PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm;
		
	PORTA.OUTTGL = 0x01;
	
	//Read Sensor Data
	DHT_on(); //wait for 2s befor Read!!!
	PORTA.OUTSET = 0xff;
	i2c_enable();
	BMP_read(&meas_data.pressure, &meas_data.temperature);	//takes 4ms
	BH1750_read(&meas_data.lux);	//takes 24ms
	/* Wind-/Regensensor here */
	
	i2c_disable();

	bat = ADCA_GetValue(ADC_CH0, ADC_CH_MUXPOS_PIN0_gc); //Read Bat-Status
	
	_delay_ms(2000);
	DHT_read(&meas_data.humidity, &tf);
	DHT_off();
	PORTA.OUTCLR = 0xff;
	
	//Simulation Wind oder so was in der Art... keine Ahnung
	if(meas_data.winddirection == 0)
	{
		meas_data.windlevel = 0x01;
		meas_data.winddirection = 0x01;
	}
	else
	{
		meas_data.winddirection <<= 1;
		meas_data.windlevel <<= 1;
	}	
	
	
	//Send Data
	RF_Wakeup();
	
	if(wind_con){ packet = RF_CreatePacket((uint8_t *)&meas_data, 10, RF_RECEIVE_ID, RF_Packet_Flags_Weather); }
	else{ packet = RF_CreatePacket((uint8_t *)&meas_data, 6, RF_RECEIVE_ID, RF_Packet_Flags_Weather); }
	RF_Send_Packet(packet);
	
	
	while(RF_CurrentStatus.State == RF_State_Transmit){_delay_ms(1);}
	while(RF_CurrentStatus.Acknowledgment != RF_Acknowledgments_State_Idle){_delay_ms(1);}
	
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
	
	PORTA.DIRSET = 0xff;
	_delay_ms(1000);

//Initialisiere Interfaces	
	ADC_Init();
	i2c_init();
	com_init();

//Initialisiere Sensoren	
	i2c_enable();
	BMP_init();
	i2c_disable();
	DHT_init();
	
//Initialisiere RF-Modul
	RF_Init(0x05);
	#ifdef TEST
		val = RF_Get_Command(0x01);
	#endif	
	
	//////////////////////////////////////////////////////////////////////////
	//LOG INTO BASE
	//////////////////////////////////////////////////////////////////////////
	ConnectToBasestation();
	
	//RF_Set_State(RF_State_Receive);
	RF_Sleep();
	
	PMIC.CTRL = PMIC_LOLVLEN_bm; //Enable Interrupt for RTC
	
//Initialisiere RTC & Sleepmanager
	sleepmgr_init();
	rtc_init();
	rtc_set_callback(alarm);
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


void ConnectToBasestation(void)
{
	uint8_t trys = 10;
	uint8_t NPtrys = 10;
	RF_Packet_t p = RF_CreatePacket(&trys,0,RF_RECEIVE_ID,RF_Packet_Flags_Time);
	while(trys-- > 0)
	{
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
				//TIM HIER MUSSTE SLEEPEN!!!!!
				break;
			}
		}
	}
	//SYNC FAILED.... Was Nu?
}
