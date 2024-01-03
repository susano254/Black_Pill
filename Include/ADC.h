#ifndef ADC_H
#define ADC_H

#include <stm32f401xc.h>


// #define RCC_ADC_CLK_SYS_SEL()			(RCC->CCIPR |= RCC_CCIPR_ADCSEL)

typedef struct {
	uint32_t number;
	uint32_t sampling_time;
	uint32_t sequence;
	uint32_t status;
} adc_channel_conf_t; 

typedef struct { 
	ADC_TypeDef *instance;
	adc_channel_conf_t	*channels;
	uint32_t 	numOfChannels;
	uint32_t 	numOfConversions;
} ADCx_init_t;


#ifdef __cplusplus
extern "C" {
#endif                                      
	void ADCInit(ADCx_init_t *init_struct);
	uint32_t analogRead(ADC_TypeDef* ADCx, uint8_t channelNumber);
#ifdef __cplusplus
}
#endif                                      


#endif