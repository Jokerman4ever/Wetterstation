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

uint8_t RF_Stuck;

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
	int16_t temp;
	uint16_t humidity;
	uint16_t pressure;
	
	uint8_t lux;
	uint8_t regen;
	uint8_t wind_v;
	uint8_t wind_r;
} MeassurmentData_t;


uint8_t data[12];

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
	
	
	int16_t tf;
	uint16_t bat;
	bool wind_con = true;
	
	RF_Packet_t packet;
	MeassurmentData_t meas_data;
	
		
	PORTA.OUTTGL = 0x01;
	
	//Read Sensor Data
	DHT_on(); //wait for 2s befor Read!!!
	PORTA.OUTSET = (1<<7);
	i2c_enable();
	BMP_read(&meas_data.pressure, &meas_data.temp);	//takes 4ms
	BH1750_read(&meas_data.lux);	//takes 24ms
	/* Wind-/Regensensor here */
	
	i2c_disable();

	bat = ADCA_GetValue(ADC_CH0, ADC_CH_MUXPOS_PIN0_gc); //Read Bat-Status
	
	_delay_ms(2000);
	DHT_read(&meas_data.humidity, &tf);
	DHT_off();
	PORTA.OUTCLR = (1<<7);
	
	//Simulation Wind oder so was in der Art... keine Ahnung
	if(meas_data.wind_r == 0)
	{
		meas_data.wind_v = 0x01;
		meas_data.wind_r = 0x01;
	}
	else
	{
		meas_data.wind_r <<= 1;
		meas_data.wind_v <<= 1;
	}	
	
	
	//Send Data
	RF_Wakeup();
	
	if(wind_con){ packet = RF_CreatePacket((uint8_t *)&meas_data, 10, RF_RECEIVE_ID, 0); }
	else{ packet = RF_CreatePacket((uint8_t *)&meas_data, 6, RF_RECEIVE_ID, 0); }
	RF_Send_Packet(packet);
	
	
	while(RF_CurrentStatus.State == RF_State_Transmit){_delay_ms(1);}
	while(RF_CurrentStatus.Acknowledgment != RF_Acknowledgments_State_Idle){_delay_ms(1);}
	
	RF_Sleep();
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
	
	ADC_Init();
	
	i2c_init();
	i2c_enable();
	BMP_init();
	i2c_disable();
	
	DHT_init();
	
	uint8_t buf[3] = {32,64,128};

	RF_Init(0x05);
	val = RF_Get_Command(0x01);
	RF_Set_State(RF_State_Receive);
	RF_Packet_t packet = RF_CreatePacket(buf,3,0x01,0x00);
	RF_Sleep();
	
	PMIC.CTRL = PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm; //Enable Interrupt
	
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
		sleepmgr_enter_sleep();
				
		//packet = RF_CreatePacket(data, 12, RF_RECEICE_ID, 0);	
		//RF_Send_Packet(packet);	
				
		//RF_Send_Packet(packet);
		
		//if(RF_CurrentStatus.Acknowledgment == RF_Acknowledgments_State_Idle && RF_CurrentStatus.State != RF_State_Receive)RF_Set_State(RF_State_Receive);
	
		//uint8_t rssi = RF_Get_Command(RF_REG_FTPRI);
		//rssi|=(1<<2);
		//RF_Set_Command(RF_REG_FTPRI,rssi);
		//RF_Send_Packet(packet);
		//RF_Get_Data(buffer, 128);
		
	/*	if(RF_CurrentStatus.NewPacket == 1)
		{
			packet = RF_Get_Packet();
			uint16_t light = (uint16_t)((packet.Data[0] <<8) + packet.Data[1]);
			power = (3.3/4096) * (uint16_t)((packet.Data[0] <<8) + packet.Data[1]);
			LED_Set_Num_Lin(light/16);
		}
		
*/
		
		//delay_ms(100);
	}
}
