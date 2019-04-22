#include "Line.h"

namespace TomatoLib {
	Line::Line(const Vector2& a, const Vector2& b) : A(a), B(b) {}

	bool Line::intersects(const Line & other) const {
		float s1_x, s1_y, s2_x, s2_y, sn, tn, sd, td, t;
		s1_x = this->B.X - this->A.X;     s1_y = this->B.Y - this->A.Y;
		s2_x = other.B.X - other.A.X;     s2_y = other.B.Y - other.A.Y;

		sn = -s1_y * (this->A.X - other.A.X) + s1_x * (this->A.Y - other.A.Y);
		sd = -s2_x * s1_y + s1_x * s2_y;
		tn = s2_x * (this->A.Y - other.A.Y) - s2_y * (this->A.X - other.A.X);
		td = -s2_x * s1_y + s1_x * s2_y;

		if (sn >= 0 && sn <= sd && tn >= 0 && tn <= td) {
			return true;
		}

		return false;
	}

	float Line::length() const {
		return this->A.Distance(this->B);
	}

	Vector2 Line::lerp(float timestep) {
		return this->A.Lerp(this->B, timestep);
	}

	bool Line::operator==(const Line& other) const {
		return this->A == other.A && this->B == other.B;
	}

	bool Line::operator!=(const Line& other) const {
		return !(*this == other);
	}


	// float
	Line Line::operator-(float other) const {
		return { this->A - other, this->B - other };
	}

	Line Line::operator+(float other) const {
		return { this->A + other, this->B + other };
	}

	Line Line::operator/(float other) const {
		return { this->A / other, this->B / other };
	}

	Line Line::operator*(float other) const {
		return { this->A * other, this->B * other };
	}

	Line& Line::operator-=(float other) {
		this->A -= other;
		this->B -= other;

		return *this;
	}

	Line& Line::operator+=(float other) {
		this->A += other;
		this->B += other;

		return *this;
	}

	Line& Line::operator*=(float other) {
		this->A *= other;
		this->B *= other;

		return *this;
	}

	Line& Line::operator/=(float other) {
		this->A /= other;
		this->B /= other;

		return *this;
	}

	// Vector2
	Line Line::operator-(const Vector2& other) const {
		return { this->A - other, this->B - other };
	}

	Line Line::operator+(const Vector2& other) const {
		return { this->A + other, this->B + other };
	}

	Line Line::operator/(const Vector2& other) const {
		return { this->A / other, this->B / other };
	}

	Line Line::operator*(const Vector2& other) const {
		return { this->A * other, this->B * other };
	}

	Line& Line::operator-=(const Vector2& other) {
		this->A -= other;
		this->B -= other;

		return *this;
	}

	Line& Line::operator+=(const Vector2& other) {
		this->A += other;
		this->B += other;

		return *this;
	}

	Line& Line::operator*=(const Vector2& other) {
		this->A *= other;
		this->B *= other;

		return *this;
	}

	Line& Line::operator/=(const Vector2& other) {
		this->A /= other;
		this->B /= other;

		return *this;
	}

	// Vector3
	Line Line::operator-(const Vector3& other) const {
		return { this->A - other, this->B - other };
	}

	Line Line::operator+(const Vector3& other) const {
		return { this->A + other, this->B + other };
	}

	Line Line::operator/(const Vector3& other) const {
		return { this->A / other, this->B / other };
	}

	Line Line::operator*(const Vector3& other) const {
		return { this->A * other, this->B * other };
	}

	Line& Line::operator-=(const Vector3& other) {
		this->A -= other;
		this->B -= other;

		return *this;
	}

	Line& Line::operator+=(const Vector3& other) {
		this->A += other;
		this->B += other;

		return *this;
	}

	Line& Line::operator*=(const Vector3& other) {
		this->A *= other;
		this->B *= other;

		return *this;
	}

	Line& Line::operator/=(const Vector3& other) {
		this->A /= other;
		this->B /= other;

		return *this;
	}

}