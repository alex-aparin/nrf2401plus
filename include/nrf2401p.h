#ifndef NRF2401P_H_INCLUDED__
#define NRF2401P_H_INCLUDED__

enum Nrf_DataRate
{
	NRF_1MBPS,
	NRF_2MPBS,
	NRF_250KBPS
};

enum Nrf_CNC
{
	NRF_CNC_NONE = 0x0,
	NRF_CNC_1BYTE,
	NRF_CNC_2BYTE
};

enum Nrf_DataPipeId
{
	NRF_DATA_PIPE_0 = 0x1,
	NRF_DATA_PIPE_1 = 0x2,
	NRF_DATA_PIPE_2 = 0x3,
	NRF_DATA_PIPE_3 = 0x4,
	NRF_DATA_PIPE_4 = 0x5,
	NRF_DATA_PIPE_5 = 0x6
}

enum Nrf_AddressWidth
{
	NRF_ADDR_3BYTES = 0x1,
	NRF_ADDR_4BYTES = 0x2,
	NRF_ADDR_5BYTES = 0x3
}

enum Nrf_TransmitPower
{
	NRF_OUT_POWER_18DBM = 0x0,
	NRF_OUT_POWER_12DBM = 0x1,
	NRF_OUT_POWER_6DBM = 0x2,
	NRF_OUT_POWER_0DBM = 0x3
}

struct Nrf_DataPipeOptions
{
	uint8_t auto_ack;
	uint8_t addr_postfix;
	uint8_t payload_size;
};

struct Nrf_ReceiveOptions
{
	uint8_t[4] addr_prefix;
	Nrf_DataPipeOptions[6] data_pipes;
	Nrf_DataPipeId pipes_count;
	Nrf_AddressWidth address_width;
};

struct Nrf_TransmitOptions
{
	Nrf_TransmitPower power;
};

struct Nrf_GlobalOptions
{
	uint8_t rf_channel;	//	up to 128 
	Nrf_DataRate data_rate;
	Nrf_CNC cnc;
	Nrf_ReceiveOptions* receive_settings;
	Nrf_TransmitOptions* transmit_options;
};

void Nrf_WriteSpi(uint8_t* data, uint8_t len);
void Nrf_ReadSpi(uint8_t* data, uint8_t len);

void Nrf_Init(const Nrf_GlobalOptions* const options);

#endif