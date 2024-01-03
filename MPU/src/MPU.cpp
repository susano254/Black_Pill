#include <I2C.h>
#include <MPU.h>
#include <GPIO.h>
#include <RCC.h>
#include <USART.h>
#include <math.h>
#include <System.h>
#include <AML/AML.h>
#include <string>
#include <vector>



using namespace AML;
using namespace global;

I2C_Handle_t i2c_handle;
// MPU_t mpu;

#define toDeg(x)	((x*180.0f)/M_PI)
#define toRad(x)	((x*M_PI)/180.0f)
// madgwick_t *madgwick;


void MPU_t::Init(){
	// madgwick = getMadgwick();
	// MadgwickInit(0.5, 0.2);
	//configure pins 
	gpio_pin_conf_t pinB6, pinB7;
	pinB6.pin = PB6;
	pinB7.pin = PB7;
	//set mode
	pinB6.mode = GPIO_PIN_ALT_FUN_MODE;
	pinB7.mode = GPIO_PIN_ALT_FUN_MODE;
	//output type
	pinB6.op_type = GPIO_PIN_OP_TYPE_OPEN_DRAIN;
	pinB7.op_type = GPIO_PIN_OP_TYPE_OPEN_DRAIN;
	////set pullups for pin 9 and 10
	pinB6.pupd = GPIO_PIN_PUPD_NO_PUPD;
	pinB7.pupd = GPIO_PIN_PUPD_NO_PUPD;
	//set speed
	pinB6.speed = GPIO_PIN_OSPEED_VERY_HIGH;
	pinB7.speed = GPIO_PIN_OSPEED_VERY_HIGH;
	//set alternate function
	pinB6.alternate_function = 4;
	pinB7.alternate_function = 4;

	//enable clock for GPIOA
	RCC_GPIOB_CLK_ENABLE();
	gpio_init(GPIOB, &pinB6);
	gpio_init(GPIOB, &pinB7);

	RCC_I2C1_CLK_ENABLE();
	i2c_handle.instance = I2C1;
 	I2CInit(&i2c_handle);


	Serial.printStrln("Starting Init sequence of MPU");
	uint8_t buffer[6];

	//init sequence
	buffer[0] = 0x80;
	i2c_master_tx(&i2c_handle, MPU, 0x6B, buffer, 1);
	Serial.printStrln("first succesful transmit");
	for(int i = 0; i < 1000; i++);

	buffer[0] = 0x00;
	i2c_master_tx(&i2c_handle, MPU, 0x6B, buffer, 1);
	for(int i = 0; i < 1000; i++);

	buffer[0] = 0x01;
	i2c_master_tx(&i2c_handle, MPU, 0x6B, buffer, 1);
	for(int i = 0; i < 1000; i++);

	//set gyro range to maximum of 2000 degrees/second
	buffer[0] = (3 << 3);
	i2c_master_tx(&i2c_handle, MPU, 0x1B, buffer, 1);

	//set acc range to be 8g
	buffer[0] = (2 << 3);
	i2c_master_tx(&i2c_handle, MPU, 0x1C, buffer, 1);

	//set acc digital low pass filter (the higher the value = more delay)
	buffer[0] = 0x6;
	i2c_master_tx(&i2c_handle, MPU, 0x1D, buffer, 1);

	//enable bypass multiplexer to access magnetometer and bmp
	buffer[0] = 0x02;
	i2c_master_tx(&i2c_handle, MPU, 0x37, buffer, 1);
	for(int i = 0; i < 800000; i++);


	/*************Init the MAG************************/
	// //just for test to see if it reads the device id correctly
	i2c_master_rx(&i2c_handle, MAG, 0x00, buffer, 1);
	buffer[0] = 0x1f;
	i2c_master_tx(&i2c_handle, MAG, 0x0A, buffer, 1);

	i2c_master_rx(&i2c_handle, MAG, 0x10, buffer, 3);
	magASA.data[0] = ((buffer[0] - 128)/256.0f) + 1.0f;
	magASA.data[1] = ((buffer[1] - 128)/256.0f) + 1.0f;
	magASA.data[2] = ((buffer[2] - 128)/256.0f) + 1.0f;

	buffer[0] = 0x00;
	i2c_master_tx(&i2c_handle, MAG, 0x0A, buffer, 1);

	buffer[0] = 0x16;
	i2c_master_tx(&i2c_handle, MAG, 0x0A, buffer, 1);
}

void MPU_t::calibrate(){
	Serial.printStrln("Calibrating....");
	for(int i = 0; i < CAL_FACT; i++){
		readGyro();

		gyroError.data[0] += gyro.data[0];
		gyroError.data[1] += gyro.data[1];
		gyroError.data[2] += gyro.data[2];
	}

	gyroError.data[0] = gyroError.data[0]/CAL_FACT;
	gyroError.data[1] = gyroError.data[1]/CAL_FACT;
	gyroError.data[2] = gyroError.data[2]/CAL_FACT;

	magError.data[0] = 10.467f;
	magError.data[1] = 22.5f;
	magError.data[2] = 44.486f;

	magScale.data[0] = 1.053f;
	magScale.data[1] = 0.9695f;
	magScale.data[2] = 0.9806f;

	calibrated = true;
}

void MPU_t::calibrateAccX(){
	float offset;
	float x_sum = 0;
	float y_sum = 0;
	float x_squared_sum = 0;
	float xy_sum = 0;

	//calibrate z axis
	Serial.printStrln("Hold sensor up x-axis up right");
	Serial.recieve();
	for(int i = 0; i < CAL_FACT; i++){
		readAcc();

		offset = acc.data[0] - 1;
		x_sum += 1;
		y_sum += offset;
		x_squared_sum += 1;
		xy_sum += 1 * offset;
	}
	Serial.printStrln("Hold sensor down x-axis upside down");
	Serial.recieve();
	for(int i = 0; i < CAL_FACT; i++){
		readAcc();

		offset = acc.data[0] + 1;
		x_sum += -1;
		y_sum += offset;
		x_squared_sum += 1;
		xy_sum += -1 * offset;
	}
	Serial.printStrln("Hold sensor side x-axis his side facing down");
	Serial.recieve();
	for(int i = 0; i < CAL_FACT; i++){
		readAcc();

		offset = acc.data[0];
		// x_sum += 0;
		y_sum += offset;
		// x_squared_sum += 0;
		// xy_sum += 0 * offset;
	}

	float n = 3*CAL_FACT;
	float m = (n*xy_sum - x_sum*y_sum)/(n*x_squared_sum - x_sum*x_sum);
	float b = (y_sum - m*x_sum)/n;
	Serial.printStr("m = ");
	Serial.printFloat(m);
	Serial.printStr(",\tb = ");
	Serial.printFloat(b);
	Serial.printStrln("");
	Serial.printStrln("done");

}
void MPU_t::calibrateAccY(){
	float offset;
	float x_sum = 0;
	float y_sum = 0;
	float x_squared_sum = 0;
	float xy_sum = 0;

	//calibrate z axis
	Serial.printStrln("Hold sensor up y-axis up right");
	Serial.recieve();
	for(int i = 0; i < CAL_FACT; i++){
		readAcc();

		offset = acc.data[1] - 1;
		x_sum += 1;
		y_sum += offset;
		x_squared_sum += 1;
		xy_sum += 1 * offset;
	}
	Serial.printStrln("Hold sensor down y-axis upside down");
	Serial.recieve();
	for(int i = 0; i < CAL_FACT; i++){
		readAcc();

		offset = acc.data[1] + 1;
		x_sum += -1;
		y_sum += offset;
		x_squared_sum += 1;
		xy_sum += -1 * offset;
	}
	Serial.printStrln("Hold sensor side y-axis his side facing down");
	Serial.recieve();
	for(int i = 0; i < CAL_FACT; i++){
		readAcc();

		offset = acc.data[1];
		// x_sum += 0;
		y_sum += offset;
		// x_squared_sum += 0;
		// xy_sum += 0 * offset;
	}

	float n = 3*CAL_FACT;
	float m = (n*xy_sum - x_sum*y_sum)/(n*x_squared_sum - x_sum*x_sum);
	float b = (y_sum - m*x_sum)/n;
	Serial.printStr("m = ");
	Serial.printFloat(m);
	Serial.printStr(",\tb = ");
	Serial.printFloat(b);
	Serial.printStrln("");
	Serial.printStrln("done");

}
void MPU_t::calibrateAccZ(){
	float offset;
	float x_sum = 0;
	float y_sum = 0;
	float x_squared_sum = 0;
	float xy_sum = 0;

	//calibrate z axis
	Serial.printStrln("Hold sensor up z-axis up right");
	Serial.recieve();
	for(int i = 0; i < CAL_FACT; i++){
		readAcc();

		offset = acc.data[2] - 1;
		x_sum += 1;
		y_sum += offset;
		x_squared_sum += 1;
		xy_sum += 1 * offset;
	}
	Serial.printStrln("Hold sensor down z-axis upside down");
	Serial.recieve();
	for(int i = 0; i < CAL_FACT; i++){
		readAcc();

		offset = acc.data[2] + 1;
		x_sum += -1;
		y_sum += offset;
		x_squared_sum += 1;
		xy_sum += -1 * offset;
	}
	Serial.printStrln("Hold sensor side z-axis his side facing down");
	Serial.recieve();
	for(int i = 0; i < CAL_FACT; i++){
		readAcc();

		offset = acc.data[2];
		// x_sum += 0;
		y_sum += offset;
		// x_squared_sum += 0;
		// xy_sum += 0 * offset;
	}

	float n = 3*CAL_FACT;
	float m = (n*xy_sum - x_sum*y_sum)/(n*x_squared_sum - x_sum*x_sum);
	float b = (y_sum - m*x_sum)/n;
	Serial.printStr("m = ");
	Serial.printFloat(m);
	Serial.printStr(",\tb = ");
	Serial.printFloat(b);
	Serial.printStrln("");
	Serial.printStrln("done");

}

void MPU_t::calibrateAcc() {
	calibrateAccX();
	calibrateAccY();
	calibrateAccZ();
}

void MPU_t::readAcc() {
	uint8_t buffer[6];
	i2c_master_rx(&i2c_handle, MPU, 0x3B, buffer, 6);

	acc.data[0] = ((int16_t) ((buffer[0] << 8) | buffer[1]))  / ACC_FACT;
	acc.data[1] = ((int16_t) ((buffer[2] << 8) | buffer[3]))  / ACC_FACT;
	acc.data[2] = ((int16_t) ((buffer[4] << 8) | buffer[5]))  / ACC_FACT;

	if(calibrated){
		accError.data[0] = -0.030f*acc.data[0] - 0.028f;
		accError.data[1] = -0.0f*acc.data[1] + 0.019f;
		accError.data[2] = -0.006f*acc.data[2] + 0.246f;

		acc.data[0] -= accError.data[0];
		acc.data[1] -= accError.data[1];
		acc.data[2] -= accError.data[2];
	}

}

void MPU_t::readGyro(){
	uint8_t buffer[6];
	i2c_master_rx(&i2c_handle, MPU, 0x43, buffer, 6);

	gyro.data[0] = ((int16_t) ((buffer[0] << 8) | buffer[1])) / GYRO_FACT;
	gyro.data[1] = ((int16_t) ((buffer[2] << 8) | buffer[3])) / GYRO_FACT;
	gyro.data[2] = ((int16_t) ((buffer[4] << 8) | buffer[5])) / GYRO_FACT;

	if(calibrated){
		gyro.data[0] -= gyroError.data[0];
		gyro.data[1] -= gyroError.data[1];
		gyro.data[2] -= gyroError.data[2];
	}
}

void MPU_t::readMag(){
	uint8_t buffer[6];
	i2c_master_rx(&i2c_handle, MAG, 0x02, buffer, 1);
	uint8_t st1 = buffer[0];
	uint8_t reg_val;

	if( !(st1 & (1 << 0)) ) return;

	i2c_master_rx(&i2c_handle, MAG, 0x03, buffer, 7);

	// for(int i = 0, reg_addr = 0x03; i < 7; i++, reg_addr++){
	// 	i2c_master_rx(&i2c_handle, MAG, reg_addr, &reg_val, 1);
	// 	buffer[i] = reg_val;
	// }

	mag.data[0] = ((int16_t) ((buffer[1] << 8) | buffer[0]) / MAG_FACT) * magASA.data[0];
	mag.data[1] = ((int16_t) ((buffer[3] << 8) | buffer[2]) / MAG_FACT) * magASA.data[1];
	mag.data[2] = ((int16_t) ((buffer[5] << 8) | buffer[4]) / MAG_FACT) * magASA.data[2];

	//change axis to align with the acc and gyro
	float temp = mag.data[0];
	mag.data[0] = mag.data[1];
	mag.data[1] = temp;
	mag.data[2] = -mag.data[2];

	uint8_t st2 = buffer[6];
	if(st2 & (1 << 3))
		Serial.printStrln("Error magnetometer overflow");

	if(calibrated){
		mag.data[0] = (mag.data[0] - magError.data[0]) * magScale.data[0];
		mag.data[1] = (mag.data[1] - magError.data[1]) * magScale.data[1];
		mag.data[2] = (mag.data[2] - magError.data[2]) * magScale.data[2];
	}
}

Vector3 ZeroVelocityUpdate::update(Vector3 &v, Vector3 threshold){
	Vector3 filteredV;

	fifo.push_back(v);
	filteredV = fifo.getAverage();

	if(abs(filteredV.data[0]) < threshold.data[0]) filteredV.data[0] = 0;
	if(abs(filteredV.data[1]) < threshold.data[1]) filteredV.data[1] = 0;
	if(abs(filteredV.data[2]) < threshold.data[2]) filteredV.data[2] = 0;

	return filteredV;
}
Vector3 ZeroVelocityUpdate::update(Vector3 &v){
	Vector3 filteredV;

	fifo.push_back(v);
	filteredV = fifo.getAverage();

	if(abs(filteredV.data[0]) < th.data[0]) filteredV.data[0] = 0;
	if(abs(filteredV.data[1]) < th.data[1]) filteredV.data[1] = 0;
	if(abs(filteredV.data[2]) < th.data[2]) filteredV.data[2] = 0;

	return filteredV;
}

void MPU_t::ZeroVelocityUpdate() {
	static FIFO accFifo(5);
	accFifo.push_back(acc);

	avgAcc = accFifo.getAverage();

	if(abs(avgAcc.data[0]) < 0.01f) avgAcc.data[0] = 0;
	if(abs(avgAcc.data[1]) < 0.01f) avgAcc.data[1] = 0;
	if(abs(avgAcc.data[2]) < 0.01f) avgAcc.data[2] = 0;

	if(avgAcc.data[0] == 0) {
		velocity.data[0] = 0;
		prev_velocity.data[0] = 0;
	}
	if(avgAcc.data[1] == 0){ 
		velocity.data[1] = 0;
		prev_velocity.data[1] = 0;
	}
	if(avgAcc.data[2] == 0){
		velocity.data[2] = 0;
		prev_velocity.data[2] = 0;
	}
}

void MPU_t::calculateAngles(float dt) {
	float accRoll, accPitch, magYaw;
	float roll, pitch, yaw;
	roll = angles.data[0];
	pitch = angles.data[1];
	yaw = angles.data[2];

	Vector3 acc_n = normalize(acc);
	float ax, ay, az;
	float gx, gy, gz;
	float mx, my, mz;
	ax = acc.data[0];
	ay = acc.data[1];
	az = acc.data[2];

	gx = gyro.data[0];
	gy = gyro.data[1];
	gz = gyro.data[2];

	mx = mag.data[0];
	my = mag.data[1];
	mz = mag.data[2];

	accRoll = atan2f(ay, sqrtf(ax*ax + az*az));
	accRoll = toDeg(accRoll);
	roll = 0.96f*(roll + gx*dt) + 0.04f*accRoll;
	float radRoll = toRad(roll);

	//accPitch = atan2(-ax, ay*sinf(radRoll) + az*cosf(radRoll) );
	accPitch = atan2f(-ax, sqrtf(ay*ay + az*az));
	accPitch = toDeg(accPitch);
	pitch = 0.96f*(pitch + gy*dt) + 0.04f*accPitch;
	float radPitch = toRad(pitch);

	magYaw = atan2f(mz*sinf(radRoll) - my*cosf(radRoll), mx*cosf(radPitch) + my*sinf(radPitch)*sinf(radRoll) + mz*sinf(radPitch)*cosf(radRoll));
	magYaw = toDeg(magYaw);

	if(magYaw > 175 || magYaw < -175)
		yaw = magYaw;
	else
		yaw = 0.96f*(yaw + gz*dt) + 0.04f*magYaw;

	angles.data[0] = roll;
	angles.data[1] = pitch;
	// angles.data[2] = yaw;
	angles.data[2] += gz*dt;
}
void MPU_t::calculatePosition(float dt) {
	Vector3 acc_g = avgAcc*981;

	// Integrate linear acceleration to obtain velocity
    velocity = prev_velocity + acc_g * dt;

    // Integrate velocity to obtain position
    position = prev_position + velocity * dt;

	prev_velocity = velocity;
	prev_position = position;
}


Quaternion Mahony::update(Vector3 &acc, Vector3 &gyro_degrees, float dt) {
	Vector3 gyro = gyro_degrees * (M_PI/180.0f);

	//computre the estimated gravity vector from the quaternion
	float data[] = {2*(q.q1*q.q3 - q.q0*q.q2), 2*(q.q0*q.q1 + q.q2*q.q3), 1 - 2*q.q1*q.q1 - 2*q.q2*q.q2};
	Vector3 v(data);

	Vector3 acc_n = normalize(acc);
	//get the incremental error by cross product between measured and estimated
	Vector3 delta_error = cross(acc_n, v);

	//compute the whole orientation error
	error = error + delta_error * dt;
	gyro = gyro + kp*delta_error + ki*error;


	//feed back gyro degrees for next iteration
	gyro_degrees = gyro * (180.0f/M_PI);

	Quaternion qdot = quatKinematicsRates_BodyRates(q, gyro);
	q = integrateQuat(q, qdot, dt);
	
	return q;
}

void Madgwick::init(MPU_t &mpu) {
}

Quaternion Madgwick::update(Vector3 &acc, Vector3 &gyro_degrees, float dt) {
	// convert gyro degrees measurements to rads
	Vector3 gyro = gyro_degrees * (M_PI/180.0f);
	// normalize acc
	Vector3 acc_n = normalize(acc);

	//computre the estimated gravity vector from the quaternion
	// estimated g = q' * g * q where g = (0, 0, 0, 1) I mean pointing downward 
	Vector3 v(2*(q.q1*q.q3 - q.q0*q.q2), 2*(q.q0*q.q1 + q.q2*q.q3), 1 - 2*q.q1*q.q1 - 2*q.q2*q.q2);
	gravity = v;

	// error is the difference between the estimated gravity vector when rotated
	// by quaternion subtracted by the actual vector of accelerometer
	Vector3 F_g = v - acc_n;

	//define the jacobian
	float J_g[3][4] = { 
		{-2*q.q2, 2*q.q3, -2*q.q0, 2*q.q1},
		{2*q.q1, 2*q.q0, 2*q.q3, 2*q.q2},
		{	0  , -4*q.q1, -4*q.q2,	0  }
	};

	//define gradient to be the jacobian * costfunction
	Quaternion gradient;
 	// now computer the gradient gradient = J_g'*F_g (J_g' means transpose)
    gradient.q0 = J_g[0][0] * F_g.data[0] + J_g[1][0] * F_g.data[1] + J_g[2][0] * F_g.data[2];
    gradient.q1 = J_g[0][1] * F_g.data[0] + J_g[1][1] * F_g.data[1] + J_g[2][1] * F_g.data[2];
    gradient.q2 = J_g[0][2] * F_g.data[0] + J_g[1][2] * F_g.data[1] + J_g[2][2] * F_g.data[2];
    gradient.q3 = J_g[0][3] * F_g.data[0] + J_g[1][3] * F_g.data[1] + J_g[2][3] * F_g.data[2];
	normalise(gradient);

	// get the direction to be the normalized gradient * beta
	// the direction needs to be opposite to the gradient in grad descent so its negative
	Quaternion q_delta = -beta*gradient;

	//get gyro increment orientation
	Quaternion qw_dot = quatKinematicsRates_BodyRates(q, gyro);

	//Fuse the measurments from both the acc and gyro to obtain the estimated attitude 
	Quaternion qdot = qw_dot + q_delta;

	q = integrateQuat(q, qdot, dt);

	// removeGravity(acc);

	return q;
}

Quaternion Madgwick::update(Vector3 &acc, Vector3 &gyro_degrees, Vector3 &mag, float dt) {
	// normalize acc, mag
	Vector3 acc_n = normalize(acc);
	Vector3 mag_n = normalize(mag);

	Quaternion q_t = conjugate(q);
	Quaternion m(mag_n);

	// convert gyro degrees measurements to rads
	Vector3 gyro = gyro_degrees * (M_PI/180.0f);

	//computre the estimated gravity vector from the quaternion
	// estimated g = q' * g * q where g = (0, 0, 0, 1) I mean pointing downward 
	Vector3 vg(2*(q.q1*q.q3 - q.q0*q.q2), 2*(q.q0*q.q1 + q.q2*q.q3), 1 - 2*q.q1*q.q1 - 2*q.q2*q.q2);

	// error is the difference between the estimated gravity vector when rotated
	// by quaternion subtracted by the actual vector of accelerometer
	Vector3 F_g = vg - acc_n;

	//define the jacobian
	float J_g[3][4] = { 
		{-2*q.q2, 2*q.q3, -2*q.q0, 2*q.q1},
		{2*q.q1, 2*q.q0, 2*q.q3, 2*q.q2},
		{	0  , -4*q.q1, -4*q.q2,	0  }
	};


	// compute estimated north mag vector from quaternion
	// estimated north = q * mag * q'
	Quaternion ht = q * m * q_t;
	Vector3 bt(sqrtf(ht.q1*ht.q1 + ht.q2*ht.q2), 0, ht.q3);
	Vector3 vb(
		bt.x*(1 - 2*q.qy*q.qy - 2*q.qz*q.qz) + bt.z*(2*q.qx*q.qz - 2*q.qw*q.qy),
		bt.x*(2*q.qx*q.qy - 2*q.qw*q.qz) + bt.z*(2*q.qw*q.qx + 2*q.qy*q.qz),
		bt.x*(2*q.qw*q.qy + 2*q.qx*q.qz) + bt.z*(1 - 2*q.qx*q.qx - 2*q.qy*q.qy)
	);

	// error is the difference between the estimated north vector when rotated
	// by quaternion subtracted by the actual vector of mag
	Vector3 F_b = vb - mag;

	//define the jacobian
	float J_b[3][4] = { 
		{-2*bt.z*q.qy, 2*bt.z*q.qz, -4*bt.x*q.qy - 2*bt.z*q.qw, -4*bt.x*q.qz + 2*bt.z*q.qx},
		{-2*bt.x*q.qz + 2*bt.z*q.qx, 2*bt.x*q.qy + 2*bt.z*q.qw, 2*bt.x*q.qx + 2*bt.z*q.qz, -2*bt.x*q.qw + 2*bt.z*q.qy},
		{-2*bt.x*q.qy, 2*bt.x*q.qz - 4*bt.z*q.qx, 2*bt.x*q.qw - 4*bt.z*q.qy, 2*bt.x*q.qx},
	};


	//define gradient to be the jacobian * costfunction
	Quaternion gradient;
 	// now computer the gradient gradient = J_g'*F_g (J_g' means transpose)
    gradient.q0 = J_g[0][0] * F_g.data[0] + J_g[1][0] * F_g.data[1] + J_g[2][0] * F_g.data[2];
    gradient.q1 = J_g[0][1] * F_g.data[0] + J_g[1][1] * F_g.data[1] + J_g[2][1] * F_g.data[2];
    gradient.q2 = J_g[0][2] * F_g.data[0] + J_g[1][2] * F_g.data[1] + J_g[2][2] * F_g.data[2];
    gradient.q3 = J_g[0][3] * F_g.data[0] + J_g[1][3] * F_g.data[1] + J_g[2][3] * F_g.data[2];
	normalise(gradient);

	// get the direction to be the normalized gradient * beta
	// the direction needs to be opposite to the gradient in grad descent so its negative
	Quaternion q_delta = -beta*gradient;

	//get gyro increment orientation
	Quaternion qw_dot = quatKinematicsRates_BodyRates(q, gyro);

	//Fuse the measurments from both the acc and gyro to obtain the estimated attitude 
	Quaternion qdot = qw_dot + q_delta;

	q = integrateQuat(q, qdot, dt);

	// removeGravity(acc);

	// EulerAngles angles = quat2EulerAngles(q);
	// //convert to degrees back
	// angles.phi *= (180.0f/M_PI);
	// angles.theta *= (180.0f/M_PI);
	// angles.psi *= (180.0f/M_PI);

	return q;
}


// AHRS algorithm update

Quaternion Madgwick::MadgwickQuaternionUpdate(Vector3 &acc, Vector3 &gyro_degrees, Vector3 &mag, float deltat) {
	// convert gyro degrees measurements to rads
	Vector3 gyro = gyro_degrees * (M_PI/180.0f);

	float ax = acc.x, ay = acc.y, az = acc.z;
	float gx = gyro.x, gy = gyro.y, gz = gyro.z;
	float mx = mag.x, my = mag.y, mz = mag.z;

    float q1 = q.q0, q2 = q.q1, q3 = q.q2, q4 = q.q3;   // short name local variable for readability

    float norm;
    float hx, hy, _2bx, _2bz;
    float s1, s2, s3, s4;
    float qDot1, qDot2, qDot3, qDot4;

    // Auxiliary variables to avoid repeated arithmetic
    float _2q1mx;
    float _2q1my;
    float _2q1mz;
    float _2q2mx;
    float _4bx;
    float _4bz;
    float _2q1 = 2.0f * q1;
    float _2q2 = 2.0f * q2;
    float _2q3 = 2.0f * q3;
    float _2q4 = 2.0f * q4;
    float _2q1q3 = 2.0f * q1 * q3;
    float _2q3q4 = 2.0f * q3 * q4;
    float q1q1 = q1 * q1;
    float q1q2 = q1 * q2;
    float q1q3 = q1 * q3;
    float q1q4 = q1 * q4;
    float q2q2 = q2 * q2;
    float q2q3 = q2 * q3;
    float q2q4 = q2 * q4;
    float q3q3 = q3 * q3;
    float q3q4 = q3 * q4;
    float q4q4 = q4 * q4;

    // Normalise mpu.accelerometer measurement
    norm = 1.0f / sqrtf(ax * ax + ay * ay + az * az);
    ax *= norm;
    ay *= norm;
    az *= norm;

    // Normalise magnetometer measurement
    norm = 1.0f / sqrtf(mx * mx + my * my + mz * mz);
    mx *= norm;
    my *= norm;
    mz *= norm;

    // Reference direction of Earth's magnetic field
    _2q1mx = 2.0f * q1 * mx;
    _2q1my = 2.0f * q1 * my;
    _2q1mz = 2.0f * q1 * mz;
    _2q2mx = 2.0f * q2 * mx;
    hx = mx * q1q1 - _2q1my * q4 + _2q1mz * q3 + mx * q2q2 + _2q2 * my * q3 + _2q2 * mz * q4 - mx * q3q3 - mx * q4q4;
    hy = _2q1mx * q4 + my * q1q1 - _2q1mz * q2 + _2q2mx * q3 - my * q2q2 + my * q3q3 + _2q3 * mz * q4 - my * q4q4;
    _2bx = sqrtf(hx * hx + hy * hy);
    _2bz = -_2q1mx * q3 + _2q1my * q2 + mz * q1q1 + _2q2mx * q4 - mz * q2q2 + _2q3 * my * q4 - mz * q3q3 + mz * q4q4;
    _4bx = 2.0f * _2bx;
    _4bz = 2.0f * _2bz;

    // Gradient decent algorithm corrective step
    s1 = -_2q3 * (2.0f * q2q4 - _2q1q3 - ax) + _2q2 * (2.0f * q1q2 + _2q3q4 - ay) - _2bz * q3 * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (-_2bx * q4 + _2bz * q2) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + _2bx * q3 * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    s2 = _2q4 * (2.0f * q2q4 - _2q1q3 - ax) + _2q1 * (2.0f * q1q2 + _2q3q4 - ay) - 4.0f * q2 * (1.0f - 2.0f * q2q2 - 2.0f * q3q3 - az) + _2bz * q4 * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (_2bx * q3 + _2bz * q1) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + (_2bx * q4 - _4bz * q2) * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    s3 = -_2q1 * (2.0f * q2q4 - _2q1q3 - ax) + _2q4 * (2.0f * q1q2 + _2q3q4 - ay) - 4.0f * q3 * (1.0f - 2.0f * q2q2 - 2.0f * q3q3 - az) + (-_4bx * q3 - _2bz * q1) * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (_2bx * q2 + _2bz * q4) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + (_2bx * q1 - _4bz * q3) * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    s4 = _2q2 * (2.0f * q2q4 - _2q1q3 - ax) + _2q3 * (2.0f * q1q2 + _2q3q4 - ay) + (-_4bx * q4 + _2bz * q2) * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (-_2bx * q1 + _2bz * q3) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + _2bx * q2 * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    norm = sqrtf(s1 * s1 + s2 * s2 + s3 * s3 + s4 * s4);    // normalise step magnitude
    norm = 1.0f/norm;
    s1 *= norm;
    s2 *= norm;
    s3 *= norm;
    s4 *= norm;

    // Compute rate of change of quaternion
    qDot1 = 0.5f * (-q2 * gx - q3 * gy - q4 * gz) - beta * s1;
    qDot2 = 0.5f * (q1 * gx + q3 * gz - q4 * gy) - beta * s2;
    qDot3 = 0.5f * (q1 * gy - q2 * gz + q4 * gx) - beta * s3;
    qDot4 = 0.5f * (q1 * gz + q2 * gy - q3 * gx) - beta * s4;

    // Integrate to yield quaternion
    q1 += qDot1 * deltat;
    q2 += qDot2 * deltat;
    q3 += qDot3 * deltat;
    q4 += qDot4 * deltat;
    norm = sqrtf(q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4);    // normalise quaternion
    norm = 1.0f/norm;

    q.q0 = q1 * norm;
    q.q1 = q2 * norm;
    q.q2 = q3 * norm;
    q.q3 = q4 * norm;

	// removeGravity(mpu.acc);

	// EulerAngles angles = quat2EulerAngles(q);
	// //convert to degrees back
	// angles.phi *= (180.0f/M_PIf);
	// angles.theta *= (180.0f/M_PIf);
	// angles.psi *= (180.0f/M_PIf);

	return q;
}

void Madgwick::removeGravity(Vector3 &acc) { 
	// estimated g = q' * g * q where g = (0, 0, 0, 1) I mean pointing downward 
	Vector3 v(2*(q.q1*q.q3 - q.q0*q.q2), 2*(q.q0*q.q1 + q.q2*q.q3), 1 - 2*q.q1*q.q1 - 2*q.q2*q.q2);
	gravity = v;
	acc = acc - gravity;
}

void MPU_t::printValues() {
	string s;
	s.append(Serial.floatToStr(acc.data[0]));
	s.append("\t");
	s.append(Serial.floatToStr(acc.data[1]));
	s.append("\t");
	s.append(Serial.floatToStr(acc.data[2]));
	s.append("\t");
	s.append(Serial.floatToStr(gyro.data[0]));
	s.append("\t");
	s.append(Serial.floatToStr(gyro.data[1]));
	s.append("\t");
	s.append(Serial.floatToStr(gyro.data[2]));
	s.append("\t");
	s.append(Serial.floatToStr(mag.data[0]));
	s.append("\t");
	s.append(Serial.floatToStr(mag.data[1]));
	s.append("\t");
	s.append(Serial.floatToStr(mag.data[2]));
	s.append("\t");
	s.append(Serial.floatToStr(angles.data[0]));
	s.append("\t");
	s.append(Serial.floatToStr(angles.data[1]));
	s.append("\t");
	s.append(Serial.floatToStr(angles.data[2]));
	Serial.printStrln((char*) s.c_str());
}

void MPU_t::printAcc(){
	string s;
	s.append(Serial.floatToStr(acc.data[0]));
	s.append("\t");
	s.append(Serial.floatToStr(acc.data[1]));
	s.append("\t");
	s.append(Serial.floatToStr(acc.data[2]));
	s.append("\t");
	Serial.printStrln((char*) s.c_str());
}

void MPU_t::printGyro(){
	string s;
	s.append(Serial.floatToStr(gyro.data[0]));
	s.append("\t");
	s.append(Serial.floatToStr(gyro.data[1]));
	s.append("\t");
	s.append(Serial.floatToStr(gyro.data[2]));
	Serial.printStrln((char*) s.c_str());
}

void MPU_t::printMag(){
	string s;
	s.append(Serial.floatToStr(mag.data[0]));
	s.append("\t");
	s.append(Serial.floatToStr(mag.data[1]));
	s.append("\t");
	s.append(Serial.floatToStr(mag.data[2]));
	Serial.printStrln((char*) s.c_str());
}

void MPU_t::printAngles(){
	string s;
	s.append(Serial.floatToStr(angles.data[0]));
	s.append("\t");
	s.append(Serial.floatToStr(angles.data[1]));
	s.append("\t");
	s.append(Serial.floatToStr(angles.data[2]));
	Serial.printStrln((char*) s.c_str());
}

void MPU_t::printAccG() {
	string s;
	s.append(Serial.floatToStr(avgAcc.data[0]*981));
	s.append("\t");
	s.append(Serial.floatToStr(avgAcc.data[1]*981));
	s.append("\t");
	s.append(Serial.floatToStr(avgAcc.data[2]*981));
	s.append("\t");
	Serial.printStr((char*) s.c_str());
}

void MPU_t::printVelocity(){
	string s;
	s.append(Serial.floatToStr(velocity.data[0]));
	s.append("\t");
	s.append(Serial.floatToStr(velocity.data[1]));
	s.append("\t");
	s.append(Serial.floatToStr(velocity.data[2]));
	s.append("\t");
	Serial.printStr((char*) s.c_str());
}
void MPU_t::printPosition(){
	string s;
	s.append(Serial.floatToStr(position.data[0]));
	s.append("\t");
	s.append(Serial.floatToStr(position.data[1]));
	s.append("\t");
	s.append(Serial.floatToStr(position.data[2]));
	Serial.printStrln((char*) s.c_str());
}

void MPU_t::printQuaternion() {
	string s;
	s.append(Serial.floatToStr(q.qw));
	s.append("\t");
	s.append(Serial.floatToStr(q.qx));
	s.append("\t");
	s.append(Serial.floatToStr(q.qy));
	s.append("\t");
	s.append(Serial.floatToStr(q.qz));
	Serial.printStrln((char*) s.c_str());
}

void MPU_t::printErrors() {
	Serial.printFloat(accError.data[0]);
	Serial.printStr(",\t");
	Serial.printFloat(accError.data[1]);
	Serial.printStr(",\t");
	Serial.printFloat(accError.data[2]);
	Serial.printStr(",\t\t");
	Serial.printFloat(gyroError.data[0]);
	Serial.printStr(",\t");
	Serial.printFloat(gyroError.data[1]);
	Serial.printStr(",\t");
	Serial.printFloat(gyroError.data[2]);
	Serial.sendByte('\n');
}

