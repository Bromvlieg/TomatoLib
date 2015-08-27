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
		glUniformMatrix4fv(location, 1, GL_FALSE, this->mat.values);
	}

	void Camera::SetMatrix(const Matrix& m) {
		this->mat = m;
	}

	void Camera::HandleRawInput(Window& w) {
		float speed = w.KeysIn[GLFW_KEY_LEFT_SHIFT] ? 60.0f : 25.0f;

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

		double mouseX, mouseY;
		glfwGetCursorPos(w.Handle, &mouseX, &mouseY);
		double mouseDeltaX = mouseX - wsize.X / 2;
		double mouseDeltaY = mouseY - wsize.Y / 2;

		float newYaw = float(mouseDeltaX) * 0.005f + this->GetYaw();
		this->SetYaw(newYaw);
		float newPitch = float(mouseDeltaY) * 0.005f + this->GetPitch();
		if (newPitch > 0.01f && newPitch < (M_PI - 0.01f)) {
			this->SetPitch(newPitch);
		}

		glfwSetCursorPos(w.Handle, wsize.X / 2, wsize.Y / 2);

		this->AddPos(movement.X, movement.Y, movement.Z);
	}
}
