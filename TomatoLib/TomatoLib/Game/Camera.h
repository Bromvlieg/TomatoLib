#pragma once
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <GL/glew.h>
#include "../Math/Vector2.h"
#include "Window.h"

namespace TomatoLib {
	class Camera {
		float x, y, z;
		float lookx, looky, lookz;
		float pitch, yaw;

		float nearClippingPlane;
		float farClippingPlane;

		void UpdateLookat();

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

		void ApplyProjMatrix();

		void InsertViewMatrix(GLint location);
		void HandleRawInput(Window& w);

		Camera();
	};
}

#endif
