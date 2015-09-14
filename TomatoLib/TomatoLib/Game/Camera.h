#pragma once
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <GL/glew.h>
#include "../Math/Vector2.h"
#include "../Math/Matrix.h"
#include "Window.h"

namespace TomatoLib {
	class Camera {
		float x, y, z;
		float lookx, looky, lookz;
		float pitch, yaw;

		float nearClippingPlane;
		float farClippingPlane;

		void UpdateLookat();

		Matrix mat;
		Matrix projmat;

	public:
		float GetX() const;
		float GetY() const;
		float GetZ() const;

		void SetX(float x);
		void SetY(float y);
		void SetZ(float z);

		void SetPos(float x, float y, float z);

		void AddX(float x);
		void AddY(float y);
		void AddZ(float z);

		void AddPos(float x, float y, float z);

		float GetPitch();
		float GetYaw();

		void SetPitch(float pitch);
		void SetYaw(float yaw);

		void InsertViewMatrix(GLint location) const;
		void HandleRawInput(Window& w);
		void SetView(const Matrix& m);
		void SetProjection(const Matrix& m);
		Matrix& GetView();
		Matrix& GetProjection();

		Vector3 WorldToScreen(Vector3& pos, Vector2& winsize);

		Camera();
	};
}

#endif
