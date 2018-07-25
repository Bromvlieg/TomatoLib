#include "Vector3.h"
#include "Vector2.h"
#include "Matrix.h"
#include <math.h>
#include <float.h>

namespace TomatoLib {
	const Vector3 Vector3::Zero = Vector3();
	const Vector3 Vector3::One = Vector3(1, 1, 1);
	const Vector3 Vector3::Min = Vector3(FLT_MIN, FLT_MIN, FLT_MIN);
	const Vector3 Vector3::Max = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);

	Vector3::Vector3() {
		X = 0.0f;
		Y = 0.0f;
		Z = 0.0f;
	}

	Vector3::Vector3(float values)
		: X(values), Y(values), Z(values) {

	}

	Vector3::Vector3(float x, float y, float z)
		: X(x), Y(y), Z(z) {

	}

	//TODO: fix to use vector2 or update function to work in 3d space
	float Vector3::DistanceToLine(const Vector3& point_a, const Vector3& point_b) {
		TomatoLib::Vector3 ap = *this - point_a;
		TomatoLib::Vector3 ab = point_b - point_a;
		float ab2 = ab.X * ab.X + ab.Y * ab.Y;
		float ap_ab = ap.X * ab.X + ap.Y * ab.Y;

		float t = ap_ab / ab2;
		if (t < 0.0f) t = 0.0f;
		else if (t > 1.0f) t = 1.0f;

		TomatoLib::Vector3 closest_point = point_a + ab * t;
		return closest_point.Distance(*this);
	}


	float Vector3::Distance(const Vector3& other) const {
		return sqrtf(((this->X - other.X) * (this->X - other.X)) + ((this->Y - other.Y) * (this->Y - other.Y)) + ((this->Z - other.Z) * (this->Z - other.Z)));
	}

	float Vector3::Length() const {
		return sqrtf(powf(X, 2) + powf(Y, 2) + powf(Z, 2));
	}

	Vector3 Vector3::Normalized() const {
		return (*this) / Length();
	}

	Vector2 Vector3::XY() const {
		return Vector2(this->X, this->Y);
	}

	Vector3 Vector3::XZY() const {
		return Vector3(this->X, this->Z, this->Y);
	}

	Vector3 Vector3::GetRotated(const Vector3 &ang, const Vector3 &orgin) const {
		Matrix rotmat = Matrix::CreateRotationZ(ang.X) * Matrix::CreateRotationY(ang.Z) * Matrix::CreateRotationX(ang.Y) * Matrix::CreateTranslation(orgin);
		return rotmat.Translate(*this);
	}

	Vector3 Vector3::Cross(const Vector3& other) const {
		Vector3 retVal;

		retVal.X = Y * other.Z - Z * other.Y;
		retVal.Y = Z * other.X - X * other.Z;
		retVal.Z = X * other.Y - Y * other.X;

		return retVal;
	}

	float Vector3::Dot(const Vector3& other) const {
		return X * other.X + Y * other.Y + Z * other.Z;
	}

	Vector3 Vector3::operator- (const Vector3& other) const {
		return Vector3(X - other.X, Y - other.Y, Z - other.Z);
	}

	Vector3 Vector3::operator+ (const Vector3& other) const {
		return Vector3(X + other.X, Y + other.Y, Z + other.Z);
	}

	Vector3 Vector3::operator- (const float& other) const {
		return Vector3(X - other, Y - other, Z - other);
	}

	Vector3 Vector3::operator+ (const float& other) const {
		return Vector3(X + other, Y + other, Z + other);
	}

	Vector3 Vector3::operator* (const float& scale) const {
		return Vector3(X * scale, Y * scale, Z * scale);
	}

	Vector3 Vector3::operator* (const Vector3& scale) const {
		return Vector3(X * scale.X, Y * scale.Y, Z * scale.Z);
	}

	Vector3 Vector3::operator/ (const Vector3& other) const {
		return Vector3(X / other.X, Y / other.Y, Z / other.Z);
	}

	Vector3 Vector3::operator/ (const float& scale) const {
		return Vector3(X / scale, Y / scale, Z / scale);
	}

	Vector3& Vector3::operator-= (const Vector3& other) {
		X = X - other.X;
		Y = Y - other.Y;
		Z = Z - other.Z;
		return *this;
	}

	Vector3& Vector3::operator+= (const Vector3& other) {
		X = X + other.X;
		Y = Y + other.Y;
		Z = Z + other.Z;
		return *this;
	}

	Vector3& Vector3::operator*= (const Vector3& other) {
		X = X * other.X;
		Y = Y * other.Y;
		Z = Z * other.Z;
		return *this;
	}

	Vector3& Vector3::operator*= (const float& other) {
		X = X * other;
		Y = Y * other;
		Z = Z * other;
		return *this;
	}

	Vector3& Vector3::operator/= (const Vector3& other) {
		X = X / other.X;
		Y = Y / other.Y;
		Z = Z / other.Z;
		return *this;
	}

	Vector3& Vector3::operator/= (const float& other) {
		X = X / other;
		Y = Y / other;
		Z = Z / other;
		return *this;
	}

	Vector3 Vector3::operator-() const {
		return Vector3(-X, -Y, -Z);
	}

	bool Vector3::operator== (const Vector3& other) const {
		return X == other.X && Y == other.Y && Z == other.Z;
	}

	bool Vector3::operator!= (const Vector3& other) const {
		return !operator==(other);
	}

	std::ostream& operator<<(std::ostream& stream, const Vector3& rhs) {
		stream << "{" << rhs.X << ", " << rhs.Y << ", " << rhs.Z << "}";

		return stream;
	}

	const Vector3 operator*(const float& lhs, const Vector3& rhs) {
		return Vector3(lhs * rhs.X, lhs * rhs.Y, lhs * rhs.Z);
	}

	const Vector3 operator-(const float& lhs, const Vector3& rhs) {
		return Vector3(lhs - rhs.X, lhs - rhs.Y, lhs - rhs.Z);
	}

	const Vector3 operator+(const float& lhs, const Vector3& rhs) {
		return Vector3(lhs + rhs.X, lhs + rhs.Y, lhs + rhs.Z);
	}

	Vector3 Vector3::FromAngles(float pitch, float yaw) {
		return Vector3(
			sinf(pitch) * cosf(yaw),
			sinf(pitch) * sinf(yaw),
			cosf(pitch)
		);
	}
}
