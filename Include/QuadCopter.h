#include <vector>
#include <GPIO.h>
#include <TIMER.h>
#include <MPU.h>
#include <NRF24.h>
#include <USART.h>
#include <System.h>
#include <queue>

using namespace std;
using namespace global;


struct Motor {
	TIMER *timer;
	channel_conf_t channel;
	uint32_t min, max;
	float current_thrust;


	Motor(TIMER *timer, channel_conf_t *channel, uint32_t min, uint32_t max);
	void setThrust(float thrust);
};


struct Controller {
	float kp, ki, kd;
	float error, prev_error, filtered_de;
	float propotional, integral, derivative;
	float command;

	explicit Controller(){
		error = 0;
		prev_error = 0;
		kp = 0;
		ki = 0;
		kd = 0;
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
		calc_error(desired, current);
		calc_proportional();
		calc_integral(dt);
		calc_derivative(dt, true);
		command = propotional + integral + derivative;
	}
};

struct QuadCopter {
	TIMER timer;
	vector<Motor> motors;
	MPU_t mpu;
	NRF24 nrf;
	float dt;
	float desired_roll = 0, desired_pitch = 0, desired_yaw = 0;
	float direct = 0;

	struct {
		Controller Roll, Pitch, Yaw, Altitude;
	} controllers;


	QuadCopter() : controllers{ Controller(), Controller(), Controller(), Controller() } {}
	void Init();
	void NRF_Read(bool);
	// void NRF_Send();
	void IMU_Update(Madgwick&, bool);
	void Control();

	private:
		// some helper functions for the init sequence
		void Update();
		void init_pins();
		void init_timer();
		void init_channels();
		void init_escs();
		void init_motors();
		void init_controllers();
};