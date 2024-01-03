#include <Vector3.h>
#include <Matrix33.h>
#include <EulerAngles.h>
#include <DCM.h>
#include <cmath>

namespace AML {

	//class is extensible for other sequence of rotations but for now I just used the xyz representation
	EulerAngles::EulerAngles()
	: phi(0.0), theta(0.0), psi(0.0), seq(EulerSequence::XYZ) {}
	EulerAngles::EulerAngles(float phi, float theta, float psi, EulerSequence seq)
	: phi(phi), theta(theta), psi(psi), seq(seq) {}

	Matrix33 euler2DCM(const EulerAngles &angles){
		switch (angles.getEulerSequence()) {
		case EulerAngles::EulerSequence::XYZ:
			return euler2DCM_XYZ(angles);
		default:
			return Matrix33::identity();
		}
	}
	EulerAngles dcm2EulerAngles(const Matrix33 &dcm, const EulerAngles::EulerSequence seq){
		const float tolerance = 0.0001;
		if(isValidDCM(dcm, tolerance)){
			switch (seq) {
			case EulerAngles::EulerSequence::XYZ:
				return dcm2EulerAngles_XYZ(dcm);
			}

		}
		return EulerAngles();
	}
	EulerAngles integrateEulerAngles(const EulerAngles &angles, const EulerAngles &rates, float dt){
		EulerAngles::EulerSequence seq = angles.getEulerSequence();
		if(seq == rates.getEulerSequence()){
			float phi_new = angles.phi + rates.phi*dt;
			float theta_new = angles.theta + rates.theta*dt;
			float psi_new = angles.psi + rates.psi*dt;

			return EulerAngles(phi_new, theta_new, psi_new, seq);
		}

		return angles;
	}
	EulerAngles eulerAnglesKinematics(const EulerAngles &angles, const Vector3 &bodyRates){
		Vector3 eulerRates;
		EulerAngles::EulerSequence seq = angles.getEulerSequence();
		switch (seq) {
		case EulerAngles::EulerSequence::XYZ:
			eulerRates = eulerRatesMatrix_E_XYZ(angles) * bodyRates;
			break;
		}
		return EulerAngles(eulerRates.x, eulerRates.y, eulerRates.z, seq);
	}
	EulerAngles interpolate(const EulerAngles &startAngles, const EulerAngles &endAngles, float t){
		EulerAngles::EulerSequence seq = startAngles.getEulerSequence();
		if(seq == endAngles.getEulerSequence()){
			if(t < 0.0) return startAngles;
			if(t > 1.0) return endAngles;

			float phi_new = (1-t)*startAngles.phi + endAngles.phi*t;
			float theta_new = (1-t)*startAngles.theta + endAngles.theta*t;
			float psi_new = (1-t)*startAngles.psi + endAngles.psi*t;

			return EulerAngles(phi_new, theta_new, psi_new, seq);
		}
		return EulerAngles();
	}
	Matrix33 euler2DCM_XYZ(const EulerAngles &angles){
		float cosPhi = cos(angles.phi);
		float sinPhi = sin(angles.phi);
		float cosTheta = cos(angles.theta);
		float sinTheta = sin(angles.theta);
		float cosPsi = cos(angles.psi);
		float sinPsi = sin(angles.psi);

		float data[3][3]= { 
			{cosTheta*cosPsi, cosTheta*sinPsi, -sinTheta}, 
			{sinPhi*sinTheta*cosPsi - cosPhi*sinPsi, sinPhi*sinTheta*sinPsi + cosPhi*cosPsi, cosTheta*sinPhi},
			{cosPhi*sinTheta*cosPsi + sinPhi*sinPsi, cosPhi*sinTheta*sinPsi - sinPhi*cosPsi, cosTheta*cosPhi}
		};

		return Matrix33(data);
	}
	EulerAngles dcm2EulerAngles_XYZ(const Matrix33 &dcm){
		float phi = atan2(dcm.m23, dcm.m33);
		float theta = -asin(dcm.m13);
		float psi = atan2(dcm.m12, dcm.m11);

		return EulerAngles(phi, theta, psi, EulerAngles::EulerSequence::XYZ);
	}
	Matrix33 eulerRatesMatrix_E_XYZ(const EulerAngles &angles){
		float tanTheta = tan(angles.theta);
		float cosPhi = cos(angles.phi);
		float sinPhi = sin(angles.phi);
		float secTheta = 1.0 / cos(angles.theta);

		float data[3][3] = {
			{1, tanTheta*sinPhi, tanTheta*cosPhi},
			{0, cosPhi, -sinPhi},
			{0, sinPhi*secTheta, cosPhi*secTheta}
		};
		return Matrix33(data);
	}
}