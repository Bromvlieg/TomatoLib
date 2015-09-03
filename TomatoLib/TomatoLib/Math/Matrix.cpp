#include "Matrix.h"
#define _SCL_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>

namespace TomatoLib {
	Matrix::Matrix() {
		values = new float[16]{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
	}

	Matrix::~Matrix() {
		delete[] values;
	}

	Matrix::Matrix(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44) {
		values = new float[16]{m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34, m41, m42, m43, m44};
	}

	Matrix::Matrix(float v) {
		values = new float[16]{v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v};
	}

	Matrix::Matrix(const Matrix& other) {
		values = new float[16];
		std::copy(other.values, &other.values[16], values);
	}

	Matrix::Matrix(Matrix&& other) {
		values = other.values;
		other.values = nullptr;
	}

	Matrix Matrix::Lerp(const Matrix& o, float timestep) {
		Matrix ret;

		for (int i = 0; i < 16; i++) {
			ret.values[i] = this->values[i] + (o.values[i] - this->values[i]) * timestep;
		}

		return ret;
	}

	Matrix& Matrix::operator= (const Matrix& rhs) {
		if (&rhs == this) { return *this; }
		std::copy(rhs.values, &rhs.values[16], values);
		return *this;
	}

	Matrix Matrix::CreateRotationX(float rotation) {
		Matrix retVal;
		retVal.values[5] = cos(rotation);
		retVal.values[6] = -sin(rotation);
		retVal.values[9] = sin(rotation);
		retVal.values[10] = cos(rotation);
		return retVal;
	}

	Matrix Matrix::CreateRotationY(float rotation) {
		Matrix retVal;
		retVal.values[0] = cos(rotation);
		retVal.values[2] = sin(rotation);
		retVal.values[8] = -sin(rotation);
		retVal.values[10] = cos(rotation);
		return retVal;
	}

	Matrix Matrix::CreateRotationZ(float rotation) {
		Matrix retVal;
		retVal.values[0] = cos(rotation);
		retVal.values[1] = -sin(rotation);
		retVal.values[4] = sin(rotation);
		retVal.values[5] = cos(rotation);
		return retVal;
	}

	Matrix Matrix::CreateScale(float scale) {
		return Matrix::CreateScale(scale, scale, scale);
	}

	Matrix Matrix::CreateScale(float x, float y, float z) {
		Matrix retVal;
		retVal.values[0] = x;
		retVal.values[5] = y;
		retVal.values[10] = z;
		return retVal;
	}

	Matrix Matrix::CreateTranslation(float x, float y, float z) {
		Matrix retVal;
		retVal.values[3] = x;
		retVal.values[7] = y;
		retVal.values[11] = z;
		return retVal;
	}

	Matrix Matrix::CreateTranslation(const Vector3& pos) {
		return CreateTranslation(pos.X, pos.Y, pos.Z);
	}

	Vector3 Matrix::GetTranslation() const {
		return this->Translate(Vector3::Zero);
	}

	Vector3 Matrix::GetScale() const {
		return Vector3(values[0], values[5], values[10]);
	}

	Matrix Matrix::CreateLookAt(float eyex, float eyey, float eyez, float targetx, float targety, float targetz, float upx, float upy, float upz) {
		return CreateLookAt(Vector3(eyex, eyey, eyez), Vector3(targetx, targety, targetz), Vector3(upx, upy, upz));
	}

	Matrix Matrix::CreateLookAt(const Vector3& eyePos, const Vector3& targetPos, const Vector3& upPos) {
		Vector3 zAxis = (eyePos - targetPos).Normalized();
		Vector3 xAxis = upPos.Cross(zAxis).Normalized();
		Vector3 yAxis = zAxis.Cross(xAxis).Normalized();

		Matrix orientation = Matrix(
			xAxis.X, yAxis.X, zAxis.X, 0,
			xAxis.Y, yAxis.Y, zAxis.Y, 0,
			xAxis.Z, yAxis.Z, zAxis.Z, 0,
			0, 0, 0, 1
			);

		Matrix translation = Matrix(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			-eyePos.X, -eyePos.Y, -eyePos.Z, 1
			);

		// Combine the orientation and translation to compute the view matrix
		return (translation * orientation);
	}

	Matrix Matrix::CreatePerspective(float fovy, float aspect, float zNear, float zFar) {
		float f = float(tan(M_PI_2 - 0.5f * fovy));
		float rangeInv = 1.0f / (zNear - zFar);

		Matrix retVal(0);

		retVal.values[0] = f / aspect;
		retVal.values[5] = f;
		retVal.values[10] = (zNear + zFar) * rangeInv;
		retVal.values[11] = -1.0;
		retVal.values[14] = zNear * zFar * rangeInv * 2.0f;

		return retVal;
	}

	Vector3 Matrix::Translate(float x, float y, float z) const {
		Vector3 retVal = Vector3();
		retVal.X = values[0] * x + values[1] * y + values[2] * z + values[3];
		retVal.Y = values[4] * x + values[5] * y + values[6] * z + values[7];
		retVal.Z = values[8] * x + values[9] * y + values[10] * z + values[11];
		return retVal;
	}

	Vector3 Matrix::Translate(const Vector3& input) const {
		return Translate(input.X, input.Y, input.Z);
	}

	Vector2 Matrix::Translate(float x, float y) const {
		Vector2 retVal;
		retVal.X = values[0] * x + values[1] * y + values[2];
		retVal.Y = values[4] * x + values[5] * y + values[6];
		return retVal;
	}

	Vector2 Matrix::Translate(const Vector2& input) const {
		return Translate(input.X, input.Y);
	}

	Matrix Matrix::operator*(const Matrix& other) const {
		Matrix retVal;

		retVal.values[0] = values[0] * other.values[0] + values[1] * other.values[4] + values[2] * other.values[8] + values[3] * other.values[12];
		retVal.values[1] = values[0] * other.values[1] + values[1] * other.values[5] + values[2] * other.values[9] + values[3] * other.values[13];
		retVal.values[2] = values[0] * other.values[2] + values[1] * other.values[6] + values[2] * other.values[10] + values[3] * other.values[14];
		retVal.values[3] = values[0] * other.values[3] + values[1] * other.values[7] + values[2] * other.values[11] + values[3] * other.values[15];

		retVal.values[4] = values[4] * other.values[0] + values[5] * other.values[4] + values[6] * other.values[8] + values[7] * other.values[12];
		retVal.values[5] = values[4] * other.values[1] + values[5] * other.values[5] + values[6] * other.values[9] + values[7] * other.values[13];
		retVal.values[6] = values[4] * other.values[2] + values[5] * other.values[6] + values[6] * other.values[10] + values[7] * other.values[14];
		retVal.values[7] = values[4] * other.values[3] + values[5] * other.values[7] + values[6] * other.values[11] + values[7] * other.values[15];

		retVal.values[8] = values[8] * other.values[0] + values[9] * other.values[4] + values[10] * other.values[8] + values[11] * other.values[12];
		retVal.values[9] = values[8] * other.values[1] + values[9] * other.values[5] + values[10] * other.values[9] + values[11] * other.values[13];
		retVal.values[10] = values[8] * other.values[2] + values[9] * other.values[6] + values[10] * other.values[10] + values[11] * other.values[14];
		retVal.values[11] = values[8] * other.values[3] + values[9] * other.values[7] + values[10] * other.values[11] + values[11] * other.values[15];

		retVal.values[12] = values[12] * other.values[0] + values[13] * other.values[4] + values[14] * other.values[8] + values[15] * other.values[12];
		retVal.values[13] = values[12] * other.values[1] + values[13] * other.values[5] + values[14] * other.values[9] + values[15] * other.values[13];
		retVal.values[14] = values[12] * other.values[2] + values[13] * other.values[6] + values[14] * other.values[10] + values[15] * other.values[14];
		retVal.values[15] = values[12] * other.values[3] + values[13] * other.values[7] + values[14] * other.values[11] + values[15] * other.values[15];

		return retVal;
	}

	void Matrix::DebugPrint() {
		printf("Matrix object at %p\n", this);
		printf("m11: %+f m12: %+f m13: %+f m14: %+f \n"
			   "m21: %+f m22: %+f m23: %+f m24: %+f \n"
			   "m31: %+f m32: %+f m33: %+f m34: %+f \n"
			   "m41: %+f m42: %+f m43: %+f m44: %+f \n",
			   values[0], values[1], values[2], values[3],
			   values[4], values[5], values[6], values[7],
			   values[8], values[9], values[10], values[11],
			   values[12], values[13], values[14], values[15]
			   );
	}
}
