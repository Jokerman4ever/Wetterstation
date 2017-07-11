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

void clock_change_2MHZ(void)
{
	uint8_t state;
	
	irqflags_t flags = cpu_irq_save();
	osc_enable(OSC_ID_RC2MHZ);
	while(!osc_is_ready(OSC_ID_RC2MHZ));
	sysclk_set_source(SYSCLK_SRC_RC2MHZ);
	//sysclk_set_prescalers();
	osc_disable(OSC_ID_RC32MHZ);
	
	XDELAY_ISFAST = 0; //Set Delay to Slow-Mode
	
	//Update RF-Moduls-Frequency
	state = sysclk_module_is_enabled(SYSCLK_PORT_C, SYSCLK_TC1);
	sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_TC1);
	TCC1.PER = 20;	//RF-Update-Timer
	TCC1.CNT = 0;
	if(!state){ sysclk_disable_module(SYSCLK_PORT_C, SYSCLK_TC1);}	
		
	state = sysclk_module_is_enabled(SYSCLK_PORT_D, SYSCLK_SPI);
	sysclk_enable_module(SYSCLK_PORT_D, SYSCLK_SPI);
	SPID.CTRL |= SPI_PRESCALER_DIV4_gc; //RF-Modul
	if(!state){ sysclk_disable_module(SYSCLK_PORT_D, SYSCLK_SPI);}
	
	
		
	state = sysclk_module_is_enabled(SYSCLK_PORT_D, SYSCLK_SPI);
	sysclk_enable_module(SYSCLK_PORT_D, SYSCLK_SPI);
	SPIE.CTRL |= SPI_PRESCALER_DIV4_gc; //Flash
	if(!state){ sysclk_disable_module(SYSCLK_PORT_D, SYSCLK_SPI);}
			
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
	sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_TC1);
	TCC1.PER = 320;	//RF-Update-Timer
	TCC1.CNT = 0;
	if(!state){ sysclk_disable_module(SYSCLK_PORT_C, SYSCLK_TC1);}
		
	state = sysclk_module_is_enabled(SYSCLK_PORT_D, SYSCLK_SPI);
	sysclk_enable_module(SYSCLK_PORT_D, SYSCLK_SPI);
	SPID.CTRL |= SPI_PRESCALER_DIV64_gc; //RF-Modul
	if(!state){ sysclk_disable_module(SYSCLK_PORT_D, SYSCLK_SPI);}
	
	
	
	state = sysclk_module_is_enabled(SYSCLK_PORT_D, SYSCLK_SPI);
	sysclk_enable_module(SYSCLK_PORT_D, SYSCLK_SPI);
	SPIE.CTRL |= SPI_PRESCALER_DIV64_gc; //Flash
	if(!state){ sysclk_disable_module(SYSCLK_PORT_D, SYSCLK_SPI);}
	
	cpu_irq_restore(flags);
}