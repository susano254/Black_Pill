#ifndef 	NRF24_H
#define 	NRF24_H

#include <System.h>
#include <RCC.h>
#include <USART.h>
#include <SPI.h>
#include <GPIO.h>


//memory map
#define CONFIG      0x00
#define EN_AA       0x01
#define EN_RXADDR   0x02
#define SETUP_AW    0x03
#define SETUP_RETR  0x04
#define RF_CH       0x05
#define RF_SETUP    0x06
#define STATUS      0x07
#define OBSERVE_TX  0x08
#define CD          0x09
#define RX_ADDR_P0  0x0A
#define RX_ADDR_P1  0x0B
#define RX_ADDR_P2  0x0C
#define RX_ADDR_P3  0x0D
#define RX_ADDR_P4  0x0E
#define RX_ADDR_P5  0x0F
#define TX_ADDR     0x10
#define RX_PW_P0    0x11
#define RX_PW_P1    0x12
#define RX_PW_P2    0x13
#define RX_PW_P3    0x14
#define RX_PW_P4    0x15
#define RX_PW_P5    0x16
#define FIFO_STATUS 0x17
#define DYNPD	    0x1C
#define FEATURE	    0x1D

/* Bit Mnemonics */


//CONFIG register bits
#define PWR_OFF		0x00
#define MASK_RX_DR  (1 << 6)
#define MASK_TX_DS  (1 << 5)
#define MASK_MAX_RT (1 << 4)
#define EN_CRC      (1 << 3)
#define CRCO        (1 << 2)
#define PWR_UP      (1 << 1)
#define PRIM_RX     (1 << 0)


//EN_AA Register bits
#define ENAA_P5     (1 << 5)
#define ENAA_P4     (1 << 4)
#define ENAA_P3     (1 << 3)
#define ENAA_P2     (1 << 2)
#define ENAA_P1     (1 << 1)
#define ENAA_P0     (1 << 0)


//EN_RXADDR Register bits
#define ERX_P5      (1 << 5)
#define ERX_P4      (1 << 4)
#define ERX_P3      (1 << 3)
#define ERX_P2      (1 << 2)
#define ERX_P1      (1 << 1)
#define ERX_P0      (1 << 0)


//Setup_AW (address width) register bits
#define AW          (1 << 0)


//SETUP RETR register bits
#define ARD         (1 << 4)
#define ARC         (1 << 0)

//RF_SETUP register bits
#define CONT_WAVE	(1 << 7)
#define RF_DR_LOW	(1 << 5)
#define PLL_LOCK    (1 << 4)
#define RF_DR_HIGH  (1 << 3)
#define RF_PWR      (1 << 1)



//Status register bits
#define RX_DR       (1 << 6)
#define TX_DS       (1 << 5)
#define MAX_RT      (1 << 4)
#define RX_P_NO     (1)
#define TX_FULL     (1 << 0)

//OBSERVE_TX register bits
#define PLOS_CNT    (1 << 4)
#define ARC_CNT     (1 << 0)

//FIFO_STATUS Register bits
#define TX_REUSE    (1 << 6)
#define FIFO_FULL   (1 << 5)
#define TX_EMPTY    (1 << 4)
#define RX_FULL     (1 << 1)
#define RX_EMPTY    (1 << 0)

//DYNPD (Dynamic payload) register bits
#define DPL_P5	    (1 << 6)
#define DPL_P4	    (1 << 4)
#define DPL_P3	    (1 << 3)
#define DPL_P2	    (1 << 2)
#define DPL_P1	    (1 << 1)
#define DPL_P0	    (1 << 0)

//FEATURE Register bits
#define EN_DPL	    (1 << 2)
#define EN_ACK_PAY  (1 << 1)
#define EN_DYN_ACK  (1 << 0)

/* Instruction Mnemonics */
#define R_REGISTER    0x00
#define W_REGISTER    0x20
#define REGISTER_MASK 0x1F
#define ACTIVATE      0x50
#define R_RX_PL_WID   0x60
#define R_RX_PAYLOAD  0x61
#define W_TX_PAYLOAD  0xA0
#define W_ACK_PAYLOAD 0xA8
#define FLUSH_TX      0xE1
#define FLUSH_RX      0xE2
#define REUSE_TX_PL   0xE3
#define NOP           0xFF

/* Non-P omissions */
#define LNA_HCURR   0

/* P model memory Map */
#define RPD         0x09

///* P model bit Mnemonics */
//#define RF_DR_LOW   5
//#define RF_DR_HIGH  3
//#define RF_PWR_LOW  1
//#define RF_PWR_HIGH 2

#define CSN_PIN 	PB12
#define CE_PIN 		PB13

struct NRF24 {
	uint8_t status;
	uint8_t buffer[100];
	const uint8_t defaultAddr[6] = {(uint8_t)0xe7, (uint8_t)0xe7, (uint8_t)0xe7, (uint8_t)0xe7, (uint8_t)0xe7};

	void init(uint8_t, uint8_t);
	void writeReg(uint8_t, uint8_t);
	void writeMultiReg(uint8_t, const uint8_t* , uint8_t);
	uint8_t readReg(uint8_t);
	void readMultiReg(uint8_t, uint8_t*, uint8_t);
	uint8_t isDataAvailable(uint8_t pipeNum);
	void setRxMode(const uint8_t *addr, uint8_t channel);
	void setTxMode(const uint8_t *addr, uint8_t channel);
	void receive();
	void transmit(uint8_t *data, int size);

	void sendCmd(uint8_t cmd);
	void CE_Enable();
	void CE_Disable();
	void CS_Select();
	void CS_UnSelect();

	void printData();
};

// #ifdef __cplusplus
// extern "C" {
// #endif                                      
// 	void NRF24Init(uint8_t, uint8_t);
// 	void NRF24WriteReg(uint8_t, uint8_t);
// 	void NRF24WriteMultiReg(uint8_t, uint8_t* , uint8_t);
// 	uint8_t NRF24ReadReg(uint8_t);
// 	void NRF24ReadMultiReg(uint8_t, uint8_t*, uint8_t);
// 	uint8_t isDataAvailable(uint8_t pipeNum);
// 	void NRF24RxMode(uint8_t *addr, uint8_t channel);
// 	void NRF24TxMode(uint8_t *addr, uint8_t channel);
// 	void receive(uint8_t*);
// 	void transmit(uint8_t*, int);

// 	void sendCmd(uint8_t);
// 	void CE_Enable();
// 	void CE_Disable();
// 	void CS_Select();
// 	void CS_UnSelect();
// #ifdef __cplusplus
// }
// #endif                                      

#endif