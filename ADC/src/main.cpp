#include <ADC.h>
#include <RCC.h>
// #include <USART.h>
#include <GPIO.h>
#include <TIMER.h>
// #include <Core.h>



using namespace global;


int main(){
	ADCx_init_t init_struct;
	init_struct.instance = ADC1;


	ADCInit(&init_struct);

	gpio_pin_conf_t pot;
	pot.pin = PA0;
	pot.mode = GPIO_PIN_Analog_MODE;
	pot.op_type = GPIO_PIN_OP_TYPE_PUSH_PULL;
	pot.speed = GPIO_PIN_OSPEED_LOW;
	pot.alternate_function = -1;

	RCC_GPIOA_CLK_ENABLE();
	gpio_init(GPIOA, &pot);

	while(1){
		uint32_t ch0;
 		ch0 = analogRead(ADC1, 0);
	}
}