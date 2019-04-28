#pragma once

#include "Vector2.h"

namespace TomatoLib {

	class Line {
	public:
		Vector2 A;
		Vector2 B;

		Line() = default;
		Line(const Vector2& a, const Vector2& b);

		bool intersects(const Line& other) const;
		bool intersects(const Line& other, Vector2& point) const;
		float length() const;
		Vector2 lerp(float timestep);

		bool operator== (const Line& other) const;
		bool operator!= (const Line& other) const;

		Line operator- (const Vector2& other) const;
		Line operator+ (const Vector2& other) const;
		Line operator/ (const Vector2& other) const;
		Line operator* (const Vector2& other) const;
		Line& operator-= (const Vector2& other);
		Line& operator+= (const Vector2& other);
		Line& operator*= (const Vector2& other);
		Line& operator/= (const Vector2& other);

		Line operator- (const Vector3& other) const;
		Line operator+ (const Vector3& other) const;
		Line operator/ (const Vector3& other) const;
		Line operator* (const Vector3& other) const;
		Line& operator-= (const Vector3& other);
		Line& operator+= (const Vector3& other);
		Line& operator*= (const Vector3& other);
		Line& operator/= (const Vector3& other);

		Line operator- (float other) const;
		Line operator+ (float other) const;
		Line operator/ (float other) const;
		Line operator* (float other) const;
		Line& operator-= (float other);
		Line& operator+= (float other);
		Line& operator*= (float other);
		Line& operator/= (float other);

	};
}