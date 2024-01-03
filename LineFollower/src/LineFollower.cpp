#include "LineFollower.h"

#define CAL_FACT	1000
#define MIN_THR		500

void switchToAnalogMode(){
	uint8_t TC = 1 << 6;
	//wait until Ttranmission completes
	while ( !(USART2->ISR & TC) );

	gpio_pin_conf_t pinA2;
	pinA2.pin = PA2;
	pinA2.mode = GPIO_PIN_OUTPUT_MODE;
	pinA2.op_type = GPIO_PIN_OP_TYPE_PUSH_PULL;
	pinA2.pupd = GPIO_PIN_PUPD_PULL_DOWN;
	pinA2.speed = GPIO_PIN_OSPEED_LOW;
	pinA2.alternate_function = -1; 			 //set pin2 to AF7	
	gpio_init(GPIOA, &pinA2);
	for(int i = 0; i < 1000; i++);

	pinA2.pin = PA2;
	pinA2.mode = GPIO_PIN_Analog_MODE;
	pinA2.op_type = GPIO_PIN_OP_TYPE_PUSH_PULL;
	pinA2.pupd = GPIO_PIN_PUPD_NO_PUPD;
	pinA2.speed = GPIO_PIN_OSPEED_LOW;
	pinA2.alternate_function = -1; 			 //set pin2 to AF7	
	gpio_init(GPIOA, &pinA2);
	for(int i = 0; i < 100; i++);
}

void switchToUsartMode() {
	gpio_pin_conf_t pinA2;
	pinA2.pin = PA2;
	//PA2, PA15 alternate function mode
	pinA2.mode = GPIO_PIN_ALT_FUN_MODE;
	pinA2.op_type = GPIO_PIN_OP_TYPE_PUSH_PULL;
	pinA2.pupd = GPIO_PIN_PUPD_NO_PUPD;
	pinA2.speed = GPIO_PIN_OSPEED_HIGH;
	pinA2.alternate_function = 7; 			 //set pin2 to AF7	

	gpio_init(GPIOA, &pinA2);

}

int map(float value, int oldMin, int oldMax, int newMin, int newMax){
	float temp = (float) (value - oldMin)/(oldMax - oldMin);
	return temp * (newMax - newMin) + newMin;
}

Motor::Motor(TIMER *timer, channel_conf_t *channel, uint32_t min, uint32_t max) {
	this->timer = timer;
	// init the thrust to 0
	current_thrust = 0;

	//deep copy the channel
	this->channel.no = channel->no;
	this->channel.io = channel->io;
	this->channel.mode = channel->mode;
	this->channel.polarity = channel->polarity;
	this->channel.capture_preload_enable = channel->capture_preload_enable;
	this->channel.capture_compare = channel->capture_compare;

	this->min = min;
	this->max = max;
}

void Motor::setThrust(float thrust){
	if(thrust < 0) thrust = 0;
	// thrust is a value between 0 and 100
	// you should map it to be a value between min and max
	uint32_t temp = map(thrust, 0, 100, this->min, this->max);
	channel.capture_compare =  temp;
	timer->set_capture_compare(timer->init_struct.instance, &channel);
	current_thrust = thrust;
}

LineFollower::LineFollower() : ir_channels{7, 12, 11, 10, 9, 8, 6, 5},
	ir_offsets{0, 0, 0, 0, 0, 0, 0, 0},
	weights{-90, -45, -22, -11, 11, 22, 45, 90}  {
	// weights{-0.3667f, -0.1333f, -0.06667f, -0.0333f, 0.0333f, 0.06667f, 0.1333f, 0.2667f}  {

	ADCx_init_t init_struct;
	init_struct.instance = ADC1;
	ADCInit(&init_struct);

	gpio_pin_conf_t pin;
	RCC_GPIOA_CLK_ENABLE();
	pin.mode = GPIO_PIN_ALT_FUN_MODE;
	pin.op_type = GPIO_PIN_OP_TYPE_PUSH_PULL;
	pin.pupd = GPIO_PIN_PUPD_NO_PUPD;
	pin.speed = GPIO_PIN_OSPEED_VERY_HIGH;
	pin.alternate_function = 1;
	pin.pin = PA8;
	gpio_init(GPIOA, &pin);
	pin.pin = PA11;
	gpio_init(GPIOA, &pin);


	TIMx_init_t tim1_init;
	tim1_init.instance = TIM1;
	tim1_init.direction = TIM_DIR_UPCOUNT;
	tim1_init.preload = TIM_CR1_ARPE;
	tim1_init.prescaler = 0;
	tim1_init.auto_reload = 2640;

	RCC_TIM1_CLK_ENABLE();							// enable clock first as usual
	timer.init(&tim1_init);

	channel_conf_t  channel;						
	channel.io = CHANNEL_IO_OUTPUT;
	channel.polarity = CHANNEL_POLARITY_HIGH;
	channel.capture_preload_enable = 0;
	channel.mode = OCxM_PWM_1;
	channel.capture_compare = 0;

	channel.no = 1;
	Motor1 = Motor(&timer, &channel, 0, 2640);
	timer.channel_init(TIM1, &channel);
	channel.no = 4;
	Motor2 = Motor(&timer, &channel, 0, 2640);
	timer.channel_init(TIM1, &channel);

	timer.timer_update();

	pid = Controller();
}

void LineFollower::calibration() {
	for(int i = 0; i < CAL_FACT; i++){
		readIrSensors();
		for(int j = 0; j < 8; j++){
			ir_offsets[j] += ir_raw_values[j];
		}
	}
	for(int j = 0; j < 8; j++){
		ir_offsets[j] /= CAL_FACT;
	}
	calibrated = true;
}

void LineFollower::readIrSensors() {
	switchToAnalogMode();
	for(uint8_t i = 0; i < n; i++){
		ir_raw_values[i] = analogRead(ADC1, ir_channels[i]);
		if(calibrated)
			ir_values[i] = ir_raw_values[i] - ir_offsets[i];
		ir_binary[i] = ir_values[i] > MIN_THR ? 1 : 0;
	}
	switchToUsartMode();

}


void LineFollower::control() {
	static uint32_t prev_micros = 0;

	float weightedSum = 0;
	int sum = 0;
	for(int i = 0; i < n; i++){
		if(ir_binary[i]){
			weightedSum += ir_values[i] * weights[i];
			sum += ir_values[i];
		}
	}
	if(sum != 0)
		pid.error = weightedSum / sum;
	dt = System.getDeltaT(prev_micros);
	prev_micros = System.getMicros();

	pid.run(-1, -1, dt);

	update();
	// }

}

void LineFollower::update() {
	Motor1.setThrust(direct + pid.command);
	Motor2.setThrust(direct - pid.command);
}

void LineFollower::printError(){
	Serial.printFloat(pid.error);
	Serial.printStrln("");
}

void LineFollower::printValues() {
	for(int i = 0; i < n; i ++){
		Serial.printInt(ir_values[i]);
		Serial.printStr(", ");
	}
	Serial.printStr("\t\t");
	Serial.printFloat(pid.error);
	Serial.printStr(", ");
	Serial.printFloat(pid.propotional);
	Serial.printStr(", ");
	Serial.printFloat(pid.derivative);
	Serial.printStr(", ");
	Serial.printFloat(pid.integral);
	Serial.printStr(", ");
	Serial.printFloat(pid.command);
	Serial.printStrln("");
}

void LineFollower::printRawValues() {
	for(int i = 0; i < n; i ++){
		Serial.printInt(ir_raw_values[i]);
		Serial.printStr(", ");
	}
	Serial.printStr("\t\t");
	Serial.printFloat(pid.error);
	Serial.printStr(", ");
	Serial.printFloat(pid.propotional);
	Serial.printStr(", ");
	Serial.printFloat(pid.derivative);
	Serial.printStr(", ");
	Serial.printFloat(pid.integral);
	Serial.printStr(", ");
	Serial.printFloat(pid.command);
	Serial.printStrln("");
}

void LineFollower::printBinary() {
	for(int i = 0; i < n; i ++){
		Serial.printInt(ir_binary[i]);
		Serial.printStr(", ");
	}
	Serial.printStr("\t\t");
	Serial.printFloat(pid.error);
	Serial.printStr(", ");
	Serial.printFloat(pid.propotional);
	Serial.printStr(", ");
	Serial.printFloat(pid.derivative);
	Serial.printStr(", ");
	Serial.printFloat(pid.integral);
	Serial.printStr(", ");
	Serial.printFloat(pid.command);
	Serial.printStrln("");

}
