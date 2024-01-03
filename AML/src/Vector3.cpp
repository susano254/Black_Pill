#include <Vector3.h>
#include <cmath>
#include <cstdint>

#define correctionFactor 	0x5f3759df

		
namespace AML {
	//Constructors
	Vector3::Vector3() : x(0.0), y(0.0), z(0.0){}
	Vector3::Vector3(float val) : x(val), y(val), z(val) {}
	Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
	Vector3::Vector3(const float data[3]) : x(data[0]), y(data[1]), z(data[2]) {}

	//operators
	//assignments for vector
	Vector3& Vector3::operator+=(const Vector3& rhs){
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}
	Vector3& Vector3::operator-=(const Vector3& rhs){
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}
	Vector3& Vector3::operator*=(const Vector3& rhs){
		x *= rhs.x;
		y *= rhs.y;
		z *= rhs.z;
		return *this;
	}
	Vector3& Vector3::operator/=(const Vector3& rhs){
		x /= rhs.x;
		y /= rhs.y;
		z /= rhs.z;
		return *this;
	}

	Vector3& Vector3::operator+=(float rhs){
		x += rhs;
		y += rhs;
		z += rhs;
		return *this;
	}
	Vector3& Vector3::operator-=(float rhs){
		x -= rhs;
		y -= rhs;
		z -= rhs;
		return *this;
	}
	Vector3& Vector3::operator*=(float rhs){
		x *= rhs;
		y *= rhs;
		z *= rhs;
		return *this;
	}
	Vector3& Vector3::operator/=(float rhs){
		x /= rhs;
		y /= rhs;
		z /= rhs;
		return *this;
	}

	const Vector3 Vector3::xAxis(){ return Vector3(1.0, 0.0, 0.0); }
	const Vector3 Vector3::yAxis(){ return Vector3(0.0, 1.0, 0.0); }
	const Vector3 Vector3::zAxis(){ return Vector3(0.0, 0.0, 1.0); }

	// vector to vector element wise operations
	Vector3 operator-(const Vector3& rhs){ return Vector3(-rhs.x, -rhs.y, -rhs.z); }
	Vector3 operator+(const Vector3& lhs, const Vector3& rhs){ return (Vector3(lhs) += rhs); }
	Vector3 operator-(const Vector3& lhs, const Vector3& rhs){ return (Vector3(lhs) -= rhs); }
	Vector3 operator*(const Vector3& lhs, const Vector3& rhs){ return (Vector3(lhs) *= rhs); }
	Vector3 operator/(const Vector3& lhs, const Vector3& rhs){ return (Vector3(lhs) /= rhs); }

	// vector to scalar element wise operations
	Vector3 operator+(const Vector3& lhs, float rhs){ return (Vector3(lhs) += rhs); }
	Vector3 operator-(const Vector3& lhs, float rhs){ return (Vector3(lhs) -= rhs); }
	Vector3 operator*(const Vector3& lhs, float rhs){ return (Vector3(lhs) *= rhs); }
	Vector3 operator/(const Vector3& lhs, float rhs){ return (Vector3(lhs) /= rhs); }
	Vector3 operator+(float lhs, const Vector3& rhs){ return (Vector3(lhs) += rhs); }
	Vector3 operator-(float lhs, const Vector3& rhs){ return (Vector3(lhs) -= rhs); }
	Vector3 operator*(float lhs, const Vector3& rhs){ return (Vector3(lhs) *= rhs); }
	Vector3 operator/(float lhs, const Vector3& rhs){ return (Vector3(lhs) /= rhs); }

	float invSqrt(float num){
		int32_t i;
		float invSqrtEstimate;

		invSqrtEstimate = num;
		i = *(int32_t *) &invSqrtEstimate;				//take the bit representation as it's into an int value
		//taking the invserse square root of a number represents negating the exponent and diving it by 2
		//so (-1* (num/2^23 - 127)/2 + 127 )*2^23 
		//where 23 is the mantissa length and 127 is the bias in IEEE 754 (to allow for negative exponents)
		//doing so corrupts the mantissa part as dividing by 2^23 is the same as shifting by right
		//so simplifying to eliminate the division we get:
		//(1.5 * 127 * 2^23 - num/2) the value to the left is: 0x5F400000 a number very close to our correctionfactor
		//however since we are approximating log(1+m) =~ m + u
		//using u to shift the approximation to minimize the error around 0.5 (max error) is equal to around 0.043

		//the bit representation of the number is IEE 754 representation of (-1)^s * (1 + M/2^23) * 2^(E - 127)
		i = correctionFactor - (i >> 1);	
		invSqrtEstimate = *(float *) &i; 					//reverse the bits back to a float value
		//finally use newton's methods to optimize the estimate
		//newGuess = oldGuess - f(y)/f`(y)
		//f(y) is the cost function = 1/invSqrtEstimate^2 - num = 0
		//simplifying we get newGuess = 1/2 * (3 - num * oldGuess^2)*oldGuess
		invSqrtEstimate = (1.5f - 0.5 * num * invSqrtEstimate * invSqrtEstimate)*invSqrtEstimate;
		//apply second time optionally for better estimate
		invSqrtEstimate = (1.5f - 0.5 * num * invSqrtEstimate * invSqrtEstimate)*invSqrtEstimate;

		return (float) invSqrtEstimate;
	}

	float norm(const Vector3& v){ 
		float invMag = invSqrt(dot(v, v));
		return (float) 1.0/invMag;
	}
	Vector3 normalize(Vector3& v){
		float invMag = invSqrt(dot(v, v));
		return v * invMag;
	}
	Vector3 unit(const Vector3& v){
		float mag = norm(v);
		if(mag > 0.0) return (Vector3(v) / mag);
		else 		  return Vector3(v);
	}
	Vector3 cross(const Vector3& lhs, const Vector3& rhs){
		float x = (lhs.y * rhs.z) - (rhs.y * lhs.z);
		float y = (lhs.z * rhs.x) - (lhs.x * rhs.z);
		float z = (lhs.x * rhs.y) - (rhs.x * lhs.y);
		return Vector3(x, y, z);
	}
	float dot(const Vector3& lhs, const Vector3& rhs){
		return (lhs.x * rhs.x +  lhs.y * rhs.y + lhs.z * rhs.z);
	}
};