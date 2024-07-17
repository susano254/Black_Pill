#ifndef MATRIX33_H
#define MATRIX33_H

#include "Vector3.h"

namespace AML {
	class Matrix33 {
		public:
			//matrix data 
			union {
				float data[3][3];
				struct{float m11, m12, m13, m21, m22, m23, m31, m32, m33;};
			};
			//Constructors
			Matrix33();
			explicit Matrix33(float value);
			explicit Matrix33(const float data[3][3]);
			explicit Matrix33(const float data[9]);
			explicit Matrix33(const Vector3 v1, const Vector3 v2, const Vector3 v3);


			//operator assignment (Matrix)
			Matrix33& operator+=(const Matrix33 &rhs);
			Matrix33& operator-=(const Matrix33 &rhs);
			Matrix33& operator*=(const Matrix33 &rhs);
			Matrix33& operator/=(const Matrix33 &rhs);

			//operator assignment (Scalar)
			Matrix33& operator+=(float rhs);
			Matrix33& operator-=(float rhs);
			Matrix33& operator*=(float rhs);
			Matrix33& operator/=(float rhs);

			//special object
			static const Matrix33 identity();
	};

	//Matrix/Matrix operations
	bool operator==(const Matrix33 &lhs, const Matrix33 &rhs);
	Matrix33 operator-(const Matrix33 &rhs);
	Matrix33 operator+(const Matrix33 &lhs, const Matrix33 &rhs);
	Matrix33 operator-(const Matrix33 &lhs, const Matrix33 &rhs);
	Matrix33 operator*(const Matrix33 &lhs, const Matrix33 &rhs);
	Matrix33 operator/(const Matrix33 &lhs, const Matrix33 &rhs);

	//Matrix / Vector operations
	Vector3 operator*(const Matrix33 &lhs, const Vector3 &rhs);

	//Matrix Scalar
	Matrix33 operator+(const Matrix33 &lhs, float rhs);
	Matrix33 operator-(const Matrix33 &lhs, float rhs);
	Matrix33 operator*(const Matrix33 &lhs, float rhs);
	Matrix33 operator/(const Matrix33 &lhs, float rhs);
	Matrix33 operator+(float lhs, const Matrix33 &rhs);
	Matrix33 operator-(float lhs, const Matrix33 &rhs);
	Matrix33 operator*(float lhs, const Matrix33 &rhs);
	Matrix33 operator/(float lhs, const Matrix33 &rhs);


	//Matrix specific helper operations
	Vector3 diag(const Matrix33 &rhs);
	Matrix33 diag(const Vector3 &diagonalVector);
	Matrix33 transpose(const Matrix33 &rhs);
	float determinant(const Matrix33 &rhs);
	Matrix33 inverse(const Matrix33 &rhs);
};

#endif