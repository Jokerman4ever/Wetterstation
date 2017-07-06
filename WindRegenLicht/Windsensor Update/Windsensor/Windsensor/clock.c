#include "clock.h"

void clock_init(void)
{
	OSC.CTRL |= OSC_RC32MEN_bm;
	while(!(OSC.STATUS & OSC_RC32MRDY_bm));	
	CCP = CCP_IOREG_gc;
	CLK.CTRL = CLK_SCLKSEL_RC32M_gc;
	CCP = CCP_IOREG_gc;
	CLK.PSCTRL = CLK_PSADIV_8_gc;
}