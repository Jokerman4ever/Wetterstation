#include <asf.h>
#include "time.h"
#include "Display/lcd-routines.h"
#include "Clock/Xdelay.h"
#include "RF.h"


void HandleClients(void);


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
	sysclk_init();
	RF_Init(0x01);
	uint8_t val = RF_Get_Command(0x01);
	PMIC.CTRL = PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm;
	sei();
	
	
	
	
	while(1)
	{
		_xdelay_ms(1);
		HandleClients();
	}
}


uint8_t Packet_buffer[20];

void HandleClients(void)
{
	if(RF_CurrentStatus.NewPacket)
	{
		RF_Packet_t p = RF_Get_Packet();
		if(RF_FindDevice(p.ID))
		{
			
		}
		else
		{
			
			 RF_RegisterDevice(p.ID);
			 uint16_t sleep = RF_GetDeviceSleepTime(p.ID);
			 Packet_buffer[0] = (sleep<<8) & 0xff;
			 Packet_buffer[1] = sleep & 0xff;
			 p = RF_CreatePacket(Packet_buffer,2,p.Sender,RF_Packet_Flags_Time);
			 RF_Send_Packet(p);
		}
	}
}
