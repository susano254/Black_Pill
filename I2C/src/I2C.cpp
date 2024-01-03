#include <GPIO.h>
#include <RCC.h>
#include <I2C.h>
#include <USART.h>


/**********************more values*********************/
#define I2C_ADDRMODE_7BIT		((uint32_t) 7)
#define I2C_ADDRMODE_10BIT		((uint32_t) 10)
#define I2C_NOSTRETCH			0x1
#define I2C_STRETCH				0x0
#define I2C_PERIPHERAL_MIN_FREQ				2
#define I2C_PERIPHERAL_MAX_FREQ				42
#define I2C_ACK						1
#define I2C_NACK					0
#define READ						1
#define WRITE						0


void i2c_reset(I2C_TypeDef* I2Cx){
	I2Cx->CR1 |= I2C_CR1_SWRST;
	for(int i = 0; i < 1000; i++);
	I2Cx->CR1 &= ~I2C_CR1_SWRST;
}
void i2c_start_gen(I2C_TypeDef* I2Cx){
	//generate the start condition 
	I2Cx->CR1 |= I2C_CR1_START;
	//wait for success
	while (I2Cx->CR1 & I2C_CR1_START);

	//poll for the SB bit to be set
	while(!(I2Cx->SR1 & I2C_SR1_SB));
}

void i2c_poll_stop_condition(I2C_TypeDef* I2Cx){
	//wait for success
	while (I2Cx->CR1 & I2C_CR1_STOP);
}

void i2c_stop_gen(I2C_TypeDef* I2Cx){
	uint16_t temp;
	//generate the start condition 
	I2Cx->CR1 |= I2C_CR1_STOP;
}

void i2c_enable_peripheral(I2C_TypeDef* I2Cx){
	I2Cx->CR1 |= I2C_CR1_PE;
}
void i2c_disable_peripheral(I2C_TypeDef* I2Cx){
	I2Cx->CR1 &= ~I2C_CR1_PE;
}

void i2c_manage_clock_stretch(I2C_TypeDef* I2Cx, uint32_t no_stretch){
	if(no_stretch)
		I2Cx->CR1 |= I2C_CR1_NOSTRETCH;
	else
		I2Cx->CR1 &= ~I2C_CR1_NOSTRETCH;
}
void i2c_set_addressing_mode(I2C_TypeDef* I2Cx, uint32_t addressing_mode){
	if(addressing_mode == I2C_ADDRMODE_7BIT)
		I2Cx->OAR1 &= ~I2C_OAR1_ADDMODE;
	else
		I2Cx->OAR1 |= I2C_OAR1_ADDMODE;
}

void i2c_set_frequency(I2C_TypeDef* I2Cx,  uint32_t frequency){
	if(frequency > I2C_PERIPHERAL_MIN_FREQ && frequency < I2C_PERIPHERAL_MAX_FREQ){
		//clear those bits first
		I2Cx->CR2 &= ~I2C_CR2_FREQ;
		//set the new value
		I2Cx->CR2 |= frequency;
	}
}

void i2c_set_clock_control_reg(I2C_TypeDef* I2Cx, uint32_t value) {
	//clear those bits first
	I2Cx->CCR = 0;

	if(value > 0 && value < 4096){
		I2Cx->CCR |= value;
	}
}

void i2c_set_trise(I2C_TypeDef* I2Cx, uint32_t value) {
	I2Cx->TRISE = 0;
	if(value > 0 && value < 64){
		I2Cx->TRISE |= value;
	}
}

void i2c_set_ack(I2C_TypeDef* I2Cx){
	I2Cx->CR1 |= I2C_CR1_ACK;
}

void i2c_set_nack(I2C_TypeDef* I2Cx){
	I2Cx->CR1 &= ~I2C_CR1_ACK;
}

void i2c_clear_ADDR_bit(I2C_TypeDef* I2Cx){
	//followed by read of SR2
	uint32_t temp = I2Cx->SR1 | I2Cx->SR2;
}

void i2c_send_slave_address(I2C_TypeDef* I2Cx, uint8_t slave_address, uint8_t RD_W){
	//write the slave address
	uint8_t Data = slave_address << 1;

	// transmit (write) mode LSB is reset, Receive (read) mode LSB is set
	if(RD_W)
		Data |= (1 << 0);
	else
		Data &= ~(1 << 0);

	I2Cx->DR = Data;
	//reading of SR1
	while(!(I2Cx->SR1 & I2C_SR1_ADDR));
}

void i2c_send_byte(I2C_TypeDef* I2Cx, uint8_t byte){
	//wait for the TXE flag 
	while(!(I2Cx->SR1 & I2C_SR1_TXE));	
	//write the data in TXDR to be sent
	I2Cx->DR = byte;
	//wait for Byte transfer finished
	while(!(I2Cx->SR1 & I2C_SR1_BTF));	
}

uint8_t i2c_read_byte(I2C_TypeDef* I2Cx){
	//wait for the TXE flag 
	while(!(I2Cx->SR1 & I2C_SR1_RXNE));	
	//write the data in TXDR to be sent
	return I2Cx->DR;
}

void I2CInit(I2C_Handle_t* handle){
	i2c_reset(handle->instance);

	//disable peripheral before going further
	i2c_disable_peripheral(handle->instance);

	//perihperal frequency to 8 Mhz
	i2c_set_frequency(handle->instance, 8);

	//configure the clock control register 
	//for a standard 100 Khz mode a 5 us low and 5 us high hold is needed with frequency = 8 Mhz CCR needs to be 40
	i2c_set_clock_control_reg(handle->instance, 40);

	//configure maximum TRISE for the feedback of the SCL
	i2c_set_trise(handle->instance, 9);

	//configure clock stretch
	i2c_manage_clock_stretch(handle->instance, I2C_STRETCH);

	i2c_set_addressing_mode(handle->instance, I2C_ADDRMODE_7BIT);

	//finally enable peripheral
	i2c_enable_peripheral(handle->instance);

	//wait for a while till the device wakes
	for(int i = 0; i < 1000; i++);
}


void i2c_master_tx(I2C_Handle_t *handle, uint8_t slave_address, uint8_t regAddr, uint8_t *buffer, uint32_t size){
	i2c_set_ack(handle->instance);

	//generate a start condition
	i2c_start_gen(handle->instance);

	i2c_send_slave_address(handle->instance, slave_address, WRITE);
	i2c_clear_ADDR_bit(handle->instance);

	//first write  regAddr
	i2c_send_byte(handle->instance, regAddr);

	//then write data to it 
	for(int i = 0; i < size; i++){
		i2c_send_byte(handle->instance, buffer[i]);
	}

	//generate the stop condition and terminate communication
	i2c_stop_gen(handle->instance);
	i2c_poll_stop_condition(handle->instance);
}


void i2c_master_rx(I2C_Handle_t *handle, uint8_t slave_address, uint8_t regAddr, uint8_t *buffer, uint32_t size){
	//generate the start condition
	i2c_start_gen(handle->instance);

	i2c_send_slave_address(handle->instance, slave_address, WRITE);
	i2c_clear_ADDR_bit(handle->instance);

	//first write  regAddr
	i2c_send_byte(handle->instance, regAddr);

	//then read
	if(size == 1) {
		i2c_start_gen(handle->instance);

		i2c_send_slave_address(handle->instance, slave_address, READ);
		i2c_set_nack(handle->instance);
		i2c_clear_ADDR_bit(handle->instance);
		i2c_stop_gen(handle->instance);
		buffer[0] = i2c_read_byte(handle->instance);
	}
	else {
		i2c_set_ack(handle->instance);
		i2c_start_gen(handle->instance);

		i2c_send_slave_address(handle->instance, slave_address, READ);
		i2c_clear_ADDR_bit(handle->instance);

		int i;
		for(i = 0; i < size-2; i++){
			buffer[i] = i2c_read_byte(handle->instance);
			i2c_set_ack(handle->instance);
		}
		buffer[i] = i2c_read_byte(handle->instance);
		i2c_set_nack(handle->instance);
		i2c_stop_gen(handle->instance);
		i++;
		buffer[i] = i2c_read_byte(handle->instance);
	}

	i2c_poll_stop_condition(handle->instance);
}




// void i2c_master_rx(I2C_Handle_t *handle, uint8_t slave_address, uint8_t regAddr, uint8_t *buffer, uint32_t size){
// 	//generate the start condition
// 	i2c_start_gen(handle->instance);

// 	i2c_send_slave_address(handle->instance, slave_address, WRITE);
// 	i2c_clear_ADDR_bit(handle->instance);

// 	//first write  regAddr
// 	i2c_send_byte(handle->instance, regAddr);

// 	//then read
// 	i2c_start_gen(handle->instance);

// 	i2c_send_slave_address(handle->instance, slave_address, READ);

// 	if(size == 1) 
// 		i2c_set_nack(handle->instance);

// 	i2c_clear_ADDR_bit(handle->instance);

// 	for(int i = 0; i < size; i++){
// 		if(i == size-1){
// 			if(size != 1){
// 				i2c_set_nack(handle->instance);
// 			}
// 			i2c_stop_gen(handle->instance);
// 		}

// 		//read  the data
// 		buffer[i] = i2c_read_byte(handle->instance);

// 		if(i < size - 2 && size != 1)
// 			i2c_set_ack(handle->instance);
// 	}
// 	i2c_poll_stop_condition(handle->instance);
// }