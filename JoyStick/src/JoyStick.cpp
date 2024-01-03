#include <JoyStick.h>

Button::Button(GPIO_TypeDef* GPIOx, uint8_t pin){
	this->GPIOx = GPIOx;
	this->pin = pin;

	gpio_pin_conf_t button;

	button.pin = pin;
	button.mode = GPIO_PIN_INPUT_MODE;
	button.op_type = GPIO_PIN_OP_TYPE_OPEN_DRAIN;
	button.pupd = GPIO_PIN_PUPD_PULL_UP;
	button.speed = GPIO_PIN_OSPEED_MEDIUM;
	button.alternate_function = -1;

	enable_clock(GPIOx);
	gpio_init(GPIOx, &button);

}
bool Button::readButton(){
	return gpio_read_pin(GPIOx, pin);
}




Potentiometer::Potentiometer(GPIO_TypeDef *GPIOx, uint8_t vr_pin, uint8_t channel_no){
	this->GPIOx = GPIOx;
	this->vr_pin = vr_pin;
	this->channel_no = channel_no;

	gpio_pin_conf_t pot;

	pot.pin = vr_pin;
	pot.mode = GPIO_PIN_Analog_MODE;
	pot.op_type = GPIO_PIN_OP_TYPE_PUSH_PULL;
	pot.speed = GPIO_PIN_OSPEED_MEDIUM;
	pot.alternate_function = -1;

	enable_clock(GPIOx);
	gpio_init(GPIOx, &pot);

}

uint32_t Potentiometer::readPot(){
	return analogRead(ADC1, channel_no);
}




JoyStick::JoyStick(Potentiometer *vr_x, Potentiometer *vr_y, Button *button){
	this->vr_x = vr_x;
	this->vr_y = vr_y;
	this->button = button;
}

void JoyStick::readJoyStick(){
	state.vr_x = vr_x->readPot();
	state.vr_y = vr_y->readPot();
	state.button  = button->readButton();
}