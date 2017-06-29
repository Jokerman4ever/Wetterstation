#include <asf.h>
#include "time.h"
#include "lcd-routines.h"
#include "Xdelay.h"
#include "RF.h"

int main (void)
{
	sysclk_init();
	RF_Init(0x01);
	
	if(RF_CurrentStatus.NewPacket)
	{
		
	}
	
	
	
	
	while(1)
	{
		_xdelay_ms(1);
	}
}
