#include "Vector3.h"
#include "Matrix.h"
#include <math.h>

namespace TomatoLib {
	const Vector3 Vector3::Zero = Vector3();
	const Vector3 Vector3::One = Vector3(1, 1, 1);

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

	float Vector3::Distance(const Vector3& other) const {
		return sqrt(((this->X - other.X) * (this->X - other.X)) + ((this->Y - other.Y) * (this->Y - other.Y)) + ((this->Z - other.Z) * (this->Z - other.Z)));
	}

	float Vector3::Length() const {
		return sqrt(pow(X, 2) + pow(Y, 2) + pow(Z, 2));
	}

	Vector3 Vector3::Normalized() const {
		return (*this) / Length();
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

	Vector3 Vector3::operator* (const float& scale) const {
		return Vector3(X * scale, Y * scale, Z * scale);
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

	const Vector3 operator*(const Vector3& lhs, const Vector3& rhs) {
		return Vector3(lhs.X * rhs.X, lhs.Y * rhs.Y, lhs.Z * rhs.Z);
	}

	Vector3 Vector3::FromAngles(float pitch, float yaw) {
		return Vector3(
			(float)(sin(pitch) * cos(yaw)),
			(float)(sin(pitch) * sin(yaw)),
			(float)(cos(pitch))
			);
	}
}
