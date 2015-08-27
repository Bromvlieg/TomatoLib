#pragma once
#ifndef __VECTOR3_H__
#define __VECTOR3_H__
namespace TomatoLib {
	class Vector3 {
	public:
		float X, Y, Z;
		Vector3();
		Vector3(float values);
		Vector3(float x, float y, float z);
		static Vector3 FromAngles(float pitch, float yaw);

		float Length() const;
		Vector3 Normalized() const;
		Vector3 GetRotated(const Vector3 &ang, const Vector3 &orgin) const;

		float Distance(const Vector3& other) const;
		Vector3 Cross(const Vector3& other) const;
		float Dot(const Vector3& other) const;

		Vector3 operator- (const Vector3& other) const;
		Vector3 operator+ (const Vector3& other) const;
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

		Vector3 XZY() const;

		bool operator== (const Vector3& other) const;
		bool operator!= (const Vector3& other) const;

		const static Vector3 Zero;
		const static Vector3 One;
	};
	const Vector3 operator*(const Vector3& lhs, const Vector3& rhs);
}
#endif
