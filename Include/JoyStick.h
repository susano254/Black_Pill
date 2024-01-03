#ifndef 	JOYSTICK_H
#define 	JOYSTICK_H

#include <ADC.h>
#include <GPIO.h>

struct Button {
	GPIO_TypeDef *GPIOx;
	uint8_t pin;

	
	Button(GPIO_TypeDef* GPIOx, uint8_t pin);
	bool readButton();
};

struct Potentiometer {
	GPIO_TypeDef *GPIOx;
	uint8_t vr_pin, channel_no;

	Potentiometer(GPIO_TypeDef * GPIOx, uint8_t vr_pin, uint8_t channel_no);
	uint32_t readPot();

};

struct JoyStick_Reading {
	uint32_t vr_x;
	uint32_t vr_y;
	bool button;
};

struct JoyStick {
	Potentiometer *vr_x;
	Potentiometer *vr_y;
	Button *button;
	JoyStick_Reading state;

	JoyStick(Potentiometer *vr_x, Potentiometer *vr_y, Button *button);
	void readJoyStick();

};



#endif