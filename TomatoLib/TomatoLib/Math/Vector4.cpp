#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"
#include "../Graphics/Color.h"

namespace TomatoLib {
	Vector4::Vector4(const Color& col) {
		this->X = static_cast<float>(col.R) / 255;
		this->Y = static_cast<float>(col.G) / 255;
		this->Z = static_cast<float>(col.B) / 255;
		this->W = static_cast<float>(col.A) / 255;
	}

	Vector4::Vector4(const Vector2& vec) : X(vec.X), Y(vec.Y) {}
	Vector4::Vector4(const Vector3& vec) : X(vec.X), Y(vec.Y), Z(vec.Z) {}

	void Vector4::operator/=(float denom) {
		this->X /= denom;
		this->Y /= denom;
		this->Z /= denom;
		this->W /= denom;
	}

	void Vector4::operator*=(float denom) {
		this->X *= denom;
		this->Y *= denom;
		this->Z *= denom;
		this->W *= denom;
	}

	void Vector4::operator-=(float denom) {
		this->X -= denom;
		this->Y -= denom;
		this->Z -= denom;
		this->W -= denom;
	}

	void Vector4::operator+=(float denom) {
		this->X += denom;
		this->Y += denom;
		this->Z += denom;
		this->W += denom;
	}

	void Vector4::operator/=(const Vector4& other) {
		this->X /= other.X;
		this->Y /= other.Y;
		this->Z /= other.Z;
		this->W /= other.W;
	}

	void Vector4::operator*=(const Vector4& other) {
		this->X *= other.X;
		this->Y *= other.Y;
		this->Z *= other.Z;
		this->W *= other.W;
	}

	void Vector4::operator-=(const Vector4& other) {
		this->X -= other.X;
		this->Y -= other.Y;
		this->Z -= other.Z;
		this->W -= other.W;
	}

	void Vector4::operator+=(const Vector4& other) {
		this->X += other.X;
		this->Y += other.Y;
		this->Z += other.Z;
		this->W += other.W;
	}

	Vector4 Vector4::operator+(const Vector4& other) const {
		return {
			this->X + other.X,
			this->Y + other.Y,
			this->Z + other.Z,
			this->W + other.W
		};
	}

	Vector4 Vector4::operator-(const Vector4& other) const {
		return {
			this->X - other.X,
			this->Y - other.Y,
			this->Z - other.Z,
			this->W - other.W
		};
	}

	Vector4 Vector4::operator/(const Vector4& other) const {
		return {
			this->X / other.X,
			this->Y / other.Y,
			this->Z / other.Z,
			this->W / other.W
		};
	}

	Vector4 Vector4::operator*(const Vector4& other) const {
		return {
			this->X * other.X,
			this->Y * other.Y,
			this->Z * other.Z,
			this->W * other.W
		};
	}

	Vector4 Vector4::operator+(float mult) const {
		return {
			this->X + mult,
			this->Y + mult,
			this->Z + mult,
			this->W + mult
		};
	}

	Vector4 Vector4::operator-(float mult) const {
		return {
			this->X - mult,
			this->Y - mult,
			this->Z - mult,
			this->W - mult
		};
	}

	Vector4 Vector4::operator/(float mult) const {
		return {
			this->X / mult,
			this->Y / mult,
			this->Z / mult,
			this->W / mult
		};
	}

	Vector4 Vector4::operator*(float mult) const {
		return {
			this->X * mult,
			this->Y * mult,
			this->Z * mult,
			this->W * mult
		};
	}

	bool Vector4::operator!=(const Vector4& other) const { return !(*this == other); }
	bool Vector4::operator==(const Vector4& other) const {
		return this->X == other.X && this->Y == other.Y && this->Z == other.Z && this->W == other.W;
	}
}