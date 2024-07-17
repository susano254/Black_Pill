#ifndef VECTOR3_H
#define VECTOR3_H

namespace AML {
	class Vector3{
		public:
			union {
				float data[3];
				struct{float x, y, z;};
			};
			
		//Constructors
		Vector3();
		Vector3(float val);
		Vector3(float x, float y, float z);
		Vector3(const float data[3]);

		//operators
		//assignments for vector
		Vector3& operator+=(const Vector3& rhs);
		Vector3& operator-=(const Vector3& rhs);
		Vector3& operator*=(const Vector3& rhs);
		Vector3& operator/=(const Vector3& rhs);
		//assignments for scalar
		Vector3& operator+=(float rhs);
		Vector3& operator-=(float rhs);
		Vector3& operator*=(float rhs);
		Vector3& operator/=(float rhs);

		static const Vector3 xAxis();
		static const Vector3 yAxis();
		static const Vector3 zAxis();
	};

	// vector to vector element wise operations
	Vector3 operator-(const Vector3& rhs);
	Vector3 operator+(const Vector3& lhs, const Vector3& rhs);
	Vector3 operator-(const Vector3& lhs, const Vector3& rhs);
	Vector3 operator*(const Vector3& lhs, const Vector3& rhs);
	Vector3 operator/(const Vector3& lhs, const Vector3& rhs);

	// vector to scalar element wise operations
	Vector3 operator+(const Vector3& lhs, float rhs);
	Vector3 operator-(const Vector3& lhs, float rhs);
	Vector3 operator*(const Vector3& lhs, float rhs);
	Vector3 operator/(const Vector3& lhs, float rhs);
	Vector3 operator+(float lhs, const Vector3& rhs);
	Vector3 operator-(float lhs, const Vector3& rhs);
	Vector3 operator*(float lhs, const Vector3& rhs);
	Vector3 operator/(float lhs, const Vector3& rhs);

	float invSqrt(float num);
	Vector3 normalize(Vector3& v);
	//void normalize(Vector3& v);
	float norm(const Vector3& v);
	Vector3 unit(const Vector3& v);
	Vector3 cross(const Vector3& lhs, const Vector3& rhs);
	float dot(const Vector3& lhs, const Vector3& rhs);
};

#endif