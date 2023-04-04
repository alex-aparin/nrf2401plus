#ifndef NRF2401P_H_INCLUDED__
#define NRF2401P_H_INCLUDED__

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

typedef struct 
{
	uint8_t auto_ack;
	uint8_t addr_postfix;
	uint8_t payload_size;
} Nrf_DataPipeOptions;

typedef struct 
{
	uint8_t addr_prefix[4];
	Nrf_DataPipeOptions data_pipes[6];
	Nrf_DataPipeId pipes_count;
	Nrf_AddressWidth address_width;
} Nrf_ReceiveOptions;

typedef struct 
{
	Nrf_TransmitPower power;
} Nrf_TransmitOptions;

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
	uint8_t rf_channel;	//	up to 128 
	Nrf_DataRate data_rate;
	Nrf_CNC cnc;
	Nrf_ReceiveOptions* receive_settings;
	Nrf_TransmitOptions* transmit_options;
} Nrf_GlobalOptions;

void Nrf_WriteSpi(uint8_t* data, uint8_t len);
void Nrf_ReadSpi(uint8_t* data, uint8_t len);

void Nrf_Init(const Nrf_GlobalOptions* const options);

#endif