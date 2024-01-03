#ifndef QUATERNION_H
#define QUATERNION_H

#include "Vector3.h"
#include "Matrix33.h"
#include "EulerAngles.h"
#include "DCM.h"
#include <limits>
#include <cmath>

namespace AML {
	class Quaternion {
		public:
			//Data
			union {
				float data[4];
				struct {
					float q0, q1, q2, q3;
				};
				struct {
					float qw, qx, qy, qz;
				};
			};


			//Constructors
			Quaternion();
			explicit Quaternion(float val);
			explicit Quaternion(const float data[4]);
			explicit Quaternion(float q0, float q1, float q2, float q3);
			explicit Quaternion(float scalar, const Vector3 &vector);
			explicit Quaternion(const Vector3 &vector);


			//Assignment operators (Quaternion)
			Quaternion& operator+=(const Quaternion &rhs);
			Quaternion& operator-=(const Quaternion &rhs);
			Quaternion& operator*=(const Quaternion &rhs);

			//Assignemnt operators (Scalar)
			Quaternion& operator+=(float rhs);
			Quaternion& operator-=(float rhs);
			Quaternion& operator*=(float rhs);
			Quaternion& operator/=(float rhs);

			//Special needed functions
			static const Quaternion identity();
			
	};
	
	//Quaternion and Quaternion operation
	Quaternion operator-(const Quaternion &rhs);
	Quaternion operator+(const Quaternion &lhs, const Quaternion &rhs);
	Quaternion operator-(const Quaternion &lhs, const Quaternion &rhs);
	Quaternion operator*(const Quaternion &lhs, const Quaternion &rhs);

	//Quaternion and constant operations
	Quaternion operator+(const Quaternion &lhs, float s);
	Quaternion operator-(const Quaternion &lhs, float s);
	Quaternion operator*(const Quaternion &lhs, float s);
	Quaternion operator/(const Quaternion &lhs, float s);
	Quaternion operator+(float s, const Quaternion &lhs);
	Quaternion operator-(float s, const Quaternion &lhs);
	Quaternion operator*(float s, const Quaternion &lhs);
	Quaternion operator/(float s, const Quaternion &lhs);

	//Quaternion and vector operations
	Vector3 operator*(const Quaternion &q, const Vector3 &v);


	//Quat operations
	Quaternion conjugate(const Quaternion &q);
	float norm(const Quaternion &q);
	void normalise(Quaternion &q);
	Quaternion inverse(const Quaternion &q);
	Quaternion unit(const Quaternion &q);
	bool isUnitQuat(const Quaternion &q, float tolerance = std::numeric_limits<float>::epsilon());
	float dot(const Quaternion &lhs, const Quaternion &rhs);

	//Converions
	Matrix33 quat2DCM(const Quaternion &q);
	Quaternion dcm2Quat(const Matrix33 &dcm);
	EulerAngles quat2EulerAngles(const Quaternion &q, const EulerAngles::EulerSequence seq = EulerAngles::EulerSequence::XYZ);
	Quaternion euler2Quat(const EulerAngles &angles);

	//define helpers here

	//euler to quat sequence helpers
	Quaternion euler2Quat_XYZ(const EulerAngles &angles);

	//quat to euler sequence helpers
	EulerAngles quat2EulerAngles_XYZ(const Quaternion &q);


	//Kinematic functions
	Quaternion integrateQuat(const Quaternion &quat, const Quaternion &quatRates, float dt);
	Quaternion quatKinematicsRates_BodyRates(const Quaternion &q, const Vector3 &bodyRates);
	Quaternion quatKinematicsRates_WorldRates(const Quaternion &q, const Vector3 &worldRates);

	//interpolation
	Quaternion linearInterpolation(const Quaternion &startQ, const Quaternion &endQ, float t);
	Quaternion slerpInterpolation(const Quaternion &startQ, const Quaternion &endQ, float t);
};

#endif