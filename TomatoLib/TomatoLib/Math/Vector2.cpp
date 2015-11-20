#include "Vector2.h"
#include <math.h>

namespace TomatoLib {
	const Vector2 Vector2::Zero = Vector2();
	const Vector2 Vector2::One = Vector2(1, 1);

	Vector2::Vector2() :X(0), Y(0) {}
	Vector2::Vector2(float values) : X(values), Y(values) {}
	Vector2::Vector2(float X, float Y) : X(X), Y(Y) {}
	Vector2::Vector2(int X, int Y) : X((float)X), Y((float)Y) {}
	Vector2::Vector2(double X, double Y) : X((float)X), Y((float)Y) {}

	float Vector2::Distance(const Vector2& other) const {
		return sqrt(((this->X - other.X) * (this->X - other.X)) + ((this->Y - other.Y) * (this->Y - other.Y)));
	}

	float Vector2::Length() const {
		return sqrt(pow(X, 2) + pow(Y, 2));
	}

	Vector2 Vector2::Normalized() const {
		float l = Length();
		return l == 0 ? Vector2::Zero : (*this) / l;
	}

	float Vector2::Dot(const Vector2& other) const {
		return X * other.X + Y * other.Y;
	}

	Vector2 Vector2::operator- (const Vector2& other) const {
		return Vector2(X - other.X, Y - other.Y);
	}

	Vector2 Vector2::operator+ (const Vector2& other) const {
		return Vector2(X + other.X, Y + other.Y);
	}

	Vector2 Vector2::operator* (const float& scale) const {
		return Vector2(X * scale, Y * scale);
	}

	Vector2 Vector2::operator/ (const Vector2& other) const {
		return Vector2(X / other.X, Y / other.Y);
	}

	Vector2 Vector2::operator/ (const float& scale) const {
		return Vector2(X / scale, Y / scale);
	}

	Vector2 Vector2::operator+ (const float& scale) const {
		return Vector2(X + scale, Y + scale);
	}

	Vector2 Vector2::Lerp(const Vector2& other, float timestep) {
		Vector2 ret;

		ret.X = this->X + (other.X - this->X) * timestep;
		ret.Y = this->Y + (other.Y - this->Y) * timestep;

		return ret;
	}

	Vector2 Vector2::RotateAroundOrigin(float rads, const Vector2& origin) {
		if (rads == 0) return *this;

		Vector2 u = *this - origin;

		if (u == Vector2::Zero)
			return u;

		float a = atan2f(u.Y, u.X);
		a += rads;

		u = u.Length() * Vector2(cosf(a), sinf(a));
		u.X = u.X + origin.X;
		u.Y = u.Y + origin.Y;

		return u;
	}

	Vector2& Vector2::operator-= (const Vector2& other) {
		X = X - other.X;
		Y = Y - other.Y;
		return *this;
	}

	Vector2& Vector2::operator+= (const Vector2& other) {
		X = X + other.X;
		Y = Y + other.Y;
		return *this;
	}

	Vector2& Vector2::operator*= (const Vector2& other) {
		X = X * other.X;
		Y = Y * other.Y;
		return *this;
	}

	Vector2& Vector2::operator*= (const float& other) {
		X = X * other;
		Y = Y * other;
		return *this;
	}

	Vector2& Vector2::operator+= (const float& other) {
		X = X + other;
		Y = Y + other;
		return *this;
	}

	Vector2& Vector2::operator/= (const Vector2& other) {
		X = X / other.X;
		Y = Y / other.Y;
		return *this;
	}

	Vector2& Vector2::operator/= (const float& other) {
		X = X / other;
		Y = Y / other;
		return *this;
	}

	bool Vector2::operator== (const Vector2& other) const {
		return X == other.X && Y == other.Y;
	}

	bool Vector2::operator!= (const Vector2& other) const {
		return !operator==(other);
	}

	const Vector2 operator*(const Vector2& lhs, const Vector2& rhs) {
		return Vector2(lhs.X * rhs.X, lhs.Y * rhs.Y);
	}
}
