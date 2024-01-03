#include <MPU.h>

#define M_PIf ((float) M_PI)
using namespace global;

int main(){
	MPU_t mpu;
	float dt = 0.0f;

	System.Init();
	Systick.Init();
	Serial.begin(115200);
	mpu.Init();
	mpu.calibrate();

	//mpu.calibrateAcc();
	//while(1);

	Mahony mahony(2.5, 0.0f);
	Madgwick madgwick(1.0f);
	ZeroVelocityUpdate gyroFilter(5, Vector3(3.0f, 3.0f, 3.0f));

	uint32_t millis = 0;
	uint32_t micros = 0;

	System.reset_cyclic_counter();
	System.enable_cyclic_counter();
	while(1){
		mpu.readAcc();
		mpu.readGyro();
		mpu.readMag();
		
		dt = System.getDeltaT(micros);
		micros = System.getMicros();

		// mpu.calculateAngles(dt);
		// mpu.angles = mahony.update(mpu.acc, mpu.gyro, dt);
		// mpu.angles = madgwick.update(mpu.acc, mpu.gyro, dt);

		// mpu.gyro = gyroFilter.update(mpu.gyro);
		mpu.q = madgwick.MadgwickQuaternionUpdate(mpu.acc, mpu.gyro, mpu.mag, dt); 
		mpu.angles = quat2EulerAngles(mpu.q);
		//convert to degrees back
		mpu.angles.phi *= (180.0f/M_PIf);
		mpu.angles.theta *= (180.0f/M_PIf);
		mpu.angles.psi *= (180.0f/M_PIf);

		mpu.printQuaternion();
		// mpu.printMag();

		// mpu.printAcc();
		// mpu.printAccG();
		// mpu.printVelocity();
		// mpu.printPosition();
		// mpu.printAngles();
		// mpu.printValues();
	}

	return 0;
}