#include <asf.h>
#include "time.h"
#include "lcd-routines.h"
#include "Xdelay.h"

int main (void)
{
	sysclk_init();
	
	
	Set_Unix_Time(500000000);
	
	tm_t bla;
	
	time_GetLocalTime(&bla);
	
	
	lcd_string((char *) &bla);
	
	while(1)
	{
		_xdelay_ms(1);
	}
}
