#ifndef 	USART_H
#define 	USART_H

#include <stm32f401xc.h>
#include <string>

using namespace std;


#define HAL_USART_ERROR_NONE		((uint32_t) 0x00)	//No Error
#define HAL_USART_ERROR_PE			((uint32_t) 0x01)	//Parity Error
#define HAL_USART_ERROR_NE			((uint32_t) 0x02)	//Noise error
#define HAL_USART_ERROR_FE			((uint32_t) 0x04)	//frame error
#define HAL_USART_ERROR_ORE			((uint32_t) 0x08)	//Overrun error
#define HAL_USART_ERROR_DMA			((uint32_t) 0x10)	//DMA transfer error

#define WL_7		0x07
#define	WL_8		0x08
#define	WL_9		0x09

//typedef enum {
//	STOP_BITS_HALF 		= 0x00,
//	STOP_BITS_ONE 		= 0x01,
//	STOP_BITS_TWO 		= 0x02,
//	STOP_BITS_ONE_HALF 	= 0x12,
//} stop_bits_t;

#define USART_OVERSAMPLING_16		0x00
#define USART_OVERSAMPLING_8		0x01

#define USART_STOP_BITS_HALF		0x00
#define USART_STOP_BITS_ONE			0x01
#define USART_STOP_BITS_TWO			0x02
#define USART_STOP_BITS_ONE_HALF	0x03

typedef enum {
	HAL_USART_STATE_RESET 		= 0x00,
	HAL_USART_STATE_READY 		= 0x01,
	HAL_USART_STATE_BUSY 		= 0x02,
	HAL_USART_STATE_BUSY_TX 	= 0x12,
	HAL_USART_STATE_BUSY_RX 	= 0x22,
	HAL_USART_STATE_BUSY_TX_RX 	= 0x32
} hal_usart_state_t;

typedef struct {
	uint32_t baudRate;
	uint32_t wordLength;
	uint32_t stopBits;
	uint32_t parity;
	uint32_t mode;
	uint32_t oversampling;
} usart_init_t;

typedef void (tx_callback_t)(void *ptr);
typedef void (rx_callback_t)(void *ptr);

typedef struct {
	USART_TypeDef		*instance;
	usart_init_t		init;
	uint8_t				*pTxBuffer;
	uint16_t			txSize;
	uint16_t			txCount;
	uint8_t				*pRxBuffer;
	uint16_t			rxSize;
	uint16_t			rxCount;
	hal_usart_state_t	tx_state;
	hal_usart_state_t	rx_state;
	uint32_t			errorCode;
	tx_callback_t		*tx_callback;
	rx_callback_t		*rx_callback;
} usart_handle_t;


struct VirtualPort {
	int stringIndex = 0;
	string buffer;
	bool RXNE = false;

	void begin(int baudRate);
	void sendByte(uint8_t byte);
	void send(uint8_t *pData, uint16_t size);
	void printStrln(char *str);
	void printStr(char *str);
	int length(char *str);
	void intToStr(int num, char *str);
	void floatToStr(float num, char *str);
	string floatToStr(float num);
	void printInt(int num);
	void printFloat(float num);
	bool isAvailable();
	string recieve();
};

void USART_EnableInterrupt(USART_TypeDef * USARTx, uint8_t interruptNumber);

struct USARTx {
	void hal_usart_init(usart_handle_t *handle);
	void hal_usart_tx(usart_handle_t *handle, uint8_t *buffer, uint32_t len);
	void hal_usart_rx(usart_handle_t *handle, uint8_t *buffer, uint32_t len);
	void hal_usart_handle_interrupt(usart_handle_t *handle);

	private:

	void hal_usart_enable(USART_TypeDef *USARTx);
	void hal_usart_disable(USART_TypeDef *USARTx);
	void hal_usart_tx_enable(USART_TypeDef *USARTx);
	void hal_usart_tx_disable(USART_TypeDef *USARTx);
	void hal_usart_rx_enable(USART_TypeDef *USARTx);
	void hal_usart_rx_disable(USART_TypeDef *USARTx);
	void hal_usart_error_interrupt_enable(USART_TypeDef *USARTx);
	void hal_usart_error_interrupt_disable(USART_TypeDef *USARTx);
	void hal_usart_configure_world_length(USART_TypeDef *USARTx, uint8_t worldLength);
	void hal_usart_configure_stop_bits(USART_TypeDef *USARTx, uint8_t nstop);
	void hal_usart_configure_over_sampling(USART_TypeDef *USARTx, uint8_t over8);
};


namespace global {
	extern VirtualPort Serial;
}

// #ifdef __cplusplus
// extern "C" {
// #endif
// 	void hal_usart_init(usart_handle_t *handle);
// 	void hal_usart_tx(usart_handle_t *handle, uint8_t *buffer, uint32_t len);
// 	void hal_usart_rx(usart_handle_t *handle, uint8_t *buffer, uint32_t len);
// 	void hal_usart_handle_interrupt(usart_handle_t *handle);

// 	void USART2Init(int baudRate, const uint8_t clock);
// 	void USART_EnableInterrupt(USART_TypeDef * USARTx, uint8_t interruptNumber);
// 	void USART2SendByte(uint8_t byte);
// 	void USART2Send(uint8_t *pData, uint16_t size);
// 	void printStrln(char *str);
// 	void printStr(char *str);
// 	int length(char *str);
// 	void intToStr(int num, char *str);
// 	void floatToStr(float num, char *str);
// 	void printInt(int num);
// 	void printFloat(float num);
// #ifdef __cplusplus
// }
// #endif


#endif