#ifndef 	SPI_H
#define 	SPI_H

#include <stm32f401xc.h>

#ifdef __cplusplus
extern "C" {
#endif                                      
	void SPIInit(uint8_t);
	uint8_t SPITransfer(uint8_t);
#ifdef __cplusplus
}
#endif                                      

#endif