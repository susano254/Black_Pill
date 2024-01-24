#include <QuadCopter.h>
#include <RCC.h>
#include <System.h>
#include <GPIO.h>

#define MOTOR1 PB4
#define MOTOR2 PB5
#define MOTOR3 PB0
#define MOTOR4 PB1
#define MOTORS_GPIO	GPIOB

using namespace global;

uint32_t millis;
uint32_t prev_millis;

void delay(uint32_t milliseconds){
	uint32_t startmillis = Systick.getMillis();
	while((Systick.getMillis()-startmillis) < milliseconds);
}

int map(int value, int oldMin, int oldMax, int newMin, int newMax){
	float temp = (float) (value - oldMin)/(oldMax - oldMin);
	return temp * (newMax - newMin) + newMin;
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
	timer->set_capture_compare(channel.no, channel.capture_compare);
	current_thrust = thrust;
}

void QuadCopter::init_pins(){
	gpio_pin_conf_t pin;

	RCC_GPIOB_CLK_ENABLE();

	pin.mode = GPIO_PIN_ALT_FUN_MODE;
	pin.op_type = GPIO_PIN_OP_TYPE_PUSH_PULL;
	pin.pupd = GPIO_PIN_PUPD_NO_PUPD;
	pin.speed = GPIO_PIN_OSPEED_VERY_HIGH;
	pin.alternate_function = 2;

	//pins PA0 to PA3 for the 4 motors
	pin.pin = MOTOR1;
	gpio_init(MOTORS_GPIO, &pin);
	pin.pin = MOTOR2;
	gpio_init(MOTORS_GPIO, &pin);
	pin.pin = MOTOR3;
	gpio_init(MOTORS_GPIO, &pin);
	pin.pin = MOTOR4;
	gpio_init(MOTORS_GPIO, &pin);
}

void QuadCopter::init_timer(){
	TIMx_init_t tim3_init;

	tim3_init.instance = TIM3;					// choose timer2
	tim3_init.direction = TIM_DIR_UPCOUNT;		// up count direction
	tim3_init.preload = TIM_CR1_ARPE;			// Auto reload preload enable
	tim3_init.prescaler = 84-1;					// no prescaler it's getting the full 80Mhz of the controller clock
	tim3_init.auto_reload = 16650-1;			// acheiving a period of 16.77ms for 60Hz

	RCC_TIM3_CLK_ENABLE();							// enable clock first as usual


	timer.init(&tim3_init);
}

void QuadCopter::init_channels(){
	channel_conf_t  channel;						// for tim2 enable the first 4 channel to output a pwm signal
	channel.io = CHANNEL_IO_OUTPUT;
	channel.polarity = CHANNEL_POLARITY_HIGH;
	channel.capture_preload_enable = 0;
	channel.mode = OCxM_PWM_1;
	channel.capture_compare = 0;

	// this range values are based on tests of escs ranges since I got ones 
	// that don't support programming mode of range calibration :(
	channel.no = 1;
	motors.push_back(Motor(&timer, &channel, 1150, 2000));
	timer.channel_init(&channel);
	channel.no = 2;
	motors.push_back(Motor(&timer, &channel, 750, 2000));
	timer.channel_init(&channel);
	channel.no = 3;
	motors.push_back(Motor(&timer, &channel, 1050, 2000));
	timer.channel_init(&channel);
	channel.no = 4;
	motors.push_back(Motor(&timer, &channel, 750, 2000));
	timer.channel_init(&channel);
	
	timer.update();
}

void QuadCopter::init_escs(){
	motors[0].setThrust(0);
	motors[1].setThrust(0);
	motors[2].setThrust(0);
	motors[3].setThrust(0);
	//delay
	delay(5000);
}

void QuadCopter::init_motors(){
	//set the pins to alternate function mode for the timer output
	init_pins();
	//init the timer
	init_timer();

	//init the timer channels for motors
	init_channels();

	//ESC init sequence
	init_escs();

	//init the pid controllers
	init_controllers();
}

void QuadCopter::init_controllers() {
	controllers.Roll.kp = 0.00;
	controllers.Roll.kd = 0.00;
	controllers.Roll.ki = 0.00;

	controllers.Pitch.kp = 0.00;
	controllers.Pitch.kd = 0.00;
	controllers.Pitch.ki = 0.00;

	controllers.Yaw.kp = 0.00;
	controllers.Yaw.kd = 0.00;
	controllers.Yaw.ki = 0.00;
}

void QuadCopter::Init(){
  	init_motors();

	direct = 5;
	Update();

	mpu.Init();
	mpu.calibrate();

	nrf.init(CE_PIN, CSN_PIN);
	nrf.setRxMode(nrf.defaultAddr, 0);

	direct = 00;
	Update();
}

void QuadCopter::NRF_Read(bool print){
	nrf.status = nrf.readReg(STATUS);
	if(nrf.isDataAvailable(0)){
		nrf.receive();
		if(print)
			nrf.printData();
	}
}

void QuadCopter::IMU_Update(Madgwick &madgwick, bool print){
	static ZeroVelocityUpdate gyroFilter(5, Vector3(3.0f, 3.0f, 3.0f));
	static uint32_t prev_micros = 0;

	mpu.readAcc();
	mpu.readGyro();
	mpu.readMag();

	dt = System.getDeltaT(prev_micros);
	prev_micros = System.getMicros();



	mpu.gyro = gyroFilter.update(mpu.gyro);
	// mpu.angles = madgwick.update(mpu.acc, mpu.gyro, dt_l);

	// mpu.calculateAngles(dt);
	mpu.q = madgwick.MadgwickQuaternionUpdate(mpu.acc, mpu.gyro, mpu.mag, dt);
	mpu.angles = quat2EulerAngles(mpu.q);
	//convert to degrees back
	mpu.angles.phi *= (180.0f/M_PIf);
	mpu.angles.theta *= (180.0f/M_PIf);
	mpu.angles.psi *= (180.0f/M_PIf);

	if(print)
		mpu.printValues();
}

void QuadCopter::Control() {
	static uint32_t prev_micros = 0;

	dt = System.getDeltaT(prev_micros);
	prev_micros = System.getMicros();

	// controllers.Roll.run(desired_roll, mpu.angles.roll, dt);
	controllers.Pitch.run(desired_pitch, mpu.angles.pitch, dt);
	// controllers.Yaw.run(desired_yaw, mpu.angles.yaw, dt);
	// controllers.Altitude.run(1, mpu.altitude, dt);)
	Update();
}

void QuadCopter::Update() {
	motors[0].setThrust(direct + controllers.Roll.command - controllers.Pitch.command + controllers.Yaw.command);
	motors[1].setThrust(direct + controllers.Roll.command + controllers.Pitch.command - controllers.Yaw.command);
	motors[2].setThrust(direct - controllers.Roll.command + controllers.Pitch.command + controllers.Yaw.command);
	motors[3].setThrust(direct - controllers.Roll.command - controllers.Pitch.command - controllers.Yaw.command);
}