/*
 * RF.h
 *
 * Created: 31.05.2017 10:41:47
 *  Author: Stud
 */ 


#ifndef RF_H_
#define RF_H_

#include <avr/io.h>
#include <util/delay.h>

#define RF_MaxDevices 16

#define RF_Max_PacketPayload 32

#define RF_CS_DATA_PORT PORTD
#define RF_CS_COM_PORT PORTD
#define RF_CS_DATA_PIN 0
#define RF_CS_COM_PIN 3
#define RF_SPI_PORT PORTD
#define RF_SPI_REG SPID

//ÜBERPRÜFEN!!!!!!!!!
//CHANGE THIS LINE IN RF.C TOO!!!
//RF_IRQ1_PORT.PIN0CTRL = (PORT_ISC_RISING_gc);
//................|............................
//to the pin its connected to
#define RF_IRQ0_PORT PORTD
#define RF_IRQ1_PORT PORTE
#define RF_IRQ0_PIN 2
#define RF_IRQ1_PIN 0

//ÜBERPRÜFEN!!!!!!!!!
#define RF_Reset_PORT PORTD
#define RF_Reset_Pin 1

#define RF_XTAL_FREQ 12.8

#define RF_CS_DATA_LOW() (RF_CS_DATA_PORT.OUTCLR = (1<<RF_CS_DATA_PIN))
#define RF_CS_DATA_HIGH() (RF_CS_DATA_PORT.OUTSET = (1<<RF_CS_DATA_PIN))
#define RF_CS_COM_LOW() (RF_CS_COM_PORT.OUTCLR = (1<<RF_CS_COM_PIN))
#define RF_CS_COM_HIGH() (RF_CS_COM_PORT.OUTSET = (1<<RF_CS_COM_PIN))

#define RF_RXIRQ0_Cont_RSSI 0x40
#define RF_RXIRQ0_Cont_SYNC 0x00
#define RF_RXIRQ0_Buffer_SYNC 0xc0
#define RF_RXIRQ0_Buffer_FIFOEMPTY 0x80
#define RF_RXIRQ0_Buffer_WRITEBYTE 0x40
#define RF_RXIRQ0_Buffer_NONE 0x00
#define RF_RXIRQ0_Packet_SYNC 0xc0
#define RF_RXIRQ0_Packet_FIFOEMPTY 0x80
#define RF_RXIRQ0_Packet_WRITEBYTE 0x40
#define RF_RXIRQ0_Packet_PLREADY 0x00

/*
typedef enum RF_RXIRQ0{RF_RXIRQ0_Cont_RSSI=0x40,
					RF_RXIRQ0_Cont_SYNC=0x00,
					RF_RXIRQ0_Buffer_SYNC=0xc0,
					RF_RXIRQ0_Buffer_FIFOEMPTY=0x80,
					RF_RXIRQ0_Buffer_WRITEBYTE=0x40,
					RF_RXIRQ0_Buffer_NONE=0x00,
					RF_RXIRQ0_Packet_SYNC=0xc0,
					RF_RXIRQ0_Packet_FIFOEMPTY=0x80,
					RF_RXIRQ0_Packet_WRITEBYTE=0x40,
					RF_RXIRQ0_Packet_PLREADY=0x00}RF_RXIRQ0_t;*/
#define RF_RXIRQ0_MASK 0xc0

#define RF_RXIRQ1_Cont_DCLK 0x00
#define RF_RXIRQ1_Buffer_FIFOTRESH 0x30
#define RF_RXIRQ1_Buffer_RSSI 0x20
#define RF_RXIRQ1_Buffer_FIFOFULL 0x10
#define RF_RXIRQ1_Buffer_NONE 0x00
#define RF_RXIRQ1_Packet_FIFOTRESH 0x30
#define RF_RXIRQ1_Packet_RSSI 0x20
#define RF_RXIRQ1_Packet_FIFOFULL 0x10
#define RF_RXIRQ1_Packet_CRCOK 0x00

/*typedef enum RF_RXIRQ1{RF_RXIRQ1_Cont_DCLK=0x00,
					RF_RXIRQ1_Buffer_FIFOTRESH=0x30,
					RF_RXIRQ1_Buffer_RSSI=0x20,
					RF_RXIRQ1_Buffer_FIFOFULL=0x10,
					RF_RXIRQ1_Buffer_NONE=0x00,
					RF_RXIRQ1_Packet_FIFOTRESH=0x30,
					RF_RXIRQ1_Packet_RSSI=0x20,
					RF_RXIRQ1_Packet_FIFOFULL=0x10,
					RF_RXIRQ1_Packet_CRCOK=0x00}RF_RXIRQ1_t;*/
#define RF_RXIRQ1_MASK 0x30

typedef enum RF_TXIRQ1{RF_TXIRQ1_TXDONE=0x08, RF_TXIRQ1_FIFOFULL=0x00}RF_TXIRQ1_t;
#define RF_TXIRQ1_MASK 0x08

//FLAGS from FTXRXIREG 0x0D
#define RF_FLAG_FIFOFULL 0x04
#define RF_FLAG_FIFOEMPTY 0x02
#define RF_FLAG_FIFOOVERRUN 0x01

typedef enum RF_Packet_Flags{RF_Packet_Flags_Ack=0x01,RF_Packet_Flags_Time=0x02,RF_Packet_Flags_Weather=0x04, RF_Packet_Flags_AllSensors=0x08, RF_Packet_Flags_BatLow=0x10}RF_Packet_Flags_t;
#define RF_Packet_Flags_MASK 0xff

typedef enum RF_Acknowledgments_State{RF_Acknowledgments_State_Pending,RF_Acknowledgments_State_Transmitted,RF_Acknowledgments_State_Idle,RF_Acknowledgments_State_Error}RF_Acknowledgments_State_t;

typedef enum RF_Acknowledgments_Result{RF_Acknowledgments_Result_OK,RF_Acknowledgments_Result_ERROR,RF_Acknowledgments_Result_Idle}RF_Acknowledgments_Result_t;

typedef enum RF_State{RF_State_Transmit=0x80,
					RF_State_Receive=0x60,
					RF_State_Freq_Syn=0x40,
					RF_State_StandBy=0x20,
					RF_State_Sleep=0x00} RF_State_t;
#define RF_State_MASK 0xe0

//RF-Band selection -> LOW: 902-915 | Mid: 915-928 | High: 863-870
typedef enum RF_Band{RF_Band_Low=0x00,
					RF_Band_Mid=0x08,
					RF_Band_High=0x10} RF_Band_t;
#define RF_Band_MASK 0x18

typedef enum RF_Modulation{RF_Modulation_FSK=0x80,
						RF_Modulation_OOK=0x40} RF_Modulation_t;
#define RF_Modulation_MASK 0xc0

typedef enum RF_Gain{RF_Gain_G0_MAX=0x00,
					RF_Gain_G4=0x01,
					RF_Gain_G9=0x02,
					RF_Gain_G13_MIN=0x03} RF_Gain_t;
#define RF_Gain_MASK 0x03

typedef enum RF_TX_Power{RF_TX_Power_13DB=0x00,
						RF_TX_Power_10DB=0x02,
						RF_TX_Power_7DB=0x04,
						RF_TX_Power_4DB=0x06,
						RF_TX_Power_1DB=0x08,
						RF_TX_Power_M2DB=0x0a,
						RF_TX_Power_M5DB=0x0c,
						RF_TX_Power_M8DB=0x0e} RF_TX_Power_t;
#define RF_TX_POWER_MASK 0x0e

typedef enum RF_FIFOAccess{RF_FIFOAccess_Read=0x40,RF_FIFOAccess_Write=0x00}RF_FIFOAccess_t;
#define RF_FIFOAccess_MASK 0x40

typedef enum RF_FIFOSize{RF_FIFOSize_F16Byte=0x00,
						RF_FIFOSize_F32Byte=0x40,
						RF_FIFOSize_F48Byte=0x80,
						RF_FIFOSize_F64Byte=0xc0} RF_FIFOSize_t;
#define RF_FIFOSize_MASK 0xc0

typedef enum RF_Mode{RF_Mode_Packet,RF_Mode_Continous,RF_Mode_Buffered} RF_Mode_t;
	
typedef enum RF_Preamble{RF_Preamble_4=0x60,
						RF_Preamble_3=0x40,
						RF_Preamble_2=0x20,
						RF_Preamble_1=0x00} RF_Preamble_t;
#define RF_Preamble_MASK 0x60

typedef enum RF_AdressFilter{RF_AddressFilter_OFF=0x00,
							RF_AddressFilter_NODEADD=0x02,
							RF_AddressFilter_NODEADD_00=0x04,
							RF_AddressFilter_NODEADD_00_FF=0x06} RF_AdressFilter_t;
#define RF_AdressFilter_MASK 0x06

typedef enum RF_SyncWord{RF_SyncWord_32=0x18,
						RF_SyncWord_24=0x10,
						RF_SyncWord_16=0x08,
						RF_SyncWord_8=0x00,
						RF_SyncWord_EN=0x20} RF_SyncWord_t;
#define RF_SyncWord_MASK 0x18

#define RF_REG_GCON 0x00
#define RF_REG_DMOD 0x01
#define RF_REG_FDEV 0x02
#define RF_REG_BRS 0x03
#define RF_REG_FIFOC 0x05
#define RF_REG_R1 0x06
#define RF_REG_P1 0x07
#define RF_REG_S1 0x08
#define RF_REG_FTXRXI 0x0D
#define RF_REG_FTPRI 0x0E
#define RF_REG_RSTHI 0x0F
#define RF_REG_FILC 0x10

#define RF_REG_SYNC 0x12
#define RF_REG_RSTS 0x14
#define RF_REG_SYNCV31 0x16
#define RF_REG_SYNCV23 0x17
#define RF_REG_SYNCV15 0x18
#define RF_REG_SYNCV07 0x19
#define RF_REG_TXCON 0x1A
#define RF_REG_CLKOUT 0x1B
#define RF_REG_PLOAD 0x1C
#define RF_REG_NADDS 0x1D
#define RF_REG_PKTC 0x1E
#define RF_REG_FCRC 0x1F

typedef struct RF_Packet
{
	uint8_t Sender;
	uint8_t Receiver;
	uint8_t ID;
	uint8_t Flags;//Flags-> Bit0 -> ACK
	uint8_t Data[RF_Max_PacketPayload];
	uint8_t Length;
} RF_Packet_t;

typedef struct RF_Config
{
	uint8_t RejectWrongAddress :1;	
	uint8_t UseAcknowledgments :1;
} RF_Config_t;

typedef struct RF_TimeSlot
{
	uint8_t ID;
	uint8_t Timeout;
}RF_TimeSlot_t;

typedef struct RF_Status
{
	RF_State_t State;
	uint8_t LocalDeviceAdd;
	uint8_t LastPacketID;
	uint8_t IRQ0;
	uint8_t IRQ1;
	uint8_t NewPacket;
	RF_Acknowledgments_State_t Acknowledgment;
	RF_Acknowledgments_Result_t AckResult;
	uint8_t AckTimeout;
	uint8_t AckRetransmit;
	uint8_t IsStuck;
	RF_TimeSlot_t TimeSlots[RF_MaxDevices];
	uint16_t CurrentSlotTime;
	uint8_t PacketsLost;
} RF_Status_t;
	
void RF_Init(uint8_t dev_add, uint8_t syncw_num);
void RF_Set_State(RF_State_t state);
void RF_Set_Band(RF_Band_t band);
void RF_Set_Modulation(RF_Modulation_t mod);
void RF_Set_FIFOSize(RF_FIFOSize_t fs);
void RF_Set_Mode(RF_Mode_t mode);
void RF_Set_PacketConfig(uint8_t useVarLength,uint8_t useCRC,uint8_t useWhite,RF_Preamble_t preamble,RF_AdressFilter_t addfil);
void RF_Set_Gain(RF_Gain_t gain);
void RF_Set_ClockOutput(uint8_t enable);
void RF_Set_TXPower(RF_TX_Power_t power);
void RF_Set_Address(uint8_t add);
void RF_Set_Sync(uint8_t* sync,uint8_t len);
void RF_Set_Sync_Num(uint8_t syncw_num);
void RF_Set_PayloadLenght(uint8_t len);
void RF_Set_FIFOAccess(RF_FIFOAccess_t access);
void RF_Set_IRQSources(uint8_t irq0, uint8_t irq1,RF_TXIRQ1_t tx);
void RF_Send_Broadcast(uint8_t* buf, uint8_t len);
uint8_t RF_Send_Packet(RF_Packet_t packet);
uint8_t RF_Send_Acknowledgment(void);
RF_Packet_t RF_CreatePacket(uint8_t* data,uint8_t length,uint8_t receiver,uint8_t flags);

uint8_t RF_Set_Command(uint8_t reg,uint8_t val);
uint8_t RF_Get_Command(uint8_t reg);
void RF_Send_Data(uint8_t* buf, uint8_t len);
void RF_Get_Data(uint8_t* buf, uint8_t len);
uint8_t RF_Get_SignalStrength(void);
RF_Packet_t RF_Get_Packet(void);
void RF_HandleInterrupt(void);
void RF_Set_Modem(void);
void RF_Update(void);
void RF_Sleep(void);
void RF_Wakeup(void);

uint8_t RF_VerifyPLLLock(void);
uint8_t RF_Set_Frequency(float centre);

extern RF_Status_t RF_CurrentStatus;

uint8_t RF_RegisterDevice(uint8_t ID);
void RF_UnregisterDevice(uint8_t ID);
uint8_t RF_CheckDeviceSlot(uint8_t ID);
uint16_t RF_GetDeviceSleepTime(uint8_t ID);
uint8_t RF_FindDevice(uint8_t ID);

#endif /* RF_H_ */