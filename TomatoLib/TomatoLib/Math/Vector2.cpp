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
		return (*this) / Length();
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
