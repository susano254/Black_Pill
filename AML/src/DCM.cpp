#include <DCM.h>
#include <cmath>

namespace AML {
	const Matrix33 DCM::rotationAroundX(float theta){
		float data[3][3] = { 
			{1.0,	 	0.0, 		0.0},
			{0.0, cosf(theta), sinf(theta)},
			{0.0, -sinf(theta), cosf(theta)}
		};

		return Matrix33(data);
	}
	const Matrix33 DCM::rotationAroundY(float phi){
		float data[3][3] = { 
			{cosf(phi), 0.0, -sinf(phi)},
			{	0.0, 	1.0,	 0.0},
			{sinf(phi), 	0.0, cosf(phi)}
		};
		return Matrix33(data);
	}
	const Matrix33 DCM::rotationAroundZ(float thai){
		float data[3][3] = { 
			{cosf(thai), sinf(thai),	0.0},
			{-sinf(thai), cosf(thai),	0.0},
			{	0.0, 	0.0,	 	1.0},
		};
		return Matrix33(data);
	}

	bool isValidDCM(const Matrix33 dcm, float tolerance){
		float det = determinant(dcm);
		//first check it's not a negative value
		if(det <= 0.0) return false;
		//then check it's equal to 1
		float error = det - 1.0;
		//absolute value of the error
		error = error < 0 ? -error : error;
		//if greater than the limit then not a valid dcm
		if(error > tolerance) return false;
		//check the matrix * it's transpose is identity matrix
		if(!(dcm*transpose(dcm) == Matrix33::identity())) return false;

		return true;
	}
	void normalizeDCM(Matrix33 &dcm){
		Vector3 x_component = Vector3(dcm.m11, dcm.m21, dcm.m31);
		Vector3 y_component = Vector3(dcm.m12, dcm.m22, dcm.m32);

		//calculate the error in the directions of the vectors (supposed to be 0)
		//i.e. all rows and columns should be independent
		float error = dot(x_component, y_component);

		//reverse the vector to the other direction of the error
		Vector3 orthogonal_x = x_component - 0.5 * error * y_component;
		Vector3 orthogonal_y = y_component - 0.5 * error * x_component;
		Vector3 orthogonal_z = cross(orthogonal_x, orthogonal_y);

		Vector3 x_norm = 0.5 * (3.0 - dot(orthogonal_x, orthogonal_x))* orthogonal_x;
		Vector3 y_norm = 0.5 * (3.0 - dot(orthogonal_y, orthogonal_y))* orthogonal_y;
		Vector3 z_norm = 0.5 * (3.0 - dot(orthogonal_z, orthogonal_z))* orthogonal_z;

		Matrix33 temp = dcm;
		dcm = Matrix33(x_norm, y_norm, z_norm);
	}

	Matrix33 integrateDCM(const Matrix33 &dcm, const Matrix33 &dcmRates, float dt){
		Matrix33 dcm_update = dcm + dcmRates*dt;
		normalizeDCM(dcm_update);
		return dcm_update;
	}
	Matrix33 dcmKinematicsBodyRates(const Matrix33 &dcm, const Vector3 &w_b){
		const float p = w_b.x;
		const float q = w_b.y;
		const float r = w_b.z;
		const float skew_matrix_data[3][3] = {
			{0.0, -r, q},
			{r, 0.0, -p},
			{-q, p, 0.0}
		};

		const Matrix33 skewMatrix(skew_matrix_data);
		Matrix33 result = -skewMatrix*dcm;
		return result;
	}
	Matrix33 dcmKinematicsWorldRates(const Matrix33 &dcm, const Vector3 &w_w){
		const float p = w_w.x;
		const float q = w_w.y;
		const float r = w_w.z;
		const float skew_matrix_data[3][3] = {
			{0.0, -r, q},
			{r, 0.0, -p},
			{-q, p, 0.0}
		};

		const Matrix33 skewMatrix(skew_matrix_data);
		Matrix33 result = dcm*skewMatrix;
		return result;
	}
}
