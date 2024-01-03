#include <JoyStick.h>
#include <GPIO.h>
#include <RCC.h>

int main(){
	uint32_t data[6];

	Potentiometer pot1(GPIOA, PA0, 0);
	Potentiometer pot2(GPIOA, PA1, 1);
	// Button btn1(GPIOB, PB9);

	Potentiometer pot3(GPIOB, PB0, 8);
	Potentiometer pot4(GPIOB, PB1, 9);
	// Button btn2(GPIOA, PA8);


	// JoyStick joystick_right(&pot1, &pot2, &btn1);
	// JoyStick joystick_left(&pot3, &pot4, &btn2);


	ADCx_init_t init_struct;
	init_struct.instance = ADC1;
	ADCInit(&init_struct);

	while(true){
		// joystick_left.readJoyStick();
		// joystick_right.readJoyStick();

		data[0] = pot1.readPot();
		data[1] = pot2.readPot();

		data[2] = pot3.readPot();
		data[3] = pot4.readPot();

	}

	return 0;
}