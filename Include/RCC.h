#ifndef RCC_H
#define RCC_H

#include <stm32f401xc.h>

// #define RCC_PLLR_ENABLE()       (RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN)

/****************************AHB1 BUS Clocks****************************/
#define RCC_DMA1_CLK_ENABLE()	  (RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN)
#define RCC_DMA2_CLK_ENABLE()	  (RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN)
#define RCC_CRC_CLK_ENABLE()	  (RCC->AHB1ENR |= RCC_AHB1ENR_CRCEN)
#define RCC_GPIOA_CLK_ENABLE()	(RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN)
#define RCC_GPIOB_CLK_ENABLE()	(RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN)
#define RCC_GPIOC_CLK_ENABLE()	(RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN)
#define RCC_GPIOD_CLK_ENABLE()	(RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN)
#define RCC_GPIOE_CLK_ENABLE()	(RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN)
#define RCC_GPIOH_CLK_ENABLE()	(RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN)

/****************************AHB2 BUS Clocks****************************/
#define RCC_USB_CLK_ENABLE()    (RCC->AHB2ENR |= RCC_AHB2ENR_OTGFSEN)

/****************************APB1 BUS Clocks****************************/
#define RCC_TIM2_CLK_ENABLE()	  (RCC->APB1ENR |= RCC_APB1ENR_TIM2EN)
#define RCC_TIM3_CLK_ENABLE()	  (RCC->APB1ENR |= RCC_APB1ENR_TIM3EN)
#define RCC_TIM4_CLK_ENABLE()	  (RCC->APB1ENR |= RCC_APB1ENR_TIM4EN)
#define RCC_TIM5_CLK_ENABLE()	  (RCC->APB1ENR |= RCC_APB1ENR_TIM5EN)
#define RCC_WWDG_CLK_ENABLE()   (RCC->APB1ENR |= RCC_APB1ENR_WWDGEN)
#define RCC_SPI2_CLK_ENABLE()   (RCC->APB1ENR |= RCC_APB1ENR_SPI2EN)
#define RCC_SPI3_CLK_ENABLE()   (RCC->APB1ENR |= RCC_APB1ENR_SPI3EN)
#define RCC_USART2_CLK_ENABLE() (RCC->APB1ENR |= RCC_APB1ENR_USART2EN)
#define RCC_I2C1_CLK_ENABLE()	  (RCC->APB1ENR |= RCC_APB1ENR_I2C1EN)
#define RCC_I2C2_CLK_ENABLE()	  (RCC->APB1ENR |= RCC_APB1ENR_I2C2EN)
#define RCC_I2C3_CLK_ENABLE()	  (RCC->APB1ENR |= RCC_APB1ENR_I2C3EN)
#define RCC_PWR_CLK_ENABLE()    (RCC->APB1ENR |= RCC_APB1ENR_PWREN)


/****************************APB2 BUS Clocks****************************/
#define RCC_TIM1_CLK_ENABLE()	  (RCC->APB2ENR |= RCC_APB2ENR_TIM1EN)
#define RCC_USART1_CLK_ENABLE()	(RCC->APB2ENR |= RCC_APB2ENR_USART1EN)
#define RCC_USART6_CLK_ENABLE()	(RCC->APB2ENR |= RCC_APB2ENR_USART6EN)
#define RCC_ADC1_CLK_ENABLE()	  (RCC->APB2ENR |= RCC_APB2ENR_ADC1EN)
#define RCC_SDIO_CLK_ENABLE()	  (RCC->APB2ENR |= RCC_APB2ENR_SDIOEN)
#define RCC_SPI1_CLK_ENABLE()	  (RCC->APB2ENR |= RCC_APB2ENR_SPI1EN)
#define RCC_SPI4_CLK_ENABLE()	  (RCC->APB2ENR |= RCC_APB2ENR_SPI4EN)
#define RCC_SYSCFG_CLK_ENABLE()	(RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN)
#define RCC_TIM9_CLK_ENABLE()	  (RCC->APB2ENR |= RCC_APB2ENR_TIM9EN)
#define RCC_TIM10_CLK_ENABLE()	(RCC->APB2ENR |= RCC_APB2ENR_TIM10EN)
#define RCC_TIM11_CLK_ENABLE()	(RCC->APB2ENR |= RCC_APB2ENR_TIM11EN)


#define RCC_GET_SYSCLK_SRC() ((RCC->CFGR & (0b11 << RCC_CFGR_SWS_Pos)) >> RCC_CFGR_SWS_Pos)

typedef enum {
 MCO_SEL_NO_CLOCK  = 0b0000,
 MCO_SEL_SYSCLK    = 0b0001,
 MCO_SEL_MSI       = 0b0010,
 MCO_SEL_HSI16     = 0b0011,
 MCO_SEL_HSE       = 0b0100,
 MCO_SEL_PLL       = 0b0101,
 MCO_SEL_LSI       = 0b0110,
 MCO_SEL_LSE       = 0b0111,
 MCO_SEL_HSI48     = 0b1000
} mco_clocks_t;


typedef enum {
 HCLK_DIV_1    = 0b000,
 HCLK_DIV_2    = 0b100,
 HCLK_DIV_4    = 0b101,
 HCLK_DIV_8    = 0b110,
 HCLK_DIV_16   = 0b111
} ppre_div_t;

typedef enum {
 SW_HSI16    = 0b00,
 SW_HSE      = 0b01,
 SW_PLL      = 0b10
} system_clock_t;


#define RCC_PLLM_MIN          2
#define RCC_PLLM_MAX          63

#define RCC_PLLN_MIN          192
#define RCC_PLLN_MAX          432

#define  RCC_PLLP_DIV2       0b00
#define  RCC_PLLP_DIV4       0b01
#define  RCC_PLLP_DIV6       0b10
#define  RCC_PLLP_DIV8       0b11




typedef enum {
  PCLK = 0b00,
  SYSCLK = 0b01,
  HSI16 = 0b10,
  LSE = 0b11
} preipheral_clocks_t;

namespace global {
	extern I2C_TypeDef *I2C1_Base;
	extern RCC_TypeDef *RCC_Base;
	extern GPIO_TypeDef *GPIOA_Base;
	extern GPIO_TypeDef *GPIOB_Base;
	extern GPIO_TypeDef *GPIOC_Base;
	extern USART_TypeDef *USART2_Base;
	extern SPI_TypeDef *SPI1_Base;
	extern NVIC_Type* NVIC_Base;
	extern SysTick_Type *SysTick_Base;
	extern FLASH_TypeDef *FLASH_Base;
}

#ifdef __cplusplus
extern "C" {
#endif

  // void SystemInit();
  void RCC_Init_Max_Clock();
  uint8_t RCC_SET_SYSCLK(uint32_t sysClock);
  // uint8_t RCC_SET_MSI_RANGE(msi_range_t range);
  void RCC_I2C3_CLK_SEL(preipheral_clocks_t clock);
  void RCC_I2C1_CLK_SEL(preipheral_clocks_t clock);
  void RCC_HSI16_CLK_ENABLE();
  void RCC_HSE25_CLK_ENABLE();
  void RCC_PLL_CLK_ENABLE();
  void RCC_SET_PLL_SRC(uint32_t pllSrc);
  void RCC_SET_PLLM_DIV(uint32_t pllM);
  void RCC_SET_PLLN_MUL(uint32_t pllN);
  void RCC_SET_PLLP_DIV(uint32_t pllP);
  void RCC_SET_AHB_PRESCALER(uint32_t prescaler);
  void RCC_SET_APB1_PRESCALER(uint32_t prescaler);
  void RCC_SET_APB2_PRESCALER(uint32_t prescaler);


#ifdef __cplusplus
}
#endif

#endif