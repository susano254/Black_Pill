#include <ADC.h>
#include <RCC.h>


void start_conversion(ADC_TypeDef *ADCx){
	ADCx->CR2 |= ADC_CR2_SWSTART;
}

void poll_end_of_conversion(ADC_TypeDef *ADCx){
	while( !(ADCx->SR & ADC_SR_EOC) );
}

// void disable_deep_pwd(ADC_TypeDef *ADCx){
// 	ADCx->CR2 &= ~ADC_CR2_ADON;
// }
// void enable_deep_pwd(ADC_TypeDef *ADCx){
// 	ADCx->CR |= ADC_CR_DEEPPWD;
// }
// void disable_adc_voltage_regulator(ADC_TypeDef *ADCx){
// 	ADCx->CR &= ~ADC_CR_ADVREGEN;
// }
// void enable_adc_voltage_regulator(ADC_TypeDef *ADCx){
// 	ADCx->CR |= ADC_CR_ADVREGEN;
// }

void enable_adc(ADC_TypeDef* ADCx){
	ADCx->CR2 |= ADC_CR2_ADON;
}

void disable_adc(ADC_TypeDef* ADCx){
	ADCx->CR2 &= ~ADC_CR2_ADON;
}

void set_conversions(uint8_t numOfConversions){

}

void set_regular_channels_sequence(adc_channel_conf_t* channels, uint8_t numOfChannels){

}

void set_channels_sampling_time(adc_channel_conf_t* channels, uint8_t numOfChannels){

}


void readChannels(adc_channel_conf_t *channels, uint8_t numOfChannels){

}

uint32_t analogRead(ADC_TypeDef* ADCx, uint8_t channelNumber){
	//one conversion
	ADCx->SQR1 = 0;
	ADCx->SQR3 = 0;

	ADCx->SQR3 |= (channelNumber << ADC_SQR3_SQ1_Pos);

	start_conversion(ADCx);

	poll_end_of_conversion(ADCx);

	return ADCx->DR;
}



void ADCInit(ADCx_init_t *init_struct){
	//select clock source for the ADC
	RCC_ADC1_CLK_ENABLE();

	//average wake up time is 20us for the voltage regulator
	//(since clock is 80mhz) and we want to wait for 50us we need to count 4000
	//finally enable the adc
	enable_adc(init_struct->instance);
	for(int i = 0; i < 4000; i++);			//delay 50us  


	RCC_GPIOA_CLK_ENABLE();
}