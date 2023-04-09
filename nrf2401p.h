#ifndef NRF2401P_H_INCLUDED__
#define NRF2401P_H_INCLUDED__

typedef unsigned char Nrf_Byte;

typedef enum 
{
	NRF_1MBPS,
	NRF_2MPBS,
	NRF_250KBPS
} Nrf_DataRate;

typedef enum 
{
	NRF_CNC_NONE = 0x0,
	NRF_CNC_1BYTE,
	NRF_CNC_2BYTE
} Nrf_CNC;

typedef enum
{
	NRF_SELECT,
	NRF_UNSELECT
} Nrf_ChipSelectType;

typedef enum 
{
	NRF_DATA_PIPE_0 = 0x1,
	NRF_DATA_PIPE_1 = 0x2,
	NRF_DATA_PIPE_2 = 0x3,
	NRF_DATA_PIPE_3 = 0x4,
	NRF_DATA_PIPE_4 = 0x5,
	NRF_DATA_PIPE_5 = 0x6
} Nrf_DataPipeId;

typedef enum 
{
	NRF_ADDR_3BYTES = 0x1,
	NRF_ADDR_4BYTES = 0x2,
	NRF_ADDR_5BYTES = 0x3
} Nrf_AddressWidth;

typedef enum 
{
	NRF_OUT_POWER_18DBM = 0x0,
	NRF_OUT_POWER_12DBM = 0x1,
	NRF_OUT_POWER_6DBM = 0x2,
	NRF_OUT_POWER_0DBM = 0x3
} Nrf_TransmitPower;

typedef enum 
{
	NRF_AUTO_ACK_DISABLE = 0x0,
	NRF_AUTO_ACK_ENABLE = 0x01
} Nrf_AutoAckType;

typedef struct 
{
	Nrf_Byte addr[5];
	Nrf_Byte len;
} Nrf_Address;

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



typedef enum 
{
	NRF_INTERRUPT_NONE = 0x0,
	NRF_INTERRUPT_RX = 0x1,
	NRF_INTERRUPT_TX = 0x2,
	NRF_INTERRUPT_MAX_RT = 0x4
} Nrf_InterruptMask;

typedef struct 
{
	Nrf_InterruptMask interrupt_mask;
	Nrf_Byte rf_channel;	//	up to 128 
	Nrf_DataRate data_rate;
	Nrf_CNC cnc;
	Nrf_TransmitPower power;
	Nrf_AddressWidth address_width;
} Nrf_GlobalOptions;

typedef enum
{
	NRF_SLEEP = 0x0,
	NRF_TRASMITTER = 0x2,
	NRF_RECEIVER = 0x3
} Nrf_Mode;

typedef enum
{
	NRF_NONE = 0x0,
	NRF_PACKET_ARRIVED = 0x2,
	NRF_PACKET_SENT = 0x4,
	NRF_MAX_RETRANSMITS = 0x8,
	NRF_TX_FIFO_FULL = 0x10
} Nrf_Status;

void Nrf_PulseChipEnable();
void Nrf_Select(Nrf_ChipSelectType select_flag);
Nrf_Byte Nrf_WriteSpi(Nrf_Byte data);

/*
		Initializes nrf2401+ device
		Params: global options for device
		Return: none
		
		NOTE: it's important to keep period ~100ms since powering nrfdevice. This is needed for transfering device into POWER DOWN state.
		Otherwise device will not be configured properly.
*/
void Nrf_Init(const Nrf_GlobalOptions* const options);
void Nrf_AddPipe(const Nrf_DataPipeOptions* const pipe_options);
void Nrf_SetMode(const Nrf_Mode);
void Nrf_Transmit(const Nrf_Byte* const data, const Nrf_Byte len, const Nrf_Address* tx_address);
Nrf_Byte Nrf_Receive(Nrf_Byte* const data, Nrf_Byte* len, Nrf_Byte* pipe_number);
Nrf_Status Nrf_GetStatus();
void Nrf_ClearStatus();
void Nrf_FlushTxFifo();
void Nrf_FlushRxFifo();

#endif