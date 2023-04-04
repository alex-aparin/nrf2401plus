#include "nrf2401p.h"

#define TO_INT(x) ((x) ? 1 : 0)

//	Register map table
#define REG_CONFIG 		0x0 	//	Configuration register
#define REG_ENAA 		0x1		//	Auto acknowledgement for data pipes
#define REG_EN_RXADDR 	0x2		//	Enable rx addresses
#define REG_SETUP_AW 	0x3 	//	Setup for addresses widths (common for all data pipes)
#define REG_SETUP_RETR	0x4		//	Setup for automatic retransmission
#define REG_RF_CH		0x5		//	Radiofrequency channel
#define REG_RF_SETUP	0x6		//	Radiofrequency setup
#define REG_STATUS		0x7		//	Status register (informs about messages)
#define REG_OBSERVE_TX 	0x8 	//	Transmit observer registers
#define REG_RPD			0x9		//	Received power detector
#define REG_RX_ADDR_P0	0xA		//	Receive address	of data pipe 0
#define REG_RX_ADDR_P1	0xB		//	Receive address	of data pipe 1
#define REG_RX_ADDR_P2	0xC		//	Receive address of data pipe 2
#define REG_RX_ADDR_P3	0xD		//	Receive address of data pipe 3
#define REG_RX_ADDR_P4	0xE		//	Receive address of data pipe 4
#define REG_RX_ADDR_P5	0xF		//	Receive address of data pipe 5
#define REG_TX_ADDR		0x10	//	Transmit address
#define REG_RX_PW_P0 	0x11	//	Number of bytes in rx payload of data pipe 0	
#define REG_RX_PW_P1 	0x12	//	Number of bytes in rx payload of data pipe 1
#define REG_RX_PW_P2 	0x13	//	Number of bytes in rx payload of data pipe 2
#define REG_RX_PW_P3 	0x14	//	Number of bytes in rx payload of data pipe 3
#define REG_RX_PW_P4 	0x15	//	Number of bytes in rx payload of data pipe 4
#define REG_RX_PW_P5 	0x16	//	Number of bytes in rx payload of data pipe 5
#define REG_FIFO_STATUS	0x17 	//	FIFO status register
#define	REG_DYNPD		0x1C	//	Enable dynamic payload length
#define REG_FEATURE		0x1D	//	Feature register

//	Register mnemonics
#define REG_CONFIG_MASK_RX_DR  6
#define REG_CONFIG_MASK_TX_DS  5
#define REG_CONFIG_MASK_MAX_RT 4
#define REG_CONFIG_EN_CRC 	   3
#define REG_CONFIG_CRCO 	   2
#define REG_CONFIG_PWR_UP 	   1
#define REG_CONFIG_PRIM_RX 	   0

#define REG_RF_SETUP_RF_DR_LOW 5
#define REG_RF_SETUP_RF_DR_HIGH 3

#define REG_FEATURE_EN_DPL 2

//	NRF2401+ commands
static void readRegister(uint8_t reg, uint8_t* const data)
{
}

static void writeRegister(uint8_t reg, const uint8_t* const data, uint8_t len)
{
}

static void readPayload(uint8_t* const data)
{
}

static void writePayload(const uint8_t* const data, uint8_t len)
{
}

static void flushTxFifo()
{
}

static void flushRxFifo()
{
}


//	Global functions implementation
void Nrf_Init(const Nrf_GlobalOptions* const options)
{
	if (!options)
		return;
	uint8_t registers[REG_FEATURE+1] = {0};
	if (options->receive_settings)
	{
		const Nrf_ReceiveOptions* const receive_options = &options->receive_settings;
		const int pipes_count = receive_options->pipes_count;
		for (int i = 0; i < pipes_count; ++i)
		{
			registers[REG_EN_RXADDR] |= 1 << i;
			registers[REG_ENAA] |= receive_options->data_pipes[i].auto_ack << i;
		}
		registers[REG_SETUP_AW] = receive_options->address_width;
		int dynamic_payload = 0;
		for (int i = 0; i < pipes_count; ++i)
		{
			if (receive_options->data_pipes[i].payload_size < 32)
			{
				dynamic_payload = 1;
				registers[REG_RX_PW_P0 +i] = receive_options->data_pipes[i].payload_size;
				registers[REG_DYNPD] |= 1 << i;
			}
			else
			{
				registers[REG_RX_PW_P0 +i] = 32;
			}
			if (i == 0 || i == 1)
			{
				uint8_t addr[5];
				addr[0] = receive_options->data_pipes[i].addr_postfix;
				addr[1] = receive_options->addr_prefix[0];
				addr[2] = receive_options->addr_prefix[1];
				addr[3] = receive_options->addr_prefix[2];
				addr[4] = receive_options->addr_prefix[3];
				registers[REG_RX_ADDR_P0 + i] = 0;	//	TODO: (alex) use here sending
				continue;
			}
			registers[REG_RX_ADDR_P0 + i] = receive_options->data_pipes[i].addr_postfix;	//	TODO: (alex) use here sending
		}
		registers[REG_FEATURE] |= TO_INT(dynamic_payload) << REG_FEATURE_EN_DPL;
	}
	{
		registers[REG_CONFIG] |= TO_INT((options->interrupt_mask & NRF_INTERRUPT_RX) != NRF_INTERRUPT_RX) << REG_CONFIG_MASK_RX_DR;
		registers[REG_CONFIG] |= TO_INT((options->interrupt_mask & NRF_INTERRUPT_TX) != NRF_INTERRUPT_TX) << REG_CONFIG_MASK_TX_DS;
		registers[REG_CONFIG] |= TO_INT((options->interrupt_mask & NRF_INTERRUPT_MAX_RT) != NRF_INTERRUPT_MAX_RT) << REG_CONFIG_MASK_MAX_RT;
		registers[REG_CONFIG] |= TO_INT(options->cnc != NRF_CNC_NONE || registers[REG_ENAA] != 0x0) << REG_CONFIG_EN_CRC;
		registers[REG_CONFIG] |= TO_INT(options->cnc == NRF_CNC_2BYTE) << REG_CONFIG_CRCO;
		registers[REG_CONFIG] |= 1 << REG_CONFIG_PWR_UP;
	}
	registers[REG_SETUP_RETR] = 0x3; // Recet value
	registers[REG_RF_CH] = options->rf_channel & 0x7f;
	if (options->data_rate == NRF_250KBPS)
	{
		registers[REG_RF_SETUP] |= 1 << REG_RF_SETUP_RF_DR_LOW;
	}
	else
	{
		registers[REG_RF_SETUP] |= TO_INT(options->data_rate == NRF_2MPBS) << REG_RF_SETUP_RF_DR_LOW;
	}
	registers[REG_RF_SETUP] |= options->transmit_options ? options->transmit_options.power : 0x3;
}
