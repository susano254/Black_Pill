#include <TIMER.h>
#include <RCC.h>
#include <GPIO.h>
#include <ADC.h>

using namespace global;

// TIM_TypeDef *TIM1_Base = TIM1;
int value = 0;

int main(){
	// System.Init();
	// Serial.begin(115200);

	gpio_pin_conf_t led1;
	led1.pin = PA2;
	led1.mode = GPIO_PIN_ALT_FUN_MODE;
	led1.op_type = GPIO_PIN_OP_TYPE_PUSH_PULL;
	led1.pupd = GPIO_PIN_PUPD_NO_PUPD;
	led1.speed = GPIO_PIN_OSPEED_VERY_HIGH;
	led1.alternate_function = 3;

	RCC_GPIOA_CLK_ENABLE();
	gpio_init(GPIOA, &led1);
	
		
	TIMER tim9;
	TIMx_init_t tim1_init;
	tim1_init.instance = TIM9;
	tim1_init.direction = TIM_DIR_UPCOUNT;
	tim1_init.preload = TIM_CR1_ARPE;
	tim1_init.prescaler = 0;
	tim1_init.auto_reload = 1000;

	channel_conf_t ch1;
	ch1.no = 1;
	ch1.io = CHANNEL_IO_OUTPUT;
	ch1.polarity = CHANNEL_POLARITY_HIGH;
	ch1.capture_preload_enable = 0;
	ch1.mode = OCxM_PWM_1;
	ch1.capture_compare = 50;


	RCC_TIM9_CLK_ENABLE();
	tim9.init(&tim1_init);
	tim9.channel_init(&ch1);
	tim9.update();
	
	while(1){
		for(int i = 0; i < 1000; i++){
			tim9.set_capture_compare(ch1.no, i);
			for(int x = 0; x < 1000; x++);
		}
		for(int i = 1000; i > 0; i--){
			tim9.set_capture_compare(ch1.no, i);
			for(int x = 0; x < 1000; x++);
		}
	}


}

// extern "C" {
	// void TIM6_DAC_IRQHandler(){
	// 	Nvic.clearPending(TIM2_IRQn);
	// 	TIM6->SR &= ~TIM_SR_UIF;
	// }
// }