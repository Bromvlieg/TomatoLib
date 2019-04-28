#include "Line.h"

namespace TomatoLib {
	Line::Line(const Vector2& a, const Vector2& b) : A(a), B(b) {}

	bool Line::intersects(const Line& other) const {
		// https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
		auto const& p = this->A;
		auto const& q = other.A;
		auto const r = this->B - p;
		auto const s = other.B - q;

		auto const t = (q - p).Cross(s) / (r.Cross(s));
		auto const u = (q - p).Cross(r) / (r.Cross(s));

		return (t >= 0 && t <= 1 && u >= 0 && u <= 1);
	}

	bool Line::intersects(const Line& other, Vector2& point) const {
		// https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
		auto const& p = this->A;
		auto const& q = other.A;
		auto const r = this->B - p;
		auto const s = other.B - q;

		auto const t = (q - p).Cross(s) / (r.Cross(s));
		auto const u = (q - p).Cross(r) / (r.Cross(s));

		point = { p + r * t };

		return (t >= 0 && t <= 1 && u >= 0 && u <= 1);
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