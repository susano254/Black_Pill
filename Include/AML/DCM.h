#ifndef DCM_H
#define DCM_H

#include "Matrix33.h"
#include "Vector3.h"

namespace AML {
	class DCM {
		public:
			static const Matrix33 rotationAroundX(float theta);
			static const Matrix33 rotationAroundY(float phi);
			static const Matrix33 rotationAroundZ(float thai);
	};

	bool isValidDCM(const Matrix33 dcm, float tolerance);
	void normalizeDCM(Matrix33 &dcm);

	Matrix33 integrateDCM(const Matrix33 &dcm, const Matrix33 &dcmRates, float dt);
	Matrix33 dcmKinematicsBodyRates(const Matrix33 &dcm, const Vector3 &w_b);
	Matrix33 dcmKinematicsWorldRates(const Matrix33 &dcm, const Vector3 &w_w);

};

#endif