#include "Matrix.h"
#define _SCL_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include <stdio.h>
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

	Matrix Matrix::Mirror() {
		Matrix ret;

		for (int x = 0; x < 4; x++) {
			for (int y = 0; y < 4; y++) {
				ret.values[x * 4 + y] = this->values[y * 4 + x];
			}
		}

		return ret;
	}

	Matrix Matrix::Inverted() {
		float num1 = this->values[0];
		float num2 = this->values[1];
		float num3 = this->values[2];
		float num4 = this->values[3];
		float num5 = this->values[4];
		float num6 = this->values[5];
		float num7 = this->values[6];
		float num8 = this->values[7];
		float num9 = this->values[8];
		float num10 = this->values[9];
		float num11 = this->values[10];
		float num12 = this->values[11];
		float num13 = this->values[12];
		float num14 = this->values[13];
		float num15 = this->values[14];
		float num16 = this->values[15];

		float num17 = (num11 * num16 - num12 * num15);
		float num18 = (num10 * num16 - num12 * num14);
		float num19 = (num10 * num15 - num11 * num14);
		float num20 = (num9 * num16 - num12 * num13);
		float num21 = (num9 * num15 - num11 * num13);
		float num22 = (num9 * num14 - num10 * num13);
		float num23 = (num6 * num17 - num7 * num18 + num8 * num19);
		float num24 = -(num5 * num17 - num7 * num20 + num8 * num21);
		float num25 = (num5 * num18 - num6 * num20 + num8 * num22);
		float num26 = -(num5 * num19 - num6 * num21 + num7 * num22);
		float num27 = (1.0f / (num1 * num23 + num2 * num24 + num3 * num25 + num4 * num26));

		float num28 = (num7 * num16 - num8 * num15);
		float num29 = (num6 * num16 - num8 * num14);
		float num30 = (num6 * num15 - num7 * num14);
		float num31 = (num5 * num16 - num8 * num13);
		float num32 = (num5 * num15 - num7 * num13);
		float num33 = (num5 * num14 - num6 * num13);

		float num34 = (num7 * num12 - num8 * num11);
		float num35 = (num6 * num12 - num8 * num10);
		float num36 = (num6 * num11 - num7 * num10);
		float num37 = (num5 * num12 - num8 * num9);
		float num38 = (num5 * num11 - num7 * num9);
		float num39 = (num5 * num10 - num6 * num9);

		Matrix matrix1;
		matrix1.values[0] = num23 * num27;
		matrix1.values[1] = -(num2 * num17 - num3 * num18 + num4 * num19) * num27;
		matrix1.values[2] = (num2 * num28 - num3 * num29 + num4 * num30) * num27;
		matrix1.values[3] = -(num2 * num34 - num3 * num35 + num4 * num36) * num27;
		matrix1.values[4] = num24 * num27;
		matrix1.values[5] = (num1 * num17 - num3 * num20 + num4 * num21) * num27;
		matrix1.values[6] = -(num1 * num28 - num3 * num31 + num4 * num32) * num27;
		matrix1.values[7] = (num1 * num34 - num3 * num37 + num4 * num38) * num27;
		matrix1.values[8] = num25 * num27;
		matrix1.values[9] = -(num1 * num18 - num2 * num20 + num4 * num22) * num27;
		matrix1.values[10] = (num1 * num29 - num2 * num31 + num4 * num33) * num27;
		matrix1.values[11] = -(num1 * num35 - num2 * num37 + num4 * num39) * num27;
		matrix1.values[12] = num26 * num27;
		matrix1.values[13] = (num1 * num19 - num2 * num21 + num3 * num22) * num27;
		matrix1.values[14] = -(num1 * num30 - num2 * num32 + num3 * num33) * num27;
		matrix1.values[15] = (num1 * num36 - num2 * num38 + num3 * num39) * num27;
		return matrix1;
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
		retVal.values[11] = zNear * zFar * rangeInv * 2.0f;
		retVal.values[14] = -1.0;

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
