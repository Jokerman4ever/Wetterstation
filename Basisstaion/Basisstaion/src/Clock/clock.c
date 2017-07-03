/*
 * clock.c
 *
 * Created: 29.06.2017 11:36:49
 *  Author: Stud
 */ 

#include "clock.h"
#include "Xdelay.h"
#include <avr/interrupt.h>
#include <ASF/common/services/clock/sysclk.h>
#include <stdbool.h>

void clock_change_2MHZ(void)
{
	bool state;
	
	irqflags_t flags = cpu_irq_save();
	osc_enable(OSC_ID_RC2MHZ);
	while(!osc_is_ready(OSC_ID_RC2MHZ));
	sysclk_set_source(SYSCLK_SRC_RC2MHZ);
	//sysclk_set_prescalers();
	osc_disable(OSC_ID_RC32MHZ);
	
	XDELAY_ISFAST = 0; //Set Delay to Slow-Mode
	
	//Update RF-Moduls-Frequency
	state = sysclk_module_is_enabled(SYSCLK_PORT_C, SYSCLK_TC1);
	sysclk_disable_module(SYSCLK_PORT_C, SYSCLK_TC1);
	TCC1.PER = 2;	//RF-Update-Timer
	TCC1.CNT = 0;
	SPID.CTRL |= SPI_PRESCALER_DIV4_gc; 
	if(state){ sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_TC1);}
		
	SPIE.CTRL |= SPI_PRESCALER_DIV4_gc;	//Flash
			
	cpu_irq_restore(flags);
}

void clock_change_32MHZ(void)
{
	bool state;
	
	irqflags_t flags = cpu_irq_save();
	osc_enable(OSC_ID_RC32MHZ);
	while(!osc_is_ready(OSC_ID_RC32MHZ));
	sysclk_set_source(SYSCLK_SRC_RC32MHZ);
	//sysclk_set_prescalers();
	osc_disable(OSC_ID_RC2MHZ);
	
	XDELAY_ISFAST = 1;	//Set Delay to Fast-Mode
	
	//Update RF-Moduls-Frequency
	state = sysclk_module_is_enabled(SYSCLK_PORT_C, SYSCLK_TC1);
	sysclk_disable_module(SYSCLK_PORT_C, SYSCLK_TC1);
	TCC1.PER = 32;	//RF-Update-Timer
	TCC1.CNT = 0;
	SPID.CTRL |= SPI_PRESCALER_DIV64_gc;
	if(state){ sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_TC1);}
		
	SPIE.CTRL |= SPI_PRESCALER_DIV64_gc; //Flash
	
	cpu_irq_restore(flags);
}