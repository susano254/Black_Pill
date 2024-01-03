#include <RCC.h>
#include <Flash.h>


namespace global {
	I2C_TypeDef *I2C1_Base = I2C1;
	RCC_TypeDef *RCC_Base = RCC;
	GPIO_TypeDef *GPIOA_Base = GPIOA;
	GPIO_TypeDef *GPIOB_Base = GPIOB;
	GPIO_TypeDef *GPIOC_Base = GPIOC;
	USART_TypeDef *USART2_Base = USART2;
	SPI_TypeDef *SPI1_Base = SPI1;
	NVIC_Type *NVIC_Base = NVIC;
	SysTick_Type *SysTick_Base = SysTick;
	FLASH_TypeDef* FLASH_Base = FLASH;
}


// sets the msi range
void RCC_HSE25_CLK_ENABLE() {
	RCC->CR |= RCC_CR_HSEON;
	while (!(RCC->CR & RCC_CR_HSERDY));
}

void RCC_HSI16_CLK_ENABLE() {
	RCC->CR |= RCC_CR_HSION;
	while (!(RCC->CR & RCC_CR_HSIRDY));
}

// pll helpers
void RCC_PLL_CLK_ENABLE() {
	RCC->CR |= RCC_CR_PLLON;
	while (!(RCC->CR & RCC_CR_PLLRDY));
}

void RCC_SET_PLL_SRC(uint32_t pllSrc) {
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC_Msk;

	if (pllSrc == RCC_PLLCFGR_PLLSRC_HSE)
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;
}

void RCC_SET_PLLM_DIV(uint32_t pllM) {
	if (pllM >= RCC_PLLM_MIN && pllM <= RCC_PLLM_MAX) {
		RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM_Msk;
		RCC->PLLCFGR |= (pllM << RCC_PLLCFGR_PLLM_Pos);
	}
}

void RCC_SET_PLLN_MUL(uint32_t pllN) {
	if (pllN >= RCC_PLLN_MIN && pllN <= RCC_PLLN_MAX) {
		RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN_Msk;
		RCC->PLLCFGR |= (pllN << RCC_PLLCFGR_PLLN_Pos);
	}
}
void RCC_SET_PLLP_DIV(uint32_t pllP) {
	if(pllP >= RCC_PLLP_DIV2 && pllP <= RCC_PLLP_DIV8){
		RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP_Msk;
		RCC->PLLCFGR |= (pllP << RCC_PLLCFGR_PLLP_Pos);
	}
}

// set sysclk mux
uint8_t RCC_SET_SYSCLK(uint32_t sysClock) {
	if (sysClock > RCC_CFGR_SW_PLL | sysClock < RCC_CFGR_SW_HSI)
		return 0;
	RCC->CFGR &= ~RCC_CFGR_SW_Msk;
	RCC->CFGR |= sysClock;
	while (((RCC->CFGR & RCC_CFGR_SWS) >> RCC_CFGR_SWS_Pos) != sysClock);
	return 1;
}

void RCC_SET_AHB_PRESCALER(uint32_t prescaler) {
	RCC->CFGR &= ~RCC_CFGR_HPRE_Msk;
	RCC->CFGR |= prescaler;
}

void RCC_SET_APB1_PRESCALER(uint32_t prescaler) {
	RCC->CFGR &= ~RCC_CFGR_PPRE1_Msk;
	RCC->CFGR |= prescaler;
}

void RCC_SET_APB2_PRESCALER(uint32_t prescaler) {
	RCC->CFGR &= ~RCC_CFGR_PPRE2_Msk;
	RCC->CFGR |= prescaler;
}

void RCC_Init_Max_Clock() {
	// Enable HSI clock
	RCC_HSI16_CLK_ENABLE();
	RCC_HSE25_CLK_ENABLE();

	// select src of the pll
	RCC_SET_PLL_SRC(RCC_PLLCFGR_PLLSRC_HSE);

	// select pllM div
	RCC_SET_PLLM_DIV(25);

	// select pllN div
	RCC_SET_PLLN_MUL(336);

	// select pllR div
	RCC_SET_PLLP_DIV(RCC_PLLP_DIV4);

	// enable pll
	RCC_PLL_CLK_ENABLE();

	// change flash latency
	FLASH_SET_LATENCY(FLASH_ACR_LATENCY_2WS);

	// configure AHB bus prescaler
	RCC_SET_AHB_PRESCALER(RCC_CFGR_HPRE_DIV1);

	// configure APB1 bus prescaler
	RCC_SET_APB1_PRESCALER(RCC_CFGR_PPRE1_DIV2);

	// configure APB2 bus prescaler
	RCC_SET_APB2_PRESCALER(RCC_CFGR_PPRE2_DIV1);

	// select pll as sysclk
	RCC_SET_SYSCLK(RCC_CFGR_SW_PLL);

	return;
}