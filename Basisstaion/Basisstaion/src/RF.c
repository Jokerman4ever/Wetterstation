/*
 * RF.c
 *
 * Created: 31.05.2017 10:41:35
 *  Author: Felix Mälk
 */ 
#include "RF.h"
#include <ASF/common/services/clock/sysclk.h>
static void RF_Send_DataHW(uint8_t data);
static uint8_t RF_Get_DataHW(void);

RF_Packet_t RF_CurrentPacket;//Used to create a packet with the RF_CreatePacket function
RF_Packet_t RF_LastReceivedPacket;//Stores the last received packet
RF_Status_t RF_CurrentStatus;//Stores the information about the RF-Modul
RF_Config_t RF_CurrentConfig;//Can be used to configure the RF-Modul

//List of RF-Modul Syncword network identifiers
uint8_t RF_Syncwords[16][4] = {{'S','Y','N','C'},
{'L','I','N','K'},
{'U','S','E','R'},
{'H','O','S','T'},
{'D','A','T','A'},
{'S','A','F','E'},
{'C','H','I','P'},
{'C','A','L','L'},
{'G','A','T','E'},
{'I','C','O','N'},
{'F','U','N','K'},
{'W','I','N','D'},
{'R','A','I','N'},
{'T','E','M','P'},
{'P','O','R','T'},
{'V','O','L','T'}};

//Puts one char onto the SPI bus
static void SPI_putc(uint8_t data)
{
	RF_SPI_REG.DATA = data;
	while(!(RF_SPI_REG.STATUS & SPI_IF_bm));
}

//Gets one char from the SPI bus
static uint8_t SPI_getc(void)
{
	RF_SPI_REG.DATA = 0x00; //Dummy Byte
	while(!(RF_SPI_REG.STATUS & SPI_IF_bm));//Wait
	return RF_SPI_REG.DATA;//return data
}

//Configures the SPI interface for the RF-Modul
static void SPI_Init(void)
{
	sysclk_enable_module(SYSCLK_PORT_D, SYSCLK_SPI);
	
	//MOSI MISO SCK
	//5     6    7
	RF_SPI_PORT.DIRSET = (1<<5);//MOSI

	RF_SPI_PORT.DIRSET = (1<<7);//SCK

	RF_SPI_PORT.PIN4CTRL |= PORT_OPC_PULLUP_gc; //CS?
	RF_SPI_REG.CTRL = SPI_ENABLE_bm | SPI_MASTER_bm | SPI_PRESCALER_DIV4_gc | SPI_MODE_0_gc;
	if(XDELAY_ISFAST) {RF_SPI_REG.CTRL |= SPI_PRESCALER_DIV64_gc;}
	//sysclk_disable_module(SYSCLK_PORT_C, SYSCLK_SPI);

	RF_CS_COM_PORT.DIRSET = (1<<RF_CS_COM_PIN);
	RF_CS_DATA_PORT.DIRSET = (1<<RF_CS_DATA_PIN);
	RF_CS_COM_HIGH();
	RF_CS_DATA_HIGH();
	RF_SPI_PORT.OUTSET = (1<<7);
	RF_SPI_PORT.OUTSET = (1<<5);
	//PORTC.OUT |= (1<<3) | (1<<4)|(1<<5)|(1<<7);//CS auf High
}

//Configures and enables the Update timer for the RF-Modul
//This timer will tick every 10ms
static void Update_Timer_Init(void)
{
	sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_TC1); //TC1 SysClock Enable
	TCC1.CTRLA = TC_CLKSEL_DIV1024_gc; //Presackler
	TCC1.CTRLB = TC_WGMODE_NORMAL_gc;
	TCC1.PER = XDELAY_ISFAST ? 320 : 20; //Zähler Top-Wert
	TCC1.CNT = 0x00; //Reset Zähler-Wert
	TCC1.INTCTRLA = TC_OVFINTLVL_MED_gc;
}

void RF_Init(uint8_t dev_add, uint8_t syncw_num)
{
	
	RF_Reset_PORT.DIRSET = (1<<RF_Reset_Pin);
	RF_Reset_PORT.OUTSET = (1<<RF_Reset_Pin);
	_xdelay_ms(5);//Wait
	RF_Reset_PORT.OUTCLR = (1<<RF_Reset_Pin);
	_xdelay_ms(200);//Wait to respond
	
	Update_Timer_Init();
	SPI_Init();
	RF_Set_Modulation(RF_Modulation_FSK);
	RF_Set_Band(RF_Band_High);
	RF_Set_State(RF_State_StandBy);
	RF_Set_FIFOSize(RF_FIFOSize_F64Byte);
	RF_Set_Address(dev_add);//Set Device local address!
	RF_CurrentStatus.LocalDeviceAdd = dev_add;
	RF_Set_ClockOutput(0);
	RF_Set_Mode(RF_Mode_Packet);//Enable Packetmode
	RF_Set_PacketConfig(1,1,1,RF_Preamble_3,RF_AddressFilter_NODEADD_00);//Configure packetmode to CRC check|Whitening|Variable packet payload|3 Bytes Preamble|Adress Filter->Node Adress and Broadcast!
	RF_Set_PayloadLenght(RF_Max_PacketPayload);//Bytes!
	RF_Set_Sync_Num(syncw_num);
	RF_Set_IRQSources(RF_RXIRQ0_Packet_FIFOEMPTY, RF_RXIRQ1_Packet_CRCOK,RF_TXIRQ1_TXDONE);
	RF_IRQ0_PORT.DIRCLR = (1<<RF_IRQ0_PIN);//Set as Input
	RF_IRQ1_PORT.DIRCLR = (1<<RF_IRQ1_PIN);//Set as Input
	RF_CurrentConfig.RejectWrongAddress = 0;//If this is set to 1, a Packet with a wrong receiver address will be discarded! Depricated!!!
	RF_CurrentConfig.UseAcknowledgments = 1;//If this is set to 1, the protocol will use Acks
	RF_CurrentStatus.NewPacket = 0;
	RF_CurrentStatus.Acknowledgment = RF_Acknowledgments_State_Idle;
	RF_CurrentStatus.AckResult = RF_Acknowledgments_Result_Idle;
#pragma region Interrupt Handling
	//RF_IRQ0_PORT.INT0MASK = (1<<RF_IRQ0_PIN);
	//RF_IRQ0_PORT.PIN2CTRL = (PORT_ISC_RISING_gc);
	//RF_IRQ0_PORT.INTCTRL |= (PORT_INT0LVL_HI_gc);
	RF_IRQ1_PORT.INT0MASK = (1<<RF_IRQ1_PIN);
	RF_IRQ1_PORT.PIN0CTRL = (PORT_ISC_RISING_gc);
	RF_IRQ1_PORT.INTCTRL |= (PORT_INT0LVL_HI_gc);
#pragma endregion Interrupt Handling

	RF_Set_TXPower(RF_TX_Power_13DB);//Set Transmitpower
	
	RF_Set_Command(RF_REG_RSTHI,128);//Signal Level interrupt threshold -> this is used for RSSI Interrupt
	RF_Set_Frequency(868.4);//Set frequency for Europa!
	RF_Set_Modem();
	
	//for (uint8_t i=0; RF_Get_Command(RF_REG_FTXRXI) & RF_FLAG_FIFOEMPTY; i++)RF_Get_DataHW();
	//Default Settings:
	//FIFO: 16Byte
	//STATE: Standby
	//Band: 915-928MHz
	//Gain: MAX G0db
	//Modulation: FSK
}

//Used to set the state of the modul
void RF_Set_State(RF_State_t state)
{
	uint8_t lastv = RF_Get_Command(RF_REG_GCON);
	lastv = (lastv & ~RF_State_MASK) |(state & RF_State_MASK);
	RF_Set_Command(RF_REG_GCON,lastv);
	RF_CurrentStatus.State = state;
}

//With this function the RF-Modul can be put to sleep
//The update timer will be disabled aswell as the SPI interface
void RF_Sleep(void)
{
	TCC1.CTRLA = TC_CLKSEL_OFF_gc;
	TCC1.CTRLFSET = TC_CMD_RESET_gc;
	sysclk_disable_module(SYSCLK_PORT_C, SYSCLK_TC1); //Disable Update-Timer
	
	RF_Set_State(RF_State_Sleep);
	RF_CS_COM_LOW();
	RF_CS_DATA_HIGH();

	sysclk_disable_module(SYSCLK_PORT_D, SYSCLK_SPI); //Disable SPI
	
	RF_SPI_PORT.OUTSET = (1<<6);
	RF_SPI_PORT.OUTCLR = (1<<7);	
}

//With this function the RF-Modul can be woken up to normal operation
void RF_Wakeup(void)
{
	//sysclk_enable_module(SYSCLK_PORT_D, SYSCLK_SPI); //Enable SPI
	SPI_Init(); //Enable SPI

	RF_Set_State(RF_State_Receive);
	_xdelay_ms(10);
	RF_Set_State(RF_State_StandBy);
	_xdelay_ms(10);
	
	Update_Timer_Init();
	sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_TC1); //Enable Update-Timer
	TCC1.CTRLA |= TC_CLKSEL_DIV1024_gc;
}

void RF_Set_Modem(void)
{
	//Sets bitrate and Frequency deviation
	RF_Set_Command(RF_REG_FDEV,0x09);//FDVAL
	RF_Set_Command(RF_REG_BRS, 0x09);
	RF_Set_Command(RF_REG_FILC,0x70|0x02);
	
	uint8_t lastv = RF_Get_Command(RF_REG_TXCON);
	lastv = (lastv & ~0xf0) |(0x20 & 0xf0);
	RF_Set_Command(RF_REG_TXCON,lastv);
}

//Used to configure the desired band
void RF_Set_Band(RF_Band_t band)
{
	uint8_t lastv = RF_Get_Command(RF_REG_GCON);
	lastv = (lastv & ~RF_Band_MASK) |(band & RF_Band_MASK);
	RF_Set_Command(RF_REG_GCON,lastv);
}

//Used to configure the desired Modulation
//The only tested configuration is FSK!!!
void RF_Set_Modulation(RF_Modulation_t mod)
{
	uint8_t lastv = RF_Get_Command(RF_REG_DMOD);
	lastv = (lastv & ~RF_Modulation_MASK) |(mod & RF_Modulation_MASK);
	RF_Set_Command(RF_REG_DMOD,lastv);
}

//Used to configure the internal FIFO of the RF-Modul
//Maximum size is 64Bytes
void RF_Set_FIFOSize(RF_FIFOSize_t fs)
{
	uint8_t lastv = RF_Get_Command(RF_REG_FIFOC);
	lastv = (lastv & ~RF_FIFOSize_MASK) |(fs & RF_FIFOSize_MASK);
	RF_Set_Command(RF_REG_FIFOC,lastv);
}

//Used to configure the general mode the RF-Modul is working in
//Either this is Packet mode, whereas the modul only transmits data in packets
//or in continuous mode were a constant data stream is send
//or in buffered mode were data is send in as a stream with a defined size
void RF_Set_Mode(RF_Mode_t mode)
{
	uint8_t lastv = RF_Get_Command(RF_REG_DMOD);
	switch(mode)
	{
		case RF_Mode_Packet: lastv |= (1<<2); lastv &= ~(1<<5); break;
		case RF_Mode_Buffered: lastv |= (1<<5); lastv &= ~(1<<2); break;
		case RF_Mode_Continous: lastv &= ~((1<<5) | (1<<2)); break;
		default: break;
	}
	RF_Set_Command(RF_REG_DMOD,lastv);
}

//Used to configure the RF-Modul in packetmode
//useVarLength can be set to 1 if the length of the packet should be variable
//useCRC determines if the RF-Modul should automaticaly use and check CRC of the packet
//useWhite can be set if data Whitening is prefered
//preamble set the length of the preamble from 1 - 4 bytes
//addfil is the address filter. It can be configured as ONLY NODEADDRESS or NODEADDRES AND BROADCAST
void RF_Set_PacketConfig(uint8_t useVarLength,uint8_t useCRC,uint8_t useWhite,RF_Preamble_t preamble,RF_AdressFilter_t addfil)
{
	uint8_t lastv = RF_Get_Command(RF_REG_PKTC);
	
	if(useVarLength)lastv|=0x80;
	else lastv &=~0x80;
	
	if(useCRC)lastv|=0x08;
	else lastv &=~0x08;
	
	if(useWhite)lastv|=0x10;
	else lastv &=~0x10;
	
	lastv = (lastv & ~RF_Preamble_MASK) | (preamble & RF_Preamble_MASK);
	lastv = (lastv & ~RF_AdressFilter_MASK) | (addfil & RF_AdressFilter_MASK);
	
	RF_Set_Command(RF_REG_PKTC,lastv);
}

//Used to configure the receive gain
void RF_Set_Gain(RF_Gain_t gain)
{
	uint8_t lastv = RF_Get_Command(RF_REG_DMOD);
	lastv = (lastv & ~RF_Gain_MASK) | (gain & RF_Gain_MASK);
	RF_Set_Command(RF_REG_DMOD,lastv);
}

//Used to enable/disable the internal clock modul of the RF-Modul
void RF_Set_ClockOutput(uint8_t enable)
{
	uint8_t lastv = RF_Get_Command(RF_REG_CLKOUT);
	if(enable)lastv |= 0x80;
	else lastv &= ~(0x80);
	RF_Set_Command(RF_REG_CLKOUT,lastv);
}

//Used to configure the transmition power of the modul
void RF_Set_TXPower(RF_TX_Power_t power)
{
	uint8_t lastv = RF_Get_Command(RF_REG_TXCON);
	lastv = (lastv & ~RF_TX_POWER_MASK) | (power & RF_TX_POWER_MASK);
	RF_Set_Command(RF_REG_TXCON,lastv);
}

//Used to set the local nodeaddress of the device
void RF_Set_Address(uint8_t add)
{
	RF_Set_Command(RF_REG_NADDS,add);
}

//Used to configure the syncword - network identifier
void RF_Set_Sync(uint8_t* sync,uint8_t len)
{
	uint8_t lastv = RF_Get_Command(RF_REG_SYNC);
	lastv = (lastv & ~RF_SyncWord_MASK) | (((len-1)<<3) & RF_SyncWord_MASK) | RF_SyncWord_EN; //RF_SyncWord_EN enables SYNC WORD RECOGNITION!(bit 6)
	RF_Set_Command(RF_REG_SYNC,lastv);
	for (uint8_t i=0; i<4; i++)
	{
		RF_Set_Command(RF_REG_SYNCV31 +i,sync[i]);
	}
}

//Extended function to define the syncword from the predifined list of syncwords
void RF_Set_Sync_Num(uint8_t syncw_num)
{
	RF_Set_Sync(&RF_Syncwords[syncw_num][0], 4);
}

//Used to set the maximum payload length
void RF_Set_PayloadLenght(uint8_t len)
{
	uint8_t lastv = RF_Get_Command(RF_REG_PLOAD);
	lastv = (lastv & ~0x7F) | len;
	RF_Set_Command(RF_REG_PLOAD,lastv);//NOT TESTED!!!!
}

//Only used internally to set the FIFO access of the RF-Modul
void RF_Set_FIFOAccess(RF_FIFOAccess_t access)
{
	uint8_t lastv = RF_Get_Command(RF_REG_FCRC);
	lastv = (lastv & ~RF_FIFOAccess_MASK) | (access & RF_FIFOAccess_MASK);
	RF_Set_Command(RF_REG_FCRC,lastv);
}

//Used to set the interrupts the RF-Modul should generate during operation
void RF_Set_IRQSources(uint8_t irq0, uint8_t irq1, RF_TXIRQ1_t tx)
{
	uint8_t lastv = RF_Get_Command(RF_REG_FTXRXI);
	lastv = (lastv & ~RF_RXIRQ0_MASK) | (irq0 & RF_RXIRQ0_MASK);
	lastv = (lastv & ~RF_RXIRQ1_MASK) | (irq1 & RF_RXIRQ1_MASK);
	lastv = (lastv & ~RF_TXIRQ1_MASK) | (tx & RF_TXIRQ1_MASK);
	RF_CurrentStatus.IRQ0 = irq0;
	RF_CurrentStatus.IRQ1 = irq1;
	RF_Set_Command(RF_REG_FTXRXI,lastv);
	
	lastv = RF_Get_Command(RF_REG_FTPRI);
	lastv |= (1<<3) | (1<<4);//Set TX Flag
	RF_Set_Command(RF_REG_FTPRI,lastv);
	lastv = RF_Get_Command(RF_REG_FTPRI);
}

//Internally used to verify the configured frequency band
uint8_t RF_VerifyPLLLock(void)
{
    // Verify PLL-lock per instructions in Note 1 section 3.12
    // Need to do this after changing frequency.
    uint8_t ftpriVal = RF_Get_Command(RF_REG_FTPRI);
    RF_Set_Command(RF_REG_FTPRI, ftpriVal | 0x02); // Clear PLL lock bit
    RF_Set_State(RF_State_Freq_Syn);
	uint16_t counter =0;
    while ((counter++ < 500))
    {
        ftpriVal = RF_Get_Command(RF_REG_FTPRI);
        if ((ftpriVal & 0x02) != 0)
            break;
		_xdelay_ms(1);
    }
    RF_Set_State(RF_State_StandBy);
    return ((ftpriVal & 0x02) != 0);
}

//Used to configure the desired frequency to use
uint8_t RF_Set_Frequency(float centre)
{
    // REVISIT: FSK only: its different for OOK :-(

    RF_Band_t FBS;
    if (centre >= 902.0 && centre < 915.0)
    {
	FBS = RF_Band_Low;
    }
    else if (centre >= 915.0 && centre <= 928.0)
    {
	FBS = RF_Band_Mid;
    }
    else if (centre >= 863.0 && centre <= 870.0)
    {
	// Not all modules support this frequency band:
	// The MRF98XAM9A does not
	FBS = RF_Band_High;
    }
//    else if (centre >= 863.0 && centre <= 870.0)
//    {
//	// Not all modules support this frequency band:
//	// The MRF98XAM9A does not
//	FBS = RH_MRF89_FBS_950_960; // Yes same as above
//    }
    else
    {
	// Cant do this freq
	return false;
    }

    // Based on frequency calcs done in MRF89XA.h
//    uint8_t R = 100; // Recommended
    uint8_t R = 119; // Also recommended :-(
    uint32_t centre_kHz = centre * 1000;
    uint32_t xtal_kHz = (RF_XTAL_FREQ * 1000);
    uint32_t compare = (centre_kHz * 8 * (R + 1)) / (9 * xtal_kHz);
    uint8_t P = ((compare - 75) / 76) + 1;
    uint8_t S = compare - (75 * (P + 1));
	
    // Now set the new register values:
    uint8_t val = RF_Get_Command(RF_REG_GCON);
    val = (val & ~RF_Band_MASK) | (FBS & RF_Band_MASK);
    RF_Set_Command(RF_REG_GCON, val);

    RF_Set_Command(RF_REG_R1, R); 
    RF_Set_Command(RF_REG_P1, P); 
    RF_Set_Command(RF_REG_S1, S); 

    return RF_VerifyPLLLock();
}

//Set the Value in the defined register
//reg - the register on the RF-Modul
//val - the value the register should be set to
uint8_t RF_Set_Command(uint8_t reg,uint8_t val)
{
	RF_CS_COM_LOW();
	uint8_t r = (reg << 1);
	SPI_putc(r);
	SPI_putc(val);
	RF_CS_COM_HIGH();
	return RF_SPI_REG.DATA;
}

//Get the value of the defined register
uint8_t RF_Get_Command(uint8_t reg)
{
	RF_CS_COM_LOW();
	uint8_t r = (reg << 1);
	r |= (1<<6);//Set read bit
	SPI_putc(r);
	uint8_t val = SPI_getc();
	RF_CS_COM_HIGH();
	return val;
}

//Sends a broadcast message with the defined message buf
void RF_Send_Broadcast(uint8_t* buf, uint8_t len)
{
	RF_Set_State(RF_State_StandBy);
	RF_Set_FIFOAccess(RF_FIFOAccess_Write);
	RF_Send_DataHW(len + 4);//Payloadlength + 4 bytes head
	RF_Send_DataHW(0x00);//Receiver address!
	RF_Send_DataHW(RF_CurrentStatus.LocalDeviceAdd);//Transmitter address!
	RF_Send_DataHW(RF_CurrentStatus.LastPacketID);
	RF_Send_DataHW(0x00);//Flags - currently empty byte
	RF_CurrentStatus.LastPacketID++;
	for (uint8_t i=0; i<len; i++)
	{
		RF_Send_DataHW(buf[i]);
	}
	RF_Set_State(RF_State_Transmit);
}

//Sends a packet created with the RF_CreatePacket function
uint8_t RF_Send_Packet(RF_Packet_t packet)
{
	RF_Set_State(RF_State_StandBy);
	RF_Set_FIFOAccess(RF_FIFOAccess_Write);
	//uint8_t len = sizeof(packet.Data);
	//if(len != packet.Length)return 1;
	RF_Send_DataHW(packet.Length + 4);//Payloadlength + 4 bytes head
	RF_Send_DataHW(packet.Receiver);//Receiver address!
	RF_Send_DataHW(RF_CurrentStatus.LocalDeviceAdd);//Transmitter address!
	RF_Send_DataHW(packet.ID);
	RF_Send_DataHW(packet.Flags);//Flags

	for (uint8_t i=0; i<packet.Length; i++)
	{
		RF_Send_DataHW(packet.Data[i]);
	}
	RF_Set_State(RF_State_Transmit);
	return 0;
}

//Sends a ACK-Packet to the sender
uint8_t RF_Send_Acknowledgment(void)
{
	RF_Set_State(RF_State_StandBy);
	RF_Set_FIFOAccess(RF_FIFOAccess_Write);
	RF_Send_DataHW(4);//4 bytes head
	RF_Send_DataHW(RF_LastReceivedPacket.Sender);//Receiver address!
	RF_Send_DataHW(RF_CurrentStatus.LocalDeviceAdd);//Transmitter address!
	RF_Send_DataHW(RF_LastReceivedPacket.ID);
	RF_Send_DataHW(RF_Packet_Flags_Ack);//Flags
 	RF_Set_State(RF_State_Transmit);
	return 0;
}

//Creates a Packet with the supplied data, length of the data, the receiver address and the flag byte
RF_Packet_t RF_CreatePacket(uint8_t* data,uint8_t length,uint8_t receiver,uint8_t flags)
{
	RF_CurrentPacket.Length = length;
	RF_CurrentPacket.Receiver = receiver;
	RF_CurrentPacket.Flags = flags;
	RF_CurrentPacket.ID = RF_CurrentStatus.LastPacketID;
	RF_CurrentStatus.LastPacketID++;
	for (uint8_t i=0; i<length; i++)
	{
		RF_CurrentPacket.Data[i] = data[i];//Copy array
	}
	return RF_CurrentPacket;
}

//Internally used to put the data bytes through the SPI interface
static void RF_Send_DataHW(uint8_t data)
{
	RF_CS_DATA_LOW();
	SPI_putc(data);
	RF_CS_DATA_HIGH();
	_xdelay_us(2); //needed??
}

//Internally used to get the data from the SPI interface
static uint8_t RF_Get_DataHW(void)
{
	RF_CS_DATA_LOW();
	uint8_t data = SPI_getc();
	RF_CS_DATA_HIGH();
	_xdelay_us(2); //needed??
	return data;
}

//In this function all the interrupts from the RF-Modul are handled
//if the modul is in receivemode the modul will read the received packet and raise the NewPacket flag
//if the modul is in transmitmode it will leave this mode and go it standbymode to save energy
void RF_HandleInterrupt(void)
{
#pragma region RF_STATE_RECEIVE
	if(RF_CurrentStatus.State == RF_State_Receive)
	{
		RF_Set_FIFOAccess(RF_FIFOAccess_Read);
		//CRC OK -> Packet ready to read
		uint8_t leng = RF_Get_DataHW();//First byte is Packet length
		if(leng > RF_Max_PacketPayload)
		{
			//packet is wrong!
			for (uint8_t i=0; RF_Get_Command(RF_REG_FTXRXI) & RF_FLAG_FIFOEMPTY; i++)RF_Get_DataHW();
			return;//Length is to big!
		}
		uint8_t receiver = RF_Get_DataHW();
		uint8_t sender = RF_Get_DataHW();
		if(RF_CurrentConfig.RejectWrongAddress && (receiver != RF_CurrentStatus.LocalDeviceAdd || receiver != 0x00))
		{
			for (uint8_t i=0; RF_Get_Command(RF_REG_FTXRXI) & RF_FLAG_FIFOEMPTY; i++)RF_Get_DataHW();
			return; //Address rejection enabled and neither the address is matching the local address nor is this a Broadcast!
		}
		uint8_t id = RF_Get_DataHW();
		uint8_t flags = RF_Get_DataHW();
		
		if(RF_CurrentConfig.UseAcknowledgments)
		{
			if(!(flags & RF_Packet_Flags_Ack))
			{
				if(RF_LastReceivedPacket.ID != id || RF_LastReceivedPacket.Sender != sender)
				{
					//Neues Paket das wir noch nicht bekommen haben... wiederholungen sind ab diesem punkt ausgeschlossen
					RF_LastReceivedPacket.Length = leng;
					RF_LastReceivedPacket.Sender = sender;
					RF_LastReceivedPacket.Receiver = receiver;
					RF_LastReceivedPacket.ID = id;
					RF_LastReceivedPacket.Flags = flags;
					for (uint8_t i=0; RF_Get_Command(RF_REG_FTXRXI) & RF_FLAG_FIFOEMPTY; i++)
					{
						RF_LastReceivedPacket.Data[i] = RF_Get_DataHW();
					}
					if(receiver == 0x00)
					{
						//Wenn es eine Broadcastnachricht war dann müssen wir hier das NewPacket flag setzen... so setzen wir es nie
						RF_CurrentStatus.NewPacket=1;
						RF_CurrentStatus.IsStuck=0;
					}
				}
				
				if(receiver!=0x00)
				{
					//Send ACK-Packet
					RF_Send_Acknowledgment();
					RF_CurrentStatus.Acknowledgment = RF_Acknowledgments_State_Transmitted;
					RF_CurrentStatus.IsStuck=0;
				}
			}
			else
			{
				//Packet is ok ACK was received
				RF_CurrentStatus.Acknowledgment = RF_Acknowledgments_State_Idle;
				RF_CurrentStatus.AckResult = RF_Acknowledgments_Result_OK;
				RF_Set_State(RF_State_StandBy);
				RF_CurrentStatus.IsStuck=0;
			}
		}
		else
		{
			//Ohne ACKs können wir die daten einfach auslesen und das NewPacket flag setzen... die einfachste methode!
			RF_LastReceivedPacket.Length = leng;
			RF_LastReceivedPacket.Sender = sender;
			RF_LastReceivedPacket.Receiver = receiver;
			RF_LastReceivedPacket.ID = id;
			RF_LastReceivedPacket.Flags = flags;
			for (uint8_t i=0; RF_Get_Command(RF_REG_FTXRXI) & RF_FLAG_FIFOEMPTY; i++)
			{
				RF_LastReceivedPacket.Data[i] = RF_Get_DataHW();
			}
			RF_CurrentStatus.NewPacket=1;
			RF_CurrentStatus.IsStuck=0;
		}
	}
#pragma endregion RF_STATE_RECEIVE
#pragma region RF_STATE_TRANSMIT
	else if(RF_CurrentStatus.State == RF_State_Transmit)
	{
		//Transmit completed
		if(RF_CurrentConfig.UseAcknowledgments)
		{
			if( RF_CurrentStatus.Acknowledgment == RF_Acknowledgments_State_Idle)
			{
				//Packet wurde versand.... warte jetzt auf das ACK
				RF_CurrentStatus.Acknowledgment = RF_Acknowledgments_State_Pending;
				RF_CurrentStatus.AckResult = RF_Acknowledgments_Result_Idle;
				RF_CurrentStatus.AckTimeout = 0;
				RF_CurrentStatus.AckRetransmit = 0;
				RF_Set_State(RF_State_Receive);
			}
			else if(RF_CurrentStatus.Acknowledgment == RF_Acknowledgments_State_Transmitted)
			{
				//ACK-Paket wurde abgesendet
				RF_CurrentStatus.Acknowledgment = RF_Acknowledgments_State_Idle;
				RF_Set_State(RF_State_StandBy);
				RF_CurrentStatus.NewPacket=1;
			}
		}
		else RF_Set_State(RF_State_StandBy);
	}
#pragma endregion RF_STATE_TRANSMIT
}

//this is the update function of the RF-Modul
//it handles hardware faults aswell as client handling in the registration list
void RF_Update(void)
{
	//Tick every 10ms
	if(RF_CurrentConfig.UseAcknowledgments && RF_CurrentStatus.Acknowledgment == RF_Acknowledgments_State_Pending)
	{
		if(RF_CurrentStatus.AckTimeout++ > 100)
		{
			RF_CurrentStatus.AckTimeout = 0;
			if(RF_CurrentStatus.AckRetransmit < 3)
			{
				//Retransmit 3 times
				RF_CurrentStatus.AckRetransmit++;
				RF_Send_Packet(RF_CurrentPacket);
			}
			else
			{
				 //Packet is lost no retransmition!
				 RF_CurrentStatus.Acknowledgment = RF_Acknowledgments_State_Idle;
				 RF_CurrentStatus.AckResult = RF_Acknowledgments_Result_ERROR;
				 RF_CurrentStatus.AckRetransmit=0;
			}
		}
	}
		
	if(RF_CurrentConfig.UseAcknowledgments && RF_CurrentStatus.Acknowledgment == RF_Acknowledgments_State_Transmitted)
	{
		//Hardware error!!! No Transmitted interrupt was generated from RF Modul!
		if(RF_CurrentStatus.AckTimeout++ > 100)
		{
			RF_CurrentStatus.AckTimeout = 0;
			RF_CurrentStatus.Acknowledgment = RF_Acknowledgments_State_Idle;
			RF_Set_State(RF_State_StandBy);
		}
	}
	
#pragma region TimeSlots
	if(RF_CurrentStatus.CurrentSlotTime++ > 30000)
	{
		RF_CurrentStatus.CurrentSlotTime = 0;
	}

	if((RF_CurrentStatus.CurrentSlotTime % 6000) == 0)//Alle 1Minute
	{
		for (uint8_t i = 0; i < RF_MaxDevices; i++)
		{
			if(!RF_CheckDeviceSlot(i))
			{
				RF_CurrentStatus.TimeSlots[i].Timeout++;
				if(RF_CurrentStatus.TimeSlots[i].Timeout > 30)//FEHLER ... Gerät hat sich seit 30 minuten nicht gemeldet! Wird aus dem TimeSlots entfernt!
				{
					RF_UnregisterDevice(i);
				}
			}
		}
	}
#pragma endregion TimeSlots
	
#pragma region Debug Error
	if(RF_IRQ1_PORT.IN & (1<<RF_IRQ1_PIN))
	{
		if(RF_CurrentStatus.IsStuck++>10)
		{
			RF_CurrentStatus.IsStuck=0;
			RF_CurrentStatus.AckTimeout = 0;
			RF_CurrentStatus.Acknowledgment = RF_Acknowledgments_State_Idle;
			RF_Set_State(RF_State_StandBy);
			_xdelay_ms(10);
			RF_Set_State(RF_State_Receive);
		}
	}
#pragma endregion Debug Error

}

//Gets the last read packet from the buffer
RF_Packet_t RF_Get_Packet(void)
{
	RF_CurrentStatus.NewPacket=0;
	return RF_LastReceivedPacket;
}

//DEPRICATED
void RF_Get_Data(uint8_t* buf, uint8_t len)
{
	RF_Set_FIFOAccess(RF_FIFOAccess_Read);
	for (uint8_t i=0; i<len; i++)
	{
		buf[i] = RF_Get_DataHW();
	}
	RF_Set_FIFOAccess(RF_FIFOAccess_Write);
}

//Used to get the signal strength of the last received packet
uint8_t RF_Get_SignalStrength(void)
{
	return RF_Get_Command(RF_REG_RSTS);
}


//Basisstation only

//This function registers a device in the timeslottable
//ID -> the nodeaddress of the device
//returns the id of the timeslot
uint8_t RF_RegisterDevice(uint8_t ID)
{
	for (uint8_t i = 1; i < RF_MaxDevices; i++)
	{
		if(RF_CheckDeviceSlot(i))
		{
			RF_CurrentStatus.TimeSlots[i].ID = ID;
			RF_CurrentStatus.TimeSlots[i].Timeout = 0;
			return i;
		}
	}
	return 0;
}

//This function removes a device from the timeslottable
//index -> the index in the timeslottable that you want to remove
void RF_UnregisterDevice(uint8_t ID)
{
	RF_CurrentStatus.TimeSlots[ID].ID=0;
	RF_CurrentStatus.TimeSlots[ID].Timeout=0;
}

//This function returns the sleeping time for a specific device that is registert int the timeslottable
//index -> the index of the device in the timeslottable you want to get the time to
uint16_t RF_GetDeviceSleepTime(uint8_t ID)
{
	uint16_t s = ID* (300 / RF_MaxDevices);
	return (30000 - RF_CurrentStatus.CurrentSlotTime + s);
}

//This function finds a specific device from its nodeaddress and returns the index of the device in the timeslottable
//ID -> nodeaddress of the device you want to find
//returns -> the index of the device in the timeslottable
uint8_t RF_FindDevice(uint8_t ID)
{
	for (uint8_t i = 1; i < RF_MaxDevices; i++)
	{
		if(RF_CurrentStatus.TimeSlots[i].ID == ID)return i;
	}
	return 0;
}

//This function checks if the specified index is free in the timeslotlist
//index -> the index you want to check if its free
//returns -> 1 if the index is empty, 0 if its full
uint8_t RF_CheckDeviceSlot(uint8_t ID)
{
	if(RF_CurrentStatus.TimeSlots[ID].ID == 0)return 1;
	else return 0;
}