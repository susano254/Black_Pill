#ifndef LINEFOLLOWER_H
#define LINEFOLLOWER_H

#include <ADC.h>
#include <RCC.h>
#include <USART.h>
#include <GPIO.h>
#include <Core.h>
#include <TIMER.h>




using namespace global;

struct Motor {
	TIMER *timer;
	channel_conf_t channel;
	uint32_t min, max;
	float current_thrust;

	Motor(){}
	Motor(TIMER *timer, channel_conf_t *channel, uint32_t min, uint32_t max);
	void setThrust(float thrust);
};

struct Controller {
	float kp, ki, kd;
	float error, prev_error, filtered_de;
	float propotional, integral, derivative;
	float command;

	Controller(){
		error = 0;
		prev_error = 0;
		kp = 1.11;
		ki = 0.0;
		kd = 0.05;
		propotional = 0;
		integral = 0;
		derivative = 0;
		command = 0;
		filtered_de = 0;
	}

	void calc_error(float desired, float current){
		error = desired - current;
	}
	void calc_proportional(){
		propotional = kp * error;
	}
	void calc_integral(float dt){
		integral += ki * error * dt;
	}
	void calc_derivative(float dt, bool filter){
		float change = error - prev_error;
		if(filter){
			float alpha = 0.98f;
			filtered_de =  alpha * (change / dt) + (1-alpha) * filtered_de;
			derivative = kd*filtered_de;
		}
		else {
			derivative = kd*(change/dt);
		}
		prev_error = error;
	}
	void run(float desired, float current, float dt){
		// calc_error(desired, current);
		calc_proportional();
		calc_integral(dt);
		calc_derivative(dt, false);
		command = propotional + integral + derivative;
	}
};

struct LineFollower{
	TIMER timer;
	Motor Motor1, Motor2;
	float dt;
	Controller pid;
	float direct = 80;
	bool calibrated = false;

	static const uint8_t n = 8;
	uint8_t ir_channels[n];
	float weights[n];
	int32_t ir_raw_values[n];
	int32_t ir_values[n];
	int32_t ir_offsets[n];
	uint8_t ir_binary[n];


	LineFollower();
	void calibration();
	void readIrSensors();
	void control();
	void update();
	void printRawValues();
	void printValues();
	void printBinary();
	void printError();
};

#ifdef __cplusplus
extern "C" {
#endif                                      
	void switchToAnalogMode();
	void switchToUsartMode();
#ifdef __cplusplus
}
#endif                                      

#endif