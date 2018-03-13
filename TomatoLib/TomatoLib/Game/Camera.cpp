#include "Camera.h"
#include "../Math/Matrix.h"
#include "../Defines.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace TomatoLib {
	void Camera::UpdateLookat() {
		lookx = x + sin(pitch) * cos(yaw);
		looky = y + sin(pitch) * sin(yaw);
		lookz = z + cos(pitch);

		this->mat = Matrix::CreateLookAt(Vector3(x, z, y), Vector3(lookx, lookz, looky), Vector3(0.0, 1.0, 0.0));
	}

	Camera::Camera() : x(0), y(4), z(0), pitch((float)M_PI_2), yaw((float)M_PI_2 + (float)M_PI) {
		UpdateLookat();
	}

	float Camera::GetX() const { return x; }
	float Camera::GetY() const { return y; }
	float Camera::GetZ() const { return z; }

	void Camera::SetX(float xx) { x = xx; UpdateLookat(); }
	void Camera::SetY(float yy) { y = yy; UpdateLookat(); }
	void Camera::SetZ(float zz) { z = zz; UpdateLookat(); }

	void Camera::AddX(float xx) { x += xx; UpdateLookat(); }
	void Camera::AddY(float yy) { y += yy; UpdateLookat(); }
	void Camera::AddZ(float zz) { z += zz; UpdateLookat(); }

	void Camera::SetPos(float xx, float yy, float zz) { x = xx; y = yy; z = zz; UpdateLookat(); }
	void Camera::AddPos(float xx, float yy, float zz) { x += xx; y += yy; z += zz; UpdateLookat(); }

	float Camera::GetPitch() { return pitch; }
	float Camera::GetYaw() { return yaw; }

	void Camera::SetPitch(float pp) { pitch = pp; UpdateLookat(); }
	void Camera::SetYaw(float yy) { yaw = yy; UpdateLookat(); }

	void Camera::InsertViewMatrix(GLint location) const {
#ifndef TL_OPENGL_OLD
		glUniformMatrix4fv(location, 1, GL_FALSE, this->mat.values);
#endif
	}

	void Camera::SetView(const Matrix& m) {
		this->mat = m;
	}

	int glhProjectf(float objx, float objy, float objz, float *modelview, float *projection, int *viewport, float *windowCoordinate) {
		//Transformation vectors
		float fTempo[8];
		//Modelview transform
		fTempo[0] = modelview[0] * objx + modelview[4] * objy + modelview[8] * objz + modelview[12];  //w is always 1
		fTempo[1] = modelview[1] * objx + modelview[5] * objy + modelview[9] * objz + modelview[13];
		fTempo[2] = modelview[2] * objx + modelview[6] * objy + modelview[10] * objz + modelview[14];
		fTempo[3] = modelview[3] * objx + modelview[7] * objy + modelview[11] * objz + modelview[15];
		//Projection transform, the final row of projection matrix is always [0 0 -1 0]
		//so we optimize for that.
		fTempo[4] = projection[0] * fTempo[0] + projection[4] * fTempo[1] + projection[8] * fTempo[2] + projection[12] * fTempo[3];
		fTempo[5] = projection[1] * fTempo[0] + projection[5] * fTempo[1] + projection[9] * fTempo[2] + projection[13] * fTempo[3];
		fTempo[6] = projection[2] * fTempo[0] + projection[6] * fTempo[1] + projection[10] * fTempo[2] + projection[14] * fTempo[3];
		fTempo[7] = -fTempo[2];
		//The result normalizes between -1 and 1
		if (fTempo[7] == 0.0f)	//The w value
			return 0;

		fTempo[7] = 1.0f / fTempo[7];
		//Perspective division
		fTempo[4] *= fTempo[7];
		fTempo[5] *= fTempo[7];
		fTempo[6] *= fTempo[7];
		//Window coordinates
		//Map x, y to range 0-1
		windowCoordinate[0] = (fTempo[4] * 0.5f + 0.5f)*viewport[2] + viewport[0];
		windowCoordinate[1] = (fTempo[5] * -0.5f + 0.5f)*viewport[3] + viewport[1];
		//This is only correct when glDepthRange(0.0, 1.0)
		windowCoordinate[2] = (1.0f + fTempo[6])*0.5f;	//Between 0 and 1
		return 1;
	}

	Vector3 Camera::WorldToScreen(const Vector3& pos, const Vector2& winsize) {
		GLint viewport[] = {0, 0, (int)winsize.X, (int)winsize.Y};

		Matrix view = this->mat;
		Matrix proj = this->projmat;

		Vector3 sp;
		glhProjectf(pos.X, pos.Y, pos.Z, view.values, proj.values, viewport, (float*)&sp);

		return sp;
	}



	void Camera::SetSpeed(float speed) {
		this->movespeed = speed;
	}

	void Camera::SetProjection(const Matrix& m) {
		this->projmat = m;
	}

	Vector3 Camera::GetPos() {
		return {x, y, z};
	}

	Vector3 Camera::GetForward() {
		return {
			sin(pitch) * cos(yaw),
			sin(pitch) * sin(yaw),
			cos(pitch)
		};
	}

	Matrix& Camera::GetView() {
		return this->mat;
	}

	Matrix& Camera::GetProjection() {
		return this->projmat;
	}

	void Camera::HandleRawInput(const Window& w) {
		float speed = w.KeysIn[GLFW_KEY_LEFT_SHIFT] ? this->movespeed * 2 : this->movespeed;

		Vector3 movement;
		if (w.KeysIn[GLFW_KEY_W]) {
			movement += Vector3(
				cos(this->GetYaw()) * speed,
				sin(this->GetYaw()) * speed,
				cos(this->GetPitch()) * speed
				);
		}

		if (w.KeysIn[GLFW_KEY_S]) {
			movement += Vector3(
				-cos(this->GetYaw()) * speed,
				-sin(this->GetYaw()) * speed,
				-cos(this->GetPitch()) * speed
				);
		}

		//left/right motion
		if (w.KeysIn[GLFW_KEY_A]) {
			movement += Vector3(
				cos(float(this->GetYaw() + (M_PI / 180) * -90)) * speed,
				sin(float(this->GetYaw() + (M_PI / 180) * -90)) * speed,
				0
				);
		}

		if (w.KeysIn[GLFW_KEY_D]) {
			movement += Vector3(
				cos(float(this->GetYaw() + (M_PI / 180) * 90)) * speed,
				sin(float(this->GetYaw() + (M_PI / 180) * 90)) * speed,
				0
				);
		}

		if (w.KeysIn[GLFW_KEY_SPACE]) {
			movement.Z += speed;
		}

		if (w.KeysIn[GLFW_KEY_C]) {
			movement.Z -= speed;
		}

		Vector2 wsize = w.GetSize();


		// TODO: move cursor move to window
		double mouseX, mouseY;

#ifdef TL_ENABLE_GLFW
		glfwGetCursorPos(w.Handle, &mouseX, &mouseY);
#endif

		double mouseDeltaX = mouseX - wsize.X / 2;
		double mouseDeltaY = mouseY - wsize.Y / 2;

		float newYaw = float(mouseDeltaX) * 0.005f + this->GetYaw();
		this->SetYaw(newYaw);
		float newPitch = float(mouseDeltaY) * 0.005f + this->GetPitch();
		if (newPitch > 0.01f && newPitch < (M_PI - 0.01f)) {
			this->SetPitch(newPitch);
		}

#ifdef TL_ENABLE_GLFW
		glfwSetCursorPos(w.Handle, wsize.X / 2, wsize.Y / 2);
#endif

		this->AddPos(movement.X, movement.Y, movement.Z);
	}
}
