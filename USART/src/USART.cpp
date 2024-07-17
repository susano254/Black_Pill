#include <USART.h>
#include <GPIO.h>
#include <RCC.h>
#include <System.h>
#include <string.h>
#include <string>

using namespace std;
using namespace global;


#define PRECISION 3
#define PREC_RADIX 1000

namespace global {
	VirtualPort Serial;
}


// void USARTx::hal_usart_enable(USART_TypeDef *USARTx){
// 	USARTx->CR1 |= USART_REG_CR1_UE;
// }
// void USARTx::hal_usart_disable(USART_TypeDef *USARTx){
// 	USARTx->CR1 &= ~USART_REG_CR1_UE;
// }

// void USARTx::hal_usart_tx_enable(USART_TypeDef *USARTx){
// 	USARTx->CR1 |= USART_REG_CR1_TE;
// }
// void USARTx::hal_usart_tx_disable(USART_TypeDef *USARTx){
// 	USARTx->CR1 &= ~USART_REG_CR1_TE;
// }

// void USARTx::hal_usart_rx_enable(USART_TypeDef *USARTx){
// 	USARTx->CR1 |= USART_REG_CR1_RE;
// }
// void USARTx::hal_usart_rx_disable(USART_TypeDef *USARTx){
// 	USARTx->CR1 &= ~USART_REG_CR1_RE;
// }

// void USARTx::hal_usart_error_interrupt_enable(USART_TypeDef *USARTx){
// 	USARTx->CR3 |= USART_REG_CR3_EIE;
// }
// void USARTx::hal_usart_error_interrupt_disable(USART_TypeDef *USARTx){
// 	USARTx->CR3 &= ~USART_REG_CR3_EIE;
// }

// void USARTx::hal_usart_configure_world_length(USART_TypeDef *USARTx, uint8_t worldLength){
// 	if(worldLength == WL_9){
// 		USARTx->CR1 |=  USART_REG_CR1_M0;
// 		USARTx->CR1 &= ~USART_REG_CR1_M1;
// 	}
// 	else if(worldLength == WL_7){
// 		USARTx->CR1 &= ~USART_REG_CR1_M0;
// 		USARTx->CR1 |=  USART_REG_CR1_M1;
// 	}
// 	else {
// 		USARTx->CR1 &= ~USART_REG_CR1_M0;
// 		USARTx->CR1 &= ~USART_REG_CR1_M1;
// 	}
// }


// void USARTx::hal_usart_configure_stop_bits(USART_TypeDef *USARTx, uint8_t nstop){
// 	USARTx->CR2 &= ~(0x11 << USART_REG_CR2_STOP_BITS);

// 	if(nstop == USART_STOP_BITS_HALF)
// 		USARTx->CR2 |=  (0x01 << USART_REG_CR2_STOP_BITS);
// 	else if(nstop == USART_STOP_BITS_TWO)
// 		USARTx->CR2 |=  (0x02 << USART_REG_CR2_STOP_BITS);
// 	else if(nstop == USART_STOP_BITS_ONE_HALF)
// 		USARTx->CR2 |=  (0x03 << USART_REG_CR2_STOP_BITS);
// 	else 
// 		USARTx->CR2 |=  (0x00 << USART_REG_CR2_STOP_BITS);
// }

// void USARTx::hal_usart_configure_over_sampling(USART_TypeDef *USARTx, uint8_t over8){
// 	if(over8 == USART_OVERSAMPLING_8)
// 		USARTx->CR1 |=  USART_REG_CR1_OVER8;
// 	else 
// 		USARTx->CR1 &= ~USART_REG_CR1_OVER8;
// }

// void USARTx::hal_usart_init(usart_handle_t *handle){
// 	//configure world length

// 	//configure number of stop bits

// 	//configure oversampling 

// 	//set baudrate

// 	//enable TX block

// 	//enable RX block

// 	//enable peripheral


// 	//set state 
// 	handle->tx_state = HAL_USART_STATE_READY;
// 	handle->rx_state = HAL_USART_STATE_READY;
// 	handle->errorCode = HAL_USART_ERROR_NONE;

// }

void VirtualPort::begin(int baudRate){
	//configure the pins
	gpio_pin_conf_t pinA2, pinA15;

	pinA2.pin = PA2;
	pinA15.pin = PA3;

	//PA2, PA3 alternate function mode
	pinA2.mode = GPIO_PIN_ALT_FUN_MODE;
	pinA15.mode = GPIO_PIN_ALT_FUN_MODE;
	pinA2.op_type = GPIO_PIN_OP_TYPE_PUSH_PULL;
	pinA15.op_type = GPIO_PIN_OP_TYPE_PUSH_PULL;
	pinA2.pupd = GPIO_PIN_PUPD_NO_PUPD;
	pinA15.pupd = GPIO_PIN_PUPD_NO_PUPD;
	pinA2.speed = GPIO_PIN_OSPEED_HIGH;
	pinA15.speed = GPIO_PIN_OSPEED_HIGH;
	pinA2.alternate_function = 7; 			 //set pin2 to AF7	
	pinA15.alternate_function = 7;			 //set pin3 to AF7

	//enable clock for GPIOA
	RCC_GPIOA_CLK_ENABLE();
	gpio_init(GPIOA, &pinA2);
	gpio_init(GPIOA, &pinA15);
	
	//enable clock
	RCC_USART2_CLK_ENABLE();

	//write the baudrate 
	float usart_div = 42000000.0/(16*baudRate);

	int mantissa = usart_div;
	int fraction = (usart_div-mantissa)*16;
	USART2->BRR |= (mantissa << USART_BRR_DIV_Mantissa_Pos) | (fraction << USART_BRR_DIV_Fraction_Pos);

	//enable peripheral
	//TE and RE set 
	USART2->CR1 |= USART_CR1_RE | USART_CR1_TE;

	//UE (USART enable) set
	USART2->CR1 |= USART_CR1_UE;


	USART_EnableInterrupt(USART2,  5);
}

void VirtualPort::sendByte(uint8_t value){
	//wait until TXE is set
	while ( !(USART2->SR & USART_SR_TXE) );

	*(volatile uint8_t*)&USART2->DR = value;
}

void VirtualPort::send(uint8_t *value, uint16_t size){
	for(int i = 0; i < size; i++)
		sendByte(value[i]);
	
	//wait until TC is set
	while ( !(USART2->SR & USART_SR_TC) );
}

void VirtualPort::floatToStr(float num, char *str){
	char intStr[10], floatStr[10];
	int integerPart, floatPart;
	uint8_t negative = 0;

	//initialize integer part to the type casted value (will remove what is after the decimal point)
	integerPart = num;
	//multiply the value by 10^3 and modulus to get the first 3 values after the decimal point
	floatPart = (int) (num * 1000.0f) % 1000;
	//if negative 
	if(num < 0){
		negative = 1;
		integerPart = -integerPart;
		floatPart = -floatPart;
	}

	//convert both numbers
	intToStr(integerPart, intStr);
	intToStr(floatPart, floatStr);

	int floatLen = length(floatStr);
	if(floatLen < 3){
		char temp[3] = {'\0'};
		int diff = 3 - floatLen;

		for(int i = 0; i < diff; i++)
			strcat(temp, "0");
		strcat(temp, floatStr);
		strcpy(floatStr, temp);
	}

	//finally combine them
	if(negative){
		char temp[20] = "-";
		strcat(temp, intStr);
		strcpy(intStr, temp);
	}
	strcat(intStr, ".");
	strcpy(str, intStr);
	strcat(str, floatStr);
}
void VirtualPort::printFloat(float num){
	char str[30];
	floatToStr(num, str);
	printStr(str);
}
string VirtualPort::floatToStr(float num){
	char str[30];
	floatToStr(num, str);
	return string(str);
}
void VirtualPort::printInt(int num) {
        char str[30];
	intToStr(num, str);
	printStr(str);
}
void VirtualPort::intToStr(int num, char *pStr){
	int mod, i;
	char str[20];
	uint8_t negative = 0;

	if(num < 0){ 
		negative = 1;
		num = -num;
	}
	

	if(num == 0) { 
		strcpy(pStr, "0");
		return;
	}

	for(i = 0; num != 0; i++){
		mod = num % 10;
		str[i] = mod + '0';
		num /= 10;
	}
	str[i] = '\0';
	for(int n = 0, m = i - 1; n < m; n++, m--){
		char temp = str[n];
		str[n] = str[m];
		str[m] = temp;
	}

	if(negative){
		strcpy(pStr,"-");
		strcat(pStr, str);
	}
	else {
		strcpy(pStr, str);
	}
}

void VirtualPort::printStrln(char *value){
	//add one to for the null byte at the end
	int size = length(value)+1;
	send((uint8_t *)value, size);
	sendByte('\n');
}

void VirtualPort::printStr(char *value){
	//add one to for the null byte at the end
	int size = length(value)+1;
	send((uint8_t *)value, size);
}
int VirtualPort::length(char *str){
	char c = str[0];
	int len = 0;

	while (c != '\0') {
		len++;
		c = str[len];
	}

	return len;
}

void USART_EnableInterrupt(USART_TypeDef *USARTx, uint8_t interrupt){
	if(interrupt < 4 || interrupt > 8) return;

	//first enable interrupt for the USART handler
	Nvic.enableInterrupt(USART2_IRQn);
	//enable RXNEIE
	USARTx->CR1 |= 1 << interrupt;
}

bool VirtualPort::isAvailable() { 
	if(RXNE) return true;
	return false;
}

string VirtualPort::recieve() {
	while(!Serial.isAvailable());
	RXNE = false;
	string temp = buffer;
	buffer.clear();
	return temp;
}

// void ITM_SendChar(uint8_t c){
// 	// global enable of Trace units DWT ITM 
// 	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
// 	// enable ITM stimulus ports for swo pin
// 	ITM->TCR |= 1 << 0;
// 	// enable stimulus port 0 
// 	ITM->TER |= 1 << 0;

// 	while(!(ITM->PORT[0].u8 & 1));

// 	ITM->PORT[0].u8 = c;
// }

extern "C" {
	void USART2_IRQHandler(){
		uint8_t temp = USART2->SR;
		uint8_t value = *(volatile uint8_t*)&USART2->DR;
		Serial.buffer += value;
		if(value == '\n' || value == '\r' || value == '\0'){
			Serial.RXNE = true;
		}
		Nvic.clearPending(USART2_IRQn);
	}
}
