#ifndef 	I2C_H
#define 	I2C_H

#include <stm32f401xc.h>


typedef enum {
	I2C_STATE_RESET	 = 0x00, 
	I2C_STATE_READY = 0x01, 
	I2C_STATE_BUSY = 0x02, 
	I2C_STATE_BUSYTX = 0x03, 
	I2C_STATE_BUSYRX = 0x04, 
	I2C_STATE_ERROR = 0x05 
} i2c_state_t;

typedef struct {
	uint32_t clockSpeed; 			//clock frequency
	uint32_t AddressingMode;		//7bit or 10 bits
	uint32_t NoStretechMode;		//clock stretching enable or disable
} I2C_Init_t;

typedef struct {
	I2C_TypeDef		*instance;			//i2c instance used
	uint8_t			*buffer;			//transfer buffer
	uint32_t 		bufferSize;
	I2C_Init_t		init;				//init struct parameters
	i2c_state_t 	state;
} I2C_Handle_t;

#ifdef __cplusplus
extern "C" {
#endif                                      
	void i2c_master_rx(I2C_Handle_t *, uint8_t slave_address, uint8_t reg_addr,  uint8_t *buffer, uint32_t size);
	void i2c_master_tx(I2C_Handle_t *, uint8_t slave_address, uint8_t reg_addr,  uint8_t *buffer, uint32_t size);
	void I2CInit(I2C_Handle_t*);
#ifdef __cplusplus
}
#endif                                      

#endif                                      