#include <Quaternion.h>

#define sqrtf(x) (1.0f / invSqrt(x))

namespace AML {
	//Constructors
	Quaternion::Quaternion() 
	: q0(0.0), q1(0.0), q2(0.0), q3(0.0) {}

	Quaternion::Quaternion(float val)
	: q0(val), q1(val), q2(val), q3(val) {}

	Quaternion::Quaternion(const float data[4])
	: q0(data[0]), q1(data[1]), q2(data[2]), q3(data[3]) {}
	Quaternion::Quaternion(float q0, float q1, float q2, float q3)
	: q0(q0), q1(q1), q2(q2), q3(q3) {}
	Quaternion::Quaternion(float scalar, const Vector3 &vector)
	: q0(scalar), q1(vector.x), q2(vector.y), q3(vector.z) {}
	Quaternion::Quaternion(const Vector3 &vector)
	: q0(0.0), q1(vector.x), q2(vector.y), q3(vector.z) {}


	//Assignment operators (Quaternion)
	Quaternion& Quaternion::operator+=(const Quaternion &rhs){
		q0 += rhs.q0;
		q1 += rhs.q1;
		q2 += rhs.q2;
		q3 += rhs.q3;

		return *this;
	}
	Quaternion& Quaternion::operator-=(const Quaternion &rhs){
		q0 -= rhs.q0;
		q1 -= rhs.q1;
		q2 -= rhs.q2;
		q3 -= rhs.q3;

		return *this;
	}
	Quaternion& Quaternion::operator*=(const Quaternion &rhs){
		float q0_new = (q0 * rhs.q0) - (q1 * rhs.q1) - (q2 * rhs.q2) - (q3 * rhs.q3);
		float q1_new = (q1 * rhs.q0) + (q0 * rhs.q1) - (q3 * rhs.q2) + (q2 * rhs.q3);
		float q2_new = (q2 * rhs.q0) + (q3 * rhs.q1) + (q0 * rhs.q2) - (q1 * rhs.q3);
		float q3_new = (q3 * rhs.q0) - (q2 * rhs.q1) + (q1 * rhs.q2) + (q0 * rhs.q3);
		q0 = q0_new;
		q1 = q1_new;
		q2 = q2_new;
		q3 = q3_new;

		return *this;
	}

	//Assignemnt operators (Scalar)
	Quaternion& Quaternion::operator+=(float rhs){
		q0 += rhs;
		q1 += rhs;
		q2 += rhs;
		q3 += rhs;

		return *this;
	}
	Quaternion& Quaternion::operator-=(float rhs){
		q0 -= rhs;
		q1 -= rhs;
		q2 -= rhs;
		q3 -= rhs;

		return *this;
	}
	Quaternion& Quaternion::operator*=(float rhs){
		q0 *= rhs;
		q1 *= rhs;
		q2 *= rhs;
		q3 *= rhs;

		return *this;
	}
	Quaternion& Quaternion::operator/=(float rhs){
		q0 /= rhs;
		q1 /= rhs;
		q2 /= rhs;
		q3 /= rhs;

		return *this;
	}

	//Special needed functions
	const Quaternion Quaternion::identity(){
		return Quaternion(1.0, 0.0, 0.0, 0.0);
	}

	
	//Quaternion and Quaternion operation
	Quaternion operator-(const Quaternion &rhs){ return Quaternion(rhs) *= -1.0; }
	Quaternion operator+(const Quaternion &lhs, const Quaternion &rhs){ return (Quaternion(lhs) += rhs); }
	Quaternion operator-(const Quaternion &lhs, const Quaternion &rhs){ return (Quaternion(lhs) -= rhs); }
	Quaternion operator*(const Quaternion &lhs, const Quaternion &rhs){ return (Quaternion(lhs) *= rhs); }

	//Quaternion and constant operations
	Quaternion operator+(const Quaternion &lhs, float s){ return (Quaternion(lhs) += s); }
	Quaternion operator-(const Quaternion &lhs, float s){ return (Quaternion(lhs) -= s); }
	Quaternion operator*(const Quaternion &lhs, float s){ return (Quaternion(lhs) *= s); }
	Quaternion operator/(const Quaternion &lhs, float s){ return (Quaternion(lhs) /= s); }
	Quaternion operator+(float s, const Quaternion &rhs){ return Quaternion(s + rhs.q0, s + rhs.q1, s + rhs.q2, s + rhs.q3); }
	Quaternion operator-(float s, const Quaternion &rhs){ return Quaternion(s - rhs.q0, s - rhs.q1, s - rhs.q2, s - rhs.q3); }
	Quaternion operator*(float s, const Quaternion &rhs){ return Quaternion(s * rhs.q0, s * rhs.q1, s * rhs.q2, s * rhs.q3); }
	Quaternion operator/(float s, const Quaternion &rhs){ return Quaternion(s / rhs.q0, s / rhs.q1, s / rhs.q2, s / rhs.q3); }

	//Quaternion and vector operations
	Vector3 operator*(const Quaternion &q, const Vector3 &v){ return quat2DCM(q) * v; }

	//Quat operations
	Quaternion conjugate(const Quaternion &q){ return Quaternion(q.q0, -q.q1, -q.q2, -q.q3); }
	float norm(const Quaternion &q){ return sqrtf(q.q0*q.q0 + q.q1*q.q1 + q.q2*q.q2 + q.q3*q.q3); }
	Quaternion inverse(const Quaternion &q){ return conjugate(q)/norm(q); }
	Quaternion unit(const Quaternion &q){
		float mag = norm(q);
		if(mag > 0.0) return (Quaternion(q)/mag);
		return Quaternion(q);
	}
	void normalise(Quaternion &q){ q = unit(q); }
	bool isUnitQuat(const Quaternion &q, float tolerance){ return (fabsf(norm(q) - 1.0f) < 2.0f*tolerance); }
	float dot(const Quaternion &lhs, const Quaternion &rhs){ return (lhs.q0*rhs.q0 + lhs.q1*rhs.q1 + lhs.q2*rhs.q2 + lhs.q3*rhs.q3); }

	//Converions
	Matrix33 quat2DCM(const Quaternion &q){
		const float TOL = 0.0001;
		if(isUnitQuat(q, TOL)){
			float q0 = q.data[0];
			float q1 = q.data[1];
			float q2 = q.data[2];
			float q3 = q.data[3];

			float data[3][3] = {{q0*q0 + q1*q1 - q2*q2 - q3*q3, 2*q1*q2 + 2*q0*q3, 2*q1*q3 - 2*q0*q2},
								{2*q1*q2 - 2*q0*q3, q0*q0 - q1*q1 + q2*q2 - q3*q3, 2*q2*q3 + 2*q0*q1},
								{2*q1*q3 + 2*q0*q2, 2*q2*q3 - 2*q0*q1, q0*q0 - q1*q1 - q2*q2 + q3*q3}};

			return Matrix33(data);
		}
		return Matrix33::identity();
	}
	Quaternion dcm2Quat(const Matrix33 &dcm){
		const float TOL = 0.0001;
		if(isValidDCM(dcm, TOL)){
			float q1_den = (1.0 + dcm.data[0][0] + dcm.data[1][1] + dcm.data[2][2]);
			float q2_den = (1.0 + dcm.data[0][0] - dcm.data[1][1] - dcm.data[2][2]);
			float q3_den = (1.0 - dcm.data[0][0] + dcm.data[1][1] - dcm.data[2][2]);
			float q4_den = (1.0 - dcm.data[0][0] - dcm.data[1][1] + dcm.data[2][2]);
			float r23mr32 = dcm.data[1][2] - dcm.data[2][1];
			float r31mr13 = dcm.data[2][0] - dcm.data[0][2];
			float r12mr21 = dcm.data[0][1] - dcm.data[1][0];
			float r23pr32 = dcm.data[1][2] + dcm.data[2][1];
			float r31pr13 = dcm.data[2][0] + dcm.data[0][2];
			float r12pr21 = dcm.data[0][1] + dcm.data[1][0];

			float den, q0, q1, q2, q3;
			if( q1_den > q2_den && q1_den > q3_den && q1_den > q4_den ){
				den = sqrtf(q1_den);
				q0 = 0.5*den;
				q1 = 0.5*r23mr32/den;
				q2 = 0.5*r31mr13/den;
				q3 = 0.5*r12mr21/den;
			}
			if( q2_den > q1_den && q2_den > q3_den && q2_den > q4_den ){
				den = sqrtf(q2_den);
				q0 = 0.5*r23mr32/den;
				q1 = 0.5*den;
				q2 = 0.5*r12pr21/den;
				q3 = 0.5*r31pr13/den;
			}
			if( q3_den > q1_den && q3_den > q2_den && q3_den > q4_den ){
				den = sqrtf(q3_den);
				q0 = 0.5*r31mr13/den;
				q1 = 0.5*r12pr21/den;
				q2 = 0.5*den;
				q3 = 0.5*r23pr32/den;
			}
			if( q4_den > q1_den && q4_den > q2_den && q4_den > q3_den ){
				den = sqrtf(q4_den);
				q0 = 0.5*r12mr21/den;
				q1 = 0.5*r31pr13/den;
				q2 = 0.5*r23pr32/den;
				q3 = 0.5*den;
			}
			return Quaternion(q0, q1, q2, q3);
		}
		return Quaternion::identity();
	}

	Quaternion euler2Quat(const EulerAngles &attitude){
		switch (attitude.getEulerSequence()) {
			case EulerAngles::EulerSequence::XYZ:
				return euler2Quat_XYZ(attitude);

			//extensible for other sequences
		}
		return Quaternion::identity();
	}

	EulerAngles quat2EulerAngles(const Quaternion &q, const EulerAngles::EulerSequence seq){
		if(isUnitQuat(q, 0.01f)){
			switch(seq){
				case EulerAngles::EulerSequence::XYZ:
					return quat2EulerAngles_XYZ(q);
				//extensible for other sequences
			}
		}
		return EulerAngles();
	}

	//define helpers here

	//euler to quat sequence helpers
	Quaternion euler2Quat_XYZ(const EulerAngles &attitude){
		float c1 = cos(0.5*attitude.data[0]);
		float c2 = cos(0.5*attitude.data[1]);
		float c3 = cos(0.5*attitude.data[2]);
		float s1 = sin(0.5*attitude.data[0]);
		float s2 = sin(0.5*attitude.data[1]);
		float s3 = sin(0.5*attitude.data[2]);
		float q0 = c1*c2*c3 + s1*s2*s3;
		float q1 = s1*c2*c3 - c1*s2*s3;
		float q2 = c1*s2*c3 + s1*c2*s3;
		float q3 = c1*c2*s3 - s1*s2*c3;

		return Quaternion(q0, q1, q2, q3);
	}

	//quat to euler sequence helpers
	EulerAngles quat2EulerAngles_XYZ(const Quaternion &q){
		float q0 = q.data[0];
		float q1 = q.data[1];
		float q2 = q.data[2];
		float q3 = q.data[3];
		float r11 = q0*q0 + q1*q1 - q2*q2 - q3*q3;
		float r12 = 2*q1*q2 + 2*q0*q3;
		float r13 = 2*q1*q3 - 2*q0*q2;
		float r23 = 2*q2*q3 + 2*q0*q1;
		float r33 = q0*q0 - q1*q1 - q2*q2 + q3*q3;
		float phi = atan2(r23, r33);
		float theta = -asin(r13);
		float psi = atan2(r12, r11);

		return EulerAngles(phi, theta, psi);
	}


	//Kinematic functions
	Quaternion integrateQuat(const Quaternion &quat, const Quaternion &quatRates, float dt){
		// q_dot = quatRates
		Quaternion quat_new = quat + quatRates*dt;
		normalise(quat_new);

		return quat_new;
	}
	Quaternion quatKinematicsRates_BodyRates(const Quaternion &quat, const Vector3 &bodyRates){
		float p = bodyRates.x;
		float q = bodyRates.y;
		float r = bodyRates.z;

		float q0 = 0.5 * (-quat.q1 * p - quat.q2 * q - quat.q3 * r);
		float q1 = 0.5 * ( quat.q0 * p + quat.q3 * q - quat.q2 * r);
		float q2 = 0.5 * (-quat.q3 * p + quat.q0 * q + quat.q1 * r);
		float q3 = 0.5 * ( quat.q2 * p - quat.q1 * q + quat.q0 * r);

		return Quaternion(q0, q1, q2, q3);
		
	}
	Quaternion quatKinematicsRates_WorldRates(const Quaternion &quat, const Vector3 &worldRates){
		float p = worldRates.x;
		float q = worldRates.y;
		float r = worldRates.z;

		float q0 = 0.5 * (-quat.q1 * p - quat.q2 * q - quat.q3 * r);
		float q1 = 0.5 * ( quat.q0 * p - quat.q3 * q + quat.q2 * r);
		float q2 = 0.5 * ( quat.q3 * p + quat.q0 * q - quat.q1 * r);
		float q3 = 0.5 * (-quat.q2 * p + quat.q1 * q + quat.q0 * r);

		return Quaternion(q0, q1, q2, q3);
	}

	//interpolation
	Quaternion linearInterpolation(const Quaternion &startQ, const Quaternion &endQ, float t){
		Quaternion q0 = unit(startQ); 	//start quaternion normalized
		Quaternion q1 = unit(endQ); 	//end quaternion normalized
		if(t < 0.0) return q0;
		if (t > 1.0) return q1;
		else return unit((1.0 - t)*q0 + t*q1);
		
	}
	Quaternion slerpInterpolation(const Quaternion &startQ, const Quaternion &endQ, float t){
		Quaternion q0 = unit(startQ); 	//start quaternion normalized
		Quaternion q1 = unit(endQ); 	//end quaternion normalized
		if(t < 0.0) return q0;
		if(t > 1.0) return q1;
		
		float dotProduct = dot(q0, q1);
		if(dotProduct < 0.0){
			q1 = -q1;
			dotProduct = -dotProduct;
		}
		//since unit quaternions dot product is equal to cosine the angle between them 
		float theta = acos(dotProduct);
		//if angle is small just better to linaer interpolate 
		if(theta < 0.0001)
			return linearInterpolation(startQ, endQ, t);
		
		//else spherically linaer interpolate 
		float a = sin((1.0-t)*theta)/sin(theta);
		float b = sin(t*theta)/sin(theta);

		return unit(a*q0+b*q1);
	}

};