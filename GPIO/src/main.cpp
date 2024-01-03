#include <GPIO.h>
#include <RCC.h>

int main(){
	gpio_pin_conf_t led;
	led.pin = PC13;
	led.mode = GPIO_PIN_OUTPUT_MODE;
	led.op_type = GPIO_PIN_OP_TYPE_PUSH_PULL;
	led.speed = GPIO_PIN_OSPEED_LOW;
	led.alternate_function = -1;

	RCC_GPIOC_CLK_ENABLE();
	gpio_init(GPIOC, &led);

	uint32_t state = GPIO_OUTPUT_LOW; 
	while(1){
		for(int i = 0; i < 100000; i++);
		gpio_write_pin(GPIOC, led.pin, state);
		state = !state;
	}
	return 0;
}