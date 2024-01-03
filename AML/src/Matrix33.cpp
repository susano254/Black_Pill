#include <Vector3.h>
#include <Matrix33.h>
#include <cmath>

namespace AML {
	//Constructors
	Matrix33::Matrix33()
	: m11(0.0), m12(0.0), m13(0.0),
	  m21(0.0), m22(0.0), m23(0.0),
	  m31(0.0), m32(0.0), m33(0.0) {}
	Matrix33::Matrix33(float value)
	: m11(value), m12(value), m13(value),
	  m21(value), m22(value), m23(value),
	  m31(value), m32(value), m33(value) {}
	Matrix33::Matrix33(const float data[3][3])
	: m11(data[0][0]), m12(data[0][1]), m13(data[0][2]),
	  m21(data[1][0]), m22(data[1][1]), m23(data[1][2]),
	  m31(data[2][0]), m32(data[2][1]), m33(data[2][2]) {}
	Matrix33::Matrix33(const float data[9])
	: m11(data[0]), m12(data[1]), m13(data[2]),
	  m21(data[3]), m22(data[4]), m23(data[5]),
	  m31(data[6]), m32(data[7]), m33(data[8]) {}
	Matrix33::Matrix33(const Vector3 v1, const Vector3 v2, const Vector3 v3)
	: m11(v1.x), m12(v2.x), m13(v3.x),
	  m21(v1.y), m22(v2.y), m23(v3.y),
	  m31(v1.z), m32(v2.z), m33(v3.z) {}


	//operator assignment (Matrix)
	Matrix33& Matrix33::operator+=(const Matrix33 &rhs){
		for(int i = 0; i < 3; i++){
			for(int j = 0; j < 3; j++)
				data[i][j] += rhs.data[i][j];
		}
		return *this;
	}
	Matrix33& Matrix33::operator-=(const Matrix33 &rhs){
		for(int i = 0; i < 3; i++){
			for(int j = 0; j < 3; j++)
				data[i][j] -= rhs.data[i][j];
		}
		return *this;
	}
	Matrix33& Matrix33::operator*=(const Matrix33 &rhs){
		float m11_temp = m11 * rhs.m11 + m12 * rhs.m21 + m13 * rhs.m31;
		float m12_temp = m11 * rhs.m12 + m12 * rhs.m22 + m13 * rhs.m32;
		float m13_temp = m11 * rhs.m13 + m12 * rhs.m23 + m13 * rhs.m33;
		float m21_temp = m21 * rhs.m11 + m22 * rhs.m21 + m23 * rhs.m31;
		float m22_temp = m21 * rhs.m12 + m22 * rhs.m22 + m23 * rhs.m32;
		float m23_temp = m21 * rhs.m13 + m22 * rhs.m23 + m23 * rhs.m33;
		float m31_temp = m31 * rhs.m11 + m32 * rhs.m21 + m33 * rhs.m31;
		float m32_temp = m31 * rhs.m12 + m32 * rhs.m22 + m33 * rhs.m32;
		float m33_temp = m31 * rhs.m13 + m32 * rhs.m23 + m33 * rhs.m33;

		m11 = m11_temp;
		m12 = m12_temp;
		m13 = m13_temp;
		m21 = m21_temp;
		m22 = m22_temp;
		m23 = m23_temp;
		m31 = m31_temp;
		m32 = m32_temp;
		m33 = m33_temp;

		return *this;
	}
	Matrix33& Matrix33::operator/=(const Matrix33 &rhs){
		*this *= inverse(rhs);
		return *this;
	}

	//operator assignment (Scalar)
	Matrix33& Matrix33::operator+=(float rhs){
		for(int i = 0; i < 3; i++){
			for(int j = 0; j < 3; j++)
				data[i][j] += rhs;
		}
		return *this;
	}
	Matrix33& Matrix33::operator-=(float rhs){
		for(int i = 0; i < 3; i++){
			for(int j = 0; j < 3; j++)
				data[i][j] -= rhs;
		}
		return *this;
	}
	Matrix33& Matrix33::operator*=(float rhs){
		for(int i = 0; i < 3; i++){
			for(int j = 0; j < 3; j++)
				data[i][j] *= rhs;
		}
		return *this;
	}
	Matrix33& Matrix33::operator/=(float rhs){
		for(int i = 0; i < 3; i++){
			for(int j = 0; j < 3; j++)
				data[i][j] /= rhs;
		}
		return *this;
	}

	//special object
	const Matrix33 Matrix33::identity(){
		return Matrix33(Vector3::xAxis(), Vector3::yAxis(), Vector3::zAxis());
	}

	//Matrix/Matrix operations
	Matrix33 operator-(const Matrix33 &rhs){
		float matrix[3][3];
		for(int i = 0; i < 3; i++){
			for(int j = 0; j < 3; j++)
				matrix[i][j] = -rhs.data[i][j];
		}
		return Matrix33(matrix);
	}
	Matrix33 operator+(const Matrix33 &lhs, const Matrix33 &rhs){ return (Matrix33(lhs) += rhs); }
	Matrix33 operator-(const Matrix33 &lhs, const Matrix33 &rhs){ return (Matrix33(lhs) -= rhs); }
	Matrix33 operator*(const Matrix33 &lhs, const Matrix33 &rhs){ return (Matrix33(lhs) *= rhs); }
	Matrix33 operator/(const Matrix33 &lhs, const Matrix33 &rhs){ return (Matrix33(lhs) /= rhs); }

	//Matrix / Vector operations
	Vector3 operator*(const Matrix33 &lhs, const Vector3 &rhs){
		float data[3];
		for(int i = 0; i < 3; i++){
			for(int j = 0; j < 3; j++)
				data[i] += lhs.data[i][j] * rhs.data[j];
		}
		return Vector3(data);
	}

	//Matrix Scalar
  	Matrix33 operator+(const Matrix33 &lhs, float rhs){ return (Matrix33(lhs) += rhs); }
  	Matrix33 operator-(const Matrix33 &lhs, float rhs){ return (Matrix33(lhs) -= rhs); }
  	Matrix33 operator*(const Matrix33 &lhs, float rhs){ return (Matrix33(lhs) *= rhs); }
  	Matrix33 operator/(const Matrix33 &lhs, float rhs){ return (Matrix33(lhs) /= rhs); }
 	Matrix33 operator+(float lhs, const Matrix33 &rhs){ return (Matrix33(lhs) += rhs); }
	Matrix33 operator-(float lhs, const Matrix33 &rhs){ return (Matrix33(lhs) -= rhs); }
	Matrix33 operator*(float lhs, const Matrix33 &rhs){ return (Matrix33(rhs) *= lhs); }
	Matrix33 operator/(float lhs, const Matrix33 &rhs){
		float matrix[3][3];
		for(int i = 0; i < 3; i++){
			for(int j = 0; j < 3; j++)
					matrix[i][j] = lhs / rhs.data[i][j];
		}
		return Matrix33(matrix);
	}


	//Matrix specific helper operations
	Matrix33 transpose(const Matrix33 &rhs){
		float matrix[3][3];
		for(int i = 0; i < 3; i++){
			for(int j = 0; j < 3; j++)
					matrix[i][j] = rhs.data[j][i];
		}
		return Matrix33(matrix);
	}
	float determinant(const Matrix33 &rhs){
		float det = rhs.m11 * (rhs.m22 * rhs.m33 - rhs.m32 * rhs.m23) 
					-rhs.m12 * (rhs.m21 * rhs.m33 - rhs.m31 * rhs.m23)
					+rhs.m13 * (rhs.m21 * rhs.m32 - rhs.m31 * rhs.m22);
		return det;
	}
	Vector3 diag(const Matrix33 &rhs){return Vector3(rhs.m11, rhs.m22, rhs.m33);}
	Matrix33 diag(const Vector3 &rhs){
		return Matrix33(Vector3::xAxis()*rhs, Vector3::yAxis()*rhs, Vector3::zAxis()*rhs);
	}
	Matrix33 inverse(const Matrix33 &rhs){
		float det = determinant(rhs);
		if(fabs(det) > 0){
			float invDet = 1 / det;
			float matrix[3][3];
			matrix[0][0] =  (rhs.m22 * rhs.m33 - rhs.m23 * rhs.m32) * invDet;
			matrix[0][1] = -(rhs.m12 * rhs.m33 - rhs.m13 * rhs.m32) * invDet;
			matrix[0][2] =  (rhs.m12 * rhs.m23 - rhs.m13 * rhs.m22) * invDet;
			matrix[1][0] = -(rhs.m21 * rhs.m33 - rhs.m23 * rhs.m31) * invDet;
			matrix[1][1] =  (rhs.m11 * rhs.m33 - rhs.m13 * rhs.m31) * invDet;
			matrix[1][2] = -(rhs.m11 * rhs.m23 - rhs.m13 * rhs.m21) * invDet;
			matrix[2][0] =  (rhs.m21 * rhs.m32 - rhs.m22 * rhs.m31) * invDet;
			matrix[2][1] = -(rhs.m11 * rhs.m32 - rhs.m12 * rhs.m31) * invDet;
			matrix[2][2] =  (rhs.m11 * rhs.m22 - rhs.m12 * rhs.m21) * invDet;
			return Matrix33(matrix);
		}
		return Matrix33(rhs);
	}

	bool operator==(const Matrix33 &lhs, const Matrix33 &rhs){
		for(int i = 0; i < 3; i++){
			for(int j = 0; j < 3; j++)
				if((lhs.data[i][j] - rhs.data[i][j]) > 0.0001) return false;
		}
		return true;
	}

};