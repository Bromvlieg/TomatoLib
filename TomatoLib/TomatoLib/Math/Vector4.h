#pragma once
#ifndef _TOMATOLIB_VECTOR4_H_
#define _TOMATOLIB_VECTOR4_H_


namespace TomatoLib {
	class Color;
	class Vector2;
	class Vector3;

	struct Vector4 {
		float X, Y, Z, W;

		Vector4() = default;
		Vector4(float x, float y = 0, float z = 0, float w = 0) : X(x), Y(y), Z(z), W(w) {}
		Vector4(const Vector2& vec);
		Vector4(const Vector3& vec);
		Vector4(const Color& col);

		void operator *= (float denom);
		void operator /= (float denom);
		void operator -= (float denom);
		void operator += (float denom);

		void operator *= (const Vector4& other);
		void operator /= (const Vector4& other);
		void operator -= (const Vector4& other);
		void operator += (const Vector4& other);

		Vector4 operator* (float mult) const;
		Vector4 operator/ (float mult) const;
		Vector4 operator+ (float mult) const;
		Vector4 operator- (float mult) const;

		Vector4 operator* (const Vector4& other) const;
		Vector4 operator/ (const Vector4& other) const;
		Vector4 operator+ (const Vector4& other) const;
		Vector4 operator- (const Vector4& other) const;
		bool operator== (const Vector4& other) const;
		bool operator!= (const Vector4& other) const;
	};
}

#endif