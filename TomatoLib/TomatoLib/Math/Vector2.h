#pragma once
#ifndef __VECTOR2_H__
#define __VECTOR2_H__

namespace TomatoLib {
	class Vector2 {
	public:
		float X, Y;
		Vector2();
		Vector2(float values);
		Vector2(int x, int y);
		Vector2(float x, float y);
		Vector2(double x, double y);

		float Distance(const Vector2& other) const;
		float Length() const;
		Vector2 Normalized() const;
		float Dot(const Vector2& other) const;
		Vector2 RotateAroundOrigin(float rads, const Vector2& origin);
		Vector2 Lerp(const Vector2& other, float timestep);

		Vector2 operator- (const Vector2& other) const;
		Vector2 operator+ (const Vector2& other) const;
		Vector2 operator+ (const float& other) const;
		Vector2 operator* (const float& other) const;
		Vector2 operator/ (const Vector2& other) const;
		Vector2 operator/ (const float& other) const;

		Vector2& operator-= (const Vector2& other);
		Vector2& operator+= (const float& other);
		Vector2& operator+= (const Vector2& other);
		Vector2& operator*= (const Vector2& other);
		Vector2& operator*= (const float& other);
		Vector2& operator/= (const Vector2& other);
		Vector2& operator/= (const float& other);

		bool operator== (const Vector2& other) const;
		bool operator!= (const Vector2& other) const;

		const static Vector2 Zero;
		const static Vector2 One;
	};

	const Vector2 operator*(const Vector2& lhs, const Vector2& rhs);
}
#endif