#include <RCC.h>
#include <GPIO.h>
#include <System.h>



using namespace global;

int main(){
	System.Init();

	gpio_pin_conf_t led;
	led.pin = PC13;
	led.mode = GPIO_PIN_OUTPUT_MODE;
	led.op_type = GPIO_PIN_OP_TYPE_PUSH_PULL;
	led.pupd = GPIO_PIN_PUPD_NO_PUPD;
	led.speed = GPIO_PIN_OSPEED_LOW;
	led.alternate_function = -1;

	RCC_GPIOC_CLK_ENABLE();
	gpio_init(GPIOC, &led);


	uint8_t state = 1;
	while(1){
		for(int i = 0; i < 8000000; i++);
		gpio_write_pin(GPIOC, led.pin, state);
		state = !state;
	}

}