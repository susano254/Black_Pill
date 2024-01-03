#include <SPI.h>
#include <GPIO.h>
#include <RCC.h>

void SPIInit(uint8_t prescaler){
	gpio_pin_conf_t pinA5, pinA6, pinA7;
	pinA5.pin = PA5;
	pinA6.pin = PA6;
	pinA7.pin = PA7;
	//set mode
	pinA5.mode = GPIO_PIN_ALT_FUN_MODE;
	pinA6.mode = GPIO_PIN_ALT_FUN_MODE;
	pinA7.mode = GPIO_PIN_ALT_FUN_MODE;
	//output type
	pinA5.op_type = GPIO_PIN_OP_TYPE_PUSH_PULL;
	pinA6.op_type = GPIO_PIN_OP_TYPE_PUSH_PULL;
	pinA7.op_type = GPIO_PIN_OP_TYPE_PUSH_PULL;
	//set speed
	pinA5.speed = GPIO_PIN_OSPEED_HIGH;
	pinA6.speed = GPIO_PIN_OSPEED_HIGH;
	pinA7.speed = GPIO_PIN_OSPEED_HIGH;
	//set alternate function
	pinA5.alternate_function = 5;
	pinA6.alternate_function = 5;
	pinA7.alternate_function = 5;

	//enable clock for GPIOA
	RCC_GPIOA_CLK_ENABLE();
	gpio_init(GPIOA, &pinA5);
	gpio_init(GPIOA, &pinA6);
	gpio_init(GPIOA, &pinA7);

	//enable clock for the peripheral
	RCC_SPI1_CLK_ENABLE();
	//set baudrate
	SPI1->CR1 |= (prescaler << SPI_CR1_BR_Pos);
	//set SSM and SSI
	SPI1->CR1 |= SPI_CR1_SSM;
	SPI1->CR1 |= SPI_CR1_SSI;
	//set to master mode
	SPI1->CR1 |= SPI_CR1_MSTR;

	//finally enable SPI
	SPI1->CR1 |= SPI_CR1_SPE;
}

uint8_t SPITransfer(uint8_t data){
  uint8_t rx;

  //wait for the TXE flag
  while( !(SPI1->SR & SPI_SR_TXE) );

  //force 8-bit data write
  *(volatile uint8_t *)&SPI1->DR = data;

  //wait for the RXNE flag
  while( !(SPI1->SR & SPI_SR_RXNE) );

  //force 8-bit data access
  rx = *(volatile uint8_t*) &SPI1->DR;

  return rx;
}
