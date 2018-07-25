#pragma once
#ifndef __VECTOR3_H__
#define __VECTOR3_H__

#include <iosfwd>

namespace TomatoLib {
	class Vector2;

	class Vector3 {
	public:
		float X, Y, Z;
		Vector3();
		Vector3(float values);
		Vector3(float x, float y, float z);

#ifdef BULLET_COLLISION_COMMON_H
		Vector3(const btVector3& o) { this->X = o.x(); this->Y = o.y(); this->Z = o.z(); }
		btVector3 ToBulletVector(){ return btVector3(this->X, this->Y, this->Z); }
#endif

		static Vector3 FromAngles(float pitch, float yaw);

		float Length() const;
		Vector3 Normalized() const;
		Vector3 GetRotated(const Vector3 &ang, const Vector3 &orgin) const;

		float DistanceToLine(const Vector3& point_a, const Vector3& point_b);

		float Distance(const Vector3& other) const;
		Vector3 Cross(const Vector3& other) const;
		float Dot(const Vector3& other) const;

		Vector3 operator- (const Vector3& other) const;
		Vector3 operator- (const float& other) const;
		Vector3 operator+ (const Vector3& other) const;
		Vector3 operator+ (const float& other) const;
		Vector3 operator* (const Vector3& other) const;
		Vector3 operator* (const float& other) const;
		Vector3 operator/ (const Vector3& other) const;
		Vector3 operator/ (const float& other) const;

		Vector3 operator- () const;

		Vector3& operator-= (const Vector3& other);
		Vector3& operator+= (const Vector3& other);
		Vector3& operator*= (const Vector3& other);
		Vector3& operator*= (const float& other);
		Vector3& operator/= (const Vector3& other);
		Vector3& operator/= (const float& other);

		Vector2 XY() const;
		Vector3 XZY() const;

		bool operator== (const Vector3& other) const;
		bool operator!= (const Vector3& other) const;

		const static Vector3 Zero;
		const static Vector3 One;
		const static Vector3 Min;
		const static Vector3 Max;
	};

	std::ostream& operator<<(std::ostream& stream, const Vector3& rhs);

	const Vector3 operator*(const float& lhs, const Vector3& rhs);
	const Vector3 operator+(const float &lhs, const Vector3& rhs);
	const Vector3 operator-(const float &lhs, const Vector3& rhs);
}
#endif
