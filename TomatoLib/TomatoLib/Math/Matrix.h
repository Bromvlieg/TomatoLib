#pragma once
#ifndef __MATRIX_H__
#define __MATRIX_H__

#include "Vector3.h"
#include "Vector2.h"

namespace TomatoLib {
	class Matrix {
	public:
		static Matrix CreateRotationX(float rotation);
		static Matrix CreateRotationY(float rotation);
		static Matrix CreateRotationZ(float rotation);
		static Matrix CreateScale(float scale);
		static Matrix CreateScale(float scaleX, float scaleY, float scaleZ);
		static Matrix CreateTranslation(float x, float y, float z);
		static Matrix CreateTranslation(const Vector3& pos);
		static Matrix CreateLookAt(float eyex, float eyey, float eyez, float targetx, float targety, float targetz, float upx, float upy, float upz);
		static Matrix CreateLookAt(const Vector3& eyePos, const Vector3& targetPos, const Vector3& up);
		static Matrix CreatePerspective(float fovy, float aspect, float zNear, float zFar);

		~Matrix();
		Matrix();
		Matrix(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44);
		Matrix(float values);

		Matrix(const Matrix& other);
		Matrix(Matrix&& other);

		Matrix& operator= (const Matrix& rhs);

		void DebugPrint();

		Vector3 Translate(float x, float y, float z) const;
		Vector3 Translate(const Vector3& input) const;

		Vector2 Translate(float x, float y) const;
		Vector2 Translate(const Vector2& input) const;

		Vector3 GetTranslation() const;
		Vector3 GetScale() const;

		Matrix Lerp(const Matrix& other, float timestep);

		Matrix operator*(const Matrix& other) const;
		void operator*=(const Matrix& other) { *this = *this * other; }

		float* values;
	};
}

#endif
