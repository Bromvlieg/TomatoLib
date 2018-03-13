#pragma once
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "../Math/Vector2.h"
#include "../Math/Matrix.h"
#include "Window.h"

#include "../Config.h"
#ifndef TL_ENABLE_EGL
#include <GL/glew.h>
#else
#include "GLES/gl.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"
#endif

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
		float movespeed = 0.5f;

	public:
		float GetX() const;
		float GetY() const;
		float GetZ() const;

		void SetX(float x);
		void SetY(float y);
		void SetZ(float z);

		void SetPos(float x, float y, float z);

		void SetSpeed(float speed);

		void AddX(float x);
		void AddY(float y);
		void AddZ(float z);

		void AddPos(float x, float y, float z);
		Vector3 GetPos();
		Vector3 GetForward();

		float GetPitch();
		float GetYaw();

		void SetPitch(float pitch);
		void SetYaw(float yaw);

		void InsertViewMatrix(GLint location) const;
		void HandleRawInput(const Window& w);
		void SetView(const Matrix& m);
		void SetProjection(const Matrix& m);
		Matrix& GetView();
		Matrix& GetProjection();

		Vector3 WorldToScreen(const Vector3& pos, const Vector2& winsize);

		Camera();
	};
}

#endif
