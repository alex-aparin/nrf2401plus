#include "nrf2401p.h"

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


#define WRITE_REGISTER(reg, data, len) \ 
send_command(COMMAND_R_REGISTER | reg, data, len);

static uint8_t command_buffer[33];

static void write_register(uint8_t const register, uint8_t value)
{
	command_buffer[0] = register | COMMAND_R_REGISTER;
	command_buffer[1] = value;
	Nrf_WriteSpi(command_buffer, 2);
}

static void write_register_ptr(uint8_t const register, uint8_t* data, uint8_t len)
{
	command_buffer[0] = register | COMMAND_R_REGISTER;
	for (int i = 0; i < len; ++i)
	{
		command_buffer[i + 1] = data[i];
	}
	Nrf_WriteSpi(command_buffer, len + 1);
}

static void send_command(uint8_t const command, uint8_t* data, uint8_t len)
{
	buffer[0] = command;
	
}

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
		{
			uint8_t enable_pipes_reg = 0;
			uint8_t enable_ack_reg = 0;
			for (int i = 0; i < pipes_count; ++i)
			{
				enable_pipes_reg |= 1 << i;
				enable_ack_reg |= receive_options->data_pipes[i].auto_ack << i;
			}
			write_register(REG_EN_RXADDR, enable_pipes_reg);
			write_register(REG_ENAA, enable_ack_reg);
		}
		write_register(REG_SETUP_AW, receive_options->address_width);
		int dynamic_payload = 0;
		uint8_t payload_sizes_reg = 0;
		for (int i = 0; i < pipes_count; ++i)
		{
			if (receive_options->data_pipes[i].payload_size < 32)
			{
				dynamic_payload = 1;
				write_register_ptr(REG_RX_PW_P0 +i, receive_options->data_pipes[i].payload_size);
				payload_sizes_reg |= 1 << i;
			}
			else
			{
				write_register_ptr(REG_RX_PW_P0 +i, 32);
			}
			if (i == 0 || i == 1)
			{
				uint8_t addr[5];
				addr[0] = receive_options->data_pipes[i].addr_postfix;
				addr[1] = receive_options->addr_prefix[0];
				addr[2] = receive_options->addr_prefix[1];
				addr[3] = receive_options->addr_prefix[2];
				addr[4] = receive_options->addr_prefix[3];
				write_register_ptr(REG_RX_ADDR_P0 + i, addr, 5);
				continue;
			}
			write_register(REG_RX_ADDR_P0 + i, receive_options->data_pipes[i].addr_postfix);
		}
		write_register(REG_DYNPD, payload_sizes_reg);
		write_register(REG_FEATURE, TO_INT(dynamic_payload) << REG_FEATURE_EN_DPL);
	}

	{
		uint8_t config_reg = 0x0;
		config_reg |= TO_INT((options->interrupt_mask & NRF_INTERRUPT_RX) != NRF_INTERRUPT_RX) << REG_CONFIG_MASK_RX_DR;
		config_reg |= TO_INT((options->interrupt_mask & NRF_INTERRUPT_TX) != NRF_INTERRUPT_TX) << REG_CONFIG_MASK_TX_DS;
		config_reg |= TO_INT((options->interrupt_mask & NRF_INTERRUPT_MAX_RT) != NRF_INTERRUPT_MAX_RT) << REG_CONFIG_MASK_MAX_RT;
		config_reg |= TO_INT(options->cnc != NRF_CNC_NONE || registers[REG_ENAA] != 0x0) << REG_CONFIG_EN_CRC;
		config_reg |= TO_INT(options->cnc == NRF_CNC_2BYTE) << REG_CONFIG_CRCO;
		config_reg |= 1 << REG_CONFIG_PWR_UP;
		write_register(REG_CONFIG, config_reg)
	}
	write_register(REG_RF_CH, options->rf_channel & 0x7f);
	{
		uint8_t rf_setup_register = options->transmit_options ? options->transmit_options.power : 0x3; 
		if (options->data_rate == NRF_250KBPS)
		{
			rf_setup_register |= 1 << REG_RF_SETUP_RF_DR_LOW;
		}
		else
		{
			rf_setup_register |= TO_INT(options->data_rate == NRF_2MPBS) << REG_RF_SETUP_RF_DR_LOW;
		}
		write_register(REG_RF_SETUP, rf_setup_register);
	}
}
