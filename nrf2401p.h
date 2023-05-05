#ifndef NRF2401P_H_INCLUDED__
#define NRF2401P_H_INCLUDED__

typedef unsigned char Nrf_Byte;

/*
	Data rate for transmitter
*/
typedef enum 
{
	NRF_1MBPS,
	NRF_2MPBS,
	NRF_250KBPS
} Nrf_DataRate;

/*
	CNC scheme 
*/
typedef enum 
{
	NRF_CNC_NONE = 0x0,
	NRF_CNC_1BYTE,
	NRF_CNC_2BYTE
} Nrf_CNC;

/*
	Type of signal for spi communcation
*/
typedef enum
{
	NRF_SPI_SELECT,
	NRF_SPI_UNSELECT
} Nrf_SPISelectType;

/*
	Type of signal for nrf device enable
*/
typedef enum
{
	NRF_CHIP_ENABLE,
	NRF_CHIP_ENABLE_PULSE,
	NRF_CHIP_DISABLE
} Nrf_ChipEnableType;

/*
	Identifier for device's pipe
*/
typedef enum 
{
	NRF_DATA_PIPE_0 = 0x1,
	NRF_DATA_PIPE_1 = 0x2,
	NRF_DATA_PIPE_2 = 0x3,
	NRF_DATA_PIPE_3 = 0x4,
	NRF_DATA_PIPE_4 = 0x5,
	NRF_DATA_PIPE_5 = 0x6
} Nrf_DataPipeId;

/*
	Size of addresses used by device
*/
typedef enum 
{
	NRF_ADDR_3BYTES = 0x1,
	NRF_ADDR_4BYTES = 0x2,
	NRF_ADDR_5BYTES = 0x3
} Nrf_AddressWidth;

/*
	Transmission power
*/
typedef enum 
{
	NRF_OUT_POWER_18DBM = 0x0,
	NRF_OUT_POWER_12DBM = 0x1,
	NRF_OUT_POWER_6DBM = 0x2,
	NRF_OUT_POWER_0DBM = 0x3
} Nrf_TransmitPower;

/*
	Auto acknowledge feature flag
*/
typedef enum 
{
	NRF_AUTO_ACK_DISABLE = 0x0,
	NRF_AUTO_ACK_ENABLE = 0x01
} Nrf_AutoAckType;

/*
	Pipe address type
*/
typedef struct 
{
	Nrf_Byte addr[5];
	Nrf_Byte len;
} Nrf_Address;

/*
	Pipe is the representation of sender. Nrf2401+ supports up to 6 pipes with the same address prefix.
	You can assign different payload sizes/auto acknoledge features for each pipe.
*/
typedef struct 
{
/*
		Auto acknowledge for data pipe. It will use retransmissions.
*/
	Nrf_AutoAckType auto_ack;

/*
		Address for data pipe. It must be unique for each data pipe with the same prefix.
		Prefix is taken into consideration only for first and second hosts.
*/
	Nrf_Address address;

/*
		Payload size for data pipe. Size lies within [0, 32] range. Default value is 32. 
		Otherwise hardware feature (known as dynamic payload) will be used
*/
	Nrf_Byte payload_size;
} Nrf_DataPipeOptions;

/*
	Source of the interruption
*/
typedef enum 
{
	NRF_INTERRUPT_NONE = 0x0,
	NRF_INTERRUPT_RX = 0x1,
	NRF_INTERRUPT_TX = 0x2,
	NRF_INTERRUPT_MAX_RT = 0x4
} Nrf_InterruptMask;

/*
	Device global options. They are used to configure nrf device, not matter you will use it as trasnmitter or receiver.
*/
typedef struct 
{
//	Interruption settings
	Nrf_InterruptMask interrupt_mask;
//	Channel value (up to 128 )
	Nrf_Byte rf_channel;
//	Data rate
	Nrf_DataRate data_rate;
//	Cnc scheme
	Nrf_CNC cnc;
//	Transmission power
	Nrf_TransmitPower power;
//	Width of address	
	Nrf_AddressWidth address_width;
} Nrf_GlobalOptions;

/*
	Device mode
*/
typedef enum
{
	NRF_SLEEP = 0x0,
	NRF_TRASMITTER = 0x2,
	NRF_RECEIVER = 0x3
} Nrf_Mode;

/*
	Device status to indicate results from device
*/
typedef enum
{
	NRF_NONE = 0x0,
	NRF_PACKET_ARRIVED = 0x2,
	NRF_PACKET_SENT = 0x4,
	NRF_MAX_RETRANSMITS = 0x8,
	NRF_TX_FIFO_FULL = 0x10
} Nrf_Status;

/*	
   =========== USER PROVIDED FUNCTIONS =============
   This library is hardware agnostic. It only provides interface for spi/device communcation.
*/
/*
	Pulses chip enable signal
*/
void     Nrf_PulseChipEnable();

/*
	Emits chip enable signal
	Params: chip enable flag
	Return: none
*/
void     Nrf_ChipEnable(Nrf_ChipEnableType enable_flag);

/*
	Selects spi line
	Params: select flag
	Return: none
*/
void Nrf_SPISelect(Nrf_SPISelectType select_flag);

/*
	Write byte with spi interface
	Params: sent byte
	Return: received byte
*/
Nrf_Byte Nrf_WriteSpi(Nrf_Byte data);
/*	=================================================*/

/*
		Initializes nrf2401+ device
		Params: global options for device
		Return: none
		
		NOTE: it's important to keep period ~100ms since powering nrfdevice. This is needed for transfering device into POWER DOWN state.
		Otherwise device will not be configured properly.
*/
void Nrf_Init(const Nrf_GlobalOptions* const options);

/*
		Adds pipe to device
		Params: pipe options
		REturn: none
*/
void Nrf_AddPipe(const Nrf_DataPipeOptions* const pipe_options);

/*
		Sets mode for device
		Params: device mode
		Return: none
*/
void Nrf_SetMode(const Nrf_Mode);

/*
		Transmits data
		Params: data, data length, recipient's address
		Return: none
*/
void Nrf_Transmit(const Nrf_Byte* const data, const Nrf_Byte len, const Nrf_Address* tx_address);

/*
		Reads data from pip
		Params: data buffer, maximal length, out pipe number
		Return: count of received bytes
*/
Nrf_Byte Nrf_Receive(Nrf_Byte* const data, Nrf_Byte* len, Nrf_Byte* pipe_number);

/*
		Gets status of last action
		Params: none
		Return: last action status
*/
Nrf_Status Nrf_GetStatus();

/*
		Clears device status
		Params: none
		REturn: none
*/
void Nrf_ClearStatus();

/*
		Clears queue of messages to transmit
		Params: none
		Return: none
*/
void Nrf_FlushTxFifo();

/*
		Clears queue of received messages
		Params: none
		Return: none
*/
void Nrf_FlushRxFifo();

#endif