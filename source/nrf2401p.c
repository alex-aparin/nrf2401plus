#include "nrf2401p.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define TO_INT(x) ((x) ? 1 : 0)

//	Commands
#define COMMAND_R_REGISTER 0x0
#define COMMAND_W_REGISTER 0x20
#define COMMAND_R_RX_PAYLOAD 0x61
#define COMMAND_W_TX_PAYLOAD 0xa0
#define COMMAND_FLUSH_TX	0xe1
#define COMMAND_FLUSH_RX	0xe2
#define COMMAND_REUSE_TX_PL	0xe3
#define COMMAND_R_RX_PL_WID	0x60
#define COMMAND_W_ACK_PAYLOAD	0xa8
#define COMMAND_W_TX_PAYLOAD_NOACK	0xb0
#define COMMAND_NOP	0xff

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

#define COMMAND_SIZE 1
#define COMMAND_DATA_MAX_SIZE 5
#define PAYLOAD_MAX_SIZE 32
#define STATUS_SIZE 1

//	NRF2401+ commands
static void writeRegister(const Nrf_Byte reg, const Nrf_Byte value)
{
	Nrf_Select(NRF_SELECT);
	Nrf_WriteSpi(reg | COMMAND_W_REGISTER);
	Nrf_WriteSpi(value);
	Nrf_Select(NRF_UNSELECT);
}

static void writeRegisterPtr(const Nrf_Byte reg, Nrf_Byte* const data, const Nrf_Byte len)
{
	Nrf_Select(NRF_SELECT);
	Nrf_WriteSpi(reg | COMMAND_W_REGISTER);
	for (int i = 0; i < len; ++i)
	{
		Nrf_WriteSpi(data[i]);
	}
	Nrf_Select(NRF_UNSELECT);
}

static Nrf_Byte readRegister(const Nrf_Byte reg, Nrf_Byte* const data, const Nrf_Byte len)
{
	Nrf_Select(NRF_SELECT);
	const Nrf_Byte status = Nrf_WriteSpi(reg | COMMAND_R_REGISTER);
	for (int i = 0; i < len; ++i)
	{
		data[i] = Nrf_WriteSpi(COMMAND_NOP);
	}
	Nrf_Select(NRF_UNSELECT);
	return status;
}

static void readPayload(Nrf_Byte* const data, const Nrf_Byte len)
{
	Nrf_Select(NRF_SELECT);
	const Nrf_Byte status = Nrf_WriteSpi(COMMAND_R_RX_PAYLOAD);
	for (int i = 0; i < len; ++i)
	{
		data[i] = Nrf_WriteSpi(COMMAND_NOP);
	}
	Nrf_Select(NRF_UNSELECT);
}

static void writePayload(Nrf_Byte* const data, const Nrf_Byte len)
{
	Nrf_Select(NRF_SELECT);
	const Nrf_Byte status = Nrf_WriteSpi(COMMAND_W_TX_PAYLOAD);
	for (int i = 0; i < len; ++i)
	{
		Nrf_WriteSpi(data[i]);
	}
	Nrf_Select(NRF_UNSELECT);
}

static void flushTxFifo()
{
	Nrf_Select(NRF_SELECT);
	Nrf_WriteSpi(COMMAND_FLUSH_TX);
	Nrf_Select(NRF_UNSELECT);
}

static void flushRxFifo()
{
	Nrf_Select(NRF_SELECT);
	Nrf_WriteSpi(COMMAND_FLUSH_RX);
	Nrf_Select(NRF_UNSELECT);
}

//	Global functions implementation
void Nrf_Init(const Nrf_GlobalOptions* const options)
{
	if (!options)
		return;
	write_register(REG_SETUP_AW, options->address_width);
	write_register(REG_RF_CH, options->rf_channel & 0x7f);
	write_register(REG_RF_SETUP, options->power | 
		(options->data_rate == NRF_250KBPS ? 1 << REG_RF_SETUP_RF_DR_LOW : TO_INT(options->data_rate == NRF_2MPBS) << REG_RF_SETUP_RF_DR_LOW));
	write_register(REG_CONFIG, 
		TO_INT((options->interrupt_mask & NRF_INTERRUPT_RX) != NRF_INTERRUPT_RX) << REG_CONFIG_MASK_RX_DR | 
		TO_INT((options->interrupt_mask & NRF_INTERRUPT_TX) != NRF_INTERRUPT_TX) << REG_CONFIG_MASK_TX_DS | 
		TO_INT((options->interrupt_mask & NRF_INTERRUPT_MAX_RT) != NRF_INTERRUPT_MAX_RT) << REG_CONFIG_MASK_MAX_RT | 
		TO_INT(options->cnc != NRF_CNC_NONE || registers[REG_ENAA] != 0x0) << REG_CONFIG_EN_CRC | 
		TO_INT(options->cnc == NRF_CNC_2BYTE) << REG_CONFIG_CRCO | 
		1 << REG_CONFIG_PWR_UP);
}

void Nrf_AddPipe(const Nrf_DataPipeOptions* const pipe_options)
{
	if (!pipe_options)
		return;
	Nrf_Byte pipes_reg = 0xff;
	Nrf_Byte enabled_ack_reg = 0x0;
	Nrf_Byte dyn_payloads_reg = 0x0;
	Nrf_Byte feature_reg = 0x0;
	Nrf_Byte addr_width = 0x0;
	readRegister(REG_EN_RXADDR, &pipes_reg, 1);
	readRegister(REG_ENAA, &enabled_ack_reg, 1);
	readRegister(REG_DYNPD, &dyn_payloads_reg, 1);
	readRegister(REG_FEATURE, &feature_reg, 1);
	readRegister(REG_SETUP_AW, &addr_width, 1);
	for (Nrf_Byte i = 0; i < 6; ++i)
	{
		if ((pipes_reg & (1 << i)) == 0x0)
		{
			const Nrf_Byte dynamic_payload_flag = TO_INT(pipe_options->payload_size < 32);
			pipes_reg |= 1 << i;
			enabled_ack_reg &= ~(1 << i);
			enabled_ack_reg |= pipe_options->auto_ack << i;
			dyn_payloads_reg &= ~(1 << i);
			dyn_payloads_reg |=  dynamic_payload_flag << i;
			feature_reg |= dynamic_payload_flag << REG_FEATURE_EN_DPL;
			write_register(REG_EN_RXADDR, pipes_reg);
			write_register(REG_ENAA, pipes_reg);
			write_register(REG_RX_PW_P0 + i, MIN(32, pipe_options->payload_size));
			write_register(REG_DYNPD, dyn_payloads_reg);
			write_register(REG_FEATURE, feature_reg);
			if (i > 1)
			{
				write_register(REG_RX_ADDR_P0 + i, pipe_options->addr_postfix);
			}
			else
			{
				Nrf_Byte addr[5];
				readRegister(REG_RX_ADDR_P0 + i, addr, 5);
				addr[0] = pipe_options->addr_postfix;
				write_register(REG_RX_ADDR_P0 + i, addr, addr_width + 2);
			}
			break;
		}
	}
}
