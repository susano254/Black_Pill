#ifndef 	MPU_H
#define 	MPU_H

#include <stm32f401xc.h>
#include <vector>
#include <RCC.h>
#include <USART.h>
#include <math.h>
#include <string.h>
#include <System.h>
#include <AML/AML.h>

#define MPU 		0x68
#define MAG 		0x0C


// #define ACC_FACT	16384.0f	
//#define ACC_FACT	8192.0f	
#define ACC_FACT	4096.0f	
//#define ACC_FACT	2048.0f	

//#define GYRO_FACT	131.0f	
//#define GYRO_FACT	65.5f	
//#define GYRO_FACT	32.8f	
#define GYRO_FACT	16.4f	

#define MAG_FACT	6.6667f
#define CAL_FACT	1000

#define M_PIf ((float) M_PI)
#define sqrtf(x) (1.0f / invSqrt(x))

using namespace AML;
using namespace std;

struct MPU_t {
	bool calibrated = false;
	Quaternion q;

	Vector3 gravity;
	Vector3 velocity, prev_velocity;
	Vector3 position, prev_position;

	Vector3 acc, avgAcc;
	Vector3 accError;

	Vector3 gyro;
	Vector3 gyroError;

	Vector3 mag;
	Vector3 magASA;
	Vector3 magError;
	Vector3 magScale;

	EulerAngles angles;
	// union {
	// 	Vector3 angles;
	// 	struct {
	// 		float roll, pitch, yaw;
	// 	};
		
	// };
	
	MPU_t(){

	}

	void Init();
	void readAcc();
	void readGyro();
	void readMag();
	void calibrate();
	void calibrateAcc();
	void calibrateGyro();
	void calibrateMag();
	void update();


	//helper
	void ZeroVelocityUpdate();
	void removeGravity(Vector3 &acc);
	void calculateAngles(float dt);
	void calculatePosition(float dt);

	//prints
	void printValues();
	void printErrors();
	void printAcc();
	void printGyro();
	void printMag();
	void printAngles();
	void printAccG();
	void printQuaternion();
	void printVelocity();
	void printPosition();

	private:
		void calibrateAccX();
		void calibrateAccY();
		void calibrateAccZ();
};


struct Madgwick {
	float beta, initBeta;
	Quaternion q, offset_q;
	EulerAngles offset_angles;
	Vector3 gravity, error;

	Madgwick() : beta(0.1f), q(1, 0, 0, 0), offset_q(1, 0, 0, 0), error(0.0f) {}
	Madgwick(float beta) : beta(beta), q(1, 0, 0, 0), offset_q(1, 0, 0, 0), error(0.0f) {}
	void init(MPU_t &mpu);
	Quaternion update(Vector3 &acc, Vector3 &gyro, float dt);
	Quaternion update(Vector3 &acc, Vector3 &gyro, Vector3 &mag, float dt);
	Quaternion MadgwickQuaternionUpdate(Vector3 &acc, Vector3 &gyro, Vector3 &mag, float dt);
	// void MadgwickQuaternionUpdate(MPU_t &mpu, float deltat);
	void removeGravity(Vector3 &acc);
};


struct Mahony {
	float kp, ki;
	Quaternion q;
	Vector3 gravity, error;

	Mahony() : kp(0.0f), ki(0.0f), q(1, 0, 0, 0), error(0.0f) {}
	Mahony(float kp, float ki) : kp(kp), ki(ki), q(1, 0, 0, 0), error(0.0f) {}
	Quaternion update(Vector3 &acc, Vector3 &gyro, float dt);
	Quaternion update(Vector3 &acc, Vector3 &gyro, Vector3 &mag, float dt);
	void printQuaternion();
};

struct FIFO {
	vector<Vector3> buffer;
	int size;

	FIFO() : size(10) {}
	FIFO(int size) : size(size) {}

	Vector3 getAverage(){
		Vector3 avg;
		for(int i = 0; i < buffer.size(); i++){
			avg += buffer[i];
		}
		avg /= (float) size;
		return avg;
	}

	void push_back(Vector3 v){
		buffer.push_back(v);
		if(buffer.size() > size){
			buffer.erase(buffer.begin());
		}
	}
};

struct ZeroVelocityUpdate {
	FIFO fifo;
	Vector3 th;

	ZeroVelocityUpdate(int n){
		fifo = FIFO(n);
	}
	ZeroVelocityUpdate(int n, Vector3 th){
		fifo = FIFO(n);
		this->th = th;
	}
	Vector3 update(Vector3 &v, Vector3 threshold);
	Vector3 update(Vector3 &v);
};

// #ifdef __cplusplus
// extern "C" {
// #endif
// 	MPU_t* getMpu();
// 	void mpu_init();
// 	void readAcc();
// 	void readGyro();
// 	void readMag();
// 	void printValues();
// 	void calibrate();
// 	void calculateAngles(float dt);
// #ifdef __cplusplus
// }
// #endif

#endif