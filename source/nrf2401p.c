#include "nrf2401p.h"

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


//	Global functions implementation
void Nrf_Init(const Nrf_GlobalOptions* const options)
{
	if (options == NULL)
		return;
	
}
