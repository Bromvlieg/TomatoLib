#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Window.h"

#include "../UI/UIManager.h"
#include "../Defines.h"

#include <mutex>
#include <cstring>

namespace TomatoLib {
	Window* Window::CurrentWindow = null;
	std::mutex EventsMutex;

	void Window::OnKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (key < 0 || key > GLFW_KEY_LAST) return; // filter out hardware that wants to be special and has special keys.

		bool ret = CurrentWindow->UIMan != null && CurrentWindow->UIMan->HandleKeyboardInteraction(key, action, mods);

		if (action == GLFW_REPEAT) return;
		CurrentWindow->KeysIn[key] = 0;

		if (ret) return;
		CurrentWindow->KeysIn[key] = action != GLFW_RELEASE ? 1 : 0;
	}

	void Window::OnMouse(GLFWwindow* window, int button, int action, int mods) {
		if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) return; // filter out hardware that wants to be special and has special keys.

		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		bool ret = CurrentWindow->UIMan != null && CurrentWindow->UIMan->HandleMouseInteraction((int)mouseX, (int)mouseY, button, action == GLFW_PRESS, mods);

		CurrentWindow->MouseIn[button] = 0;
		if (ret || button > 2) return;
		CurrentWindow->MouseIn[button] = action == GLFW_PRESS ? 1 : 0;
	}

	void Window::OnChar(GLFWwindow* window, unsigned int ch) {
		if (CurrentWindow->UIMan == null) return;
		CurrentWindow->UIMan->HandleCharInteraction(ch);
	}

	void Window::OnScroll(GLFWwindow* window, double sx, double sy) {
		int w, h;
		double x, y;

		glfwGetCursorPos(window, &x, &y);
		glfwGetWindowSize(window, &w, &h);

		if (x < 0 || y < 0 || x > w || y > h) return;

		if (CurrentWindow->UIMan == null) return;
		CurrentWindow->UIMan->HandleScrollInteraction((int)sx * 10, (int)sy * 10);
	}

	void Window::OnMousePos(GLFWwindow* window, double x, double y) {
		if (CurrentWindow->UIMan == null) return;
		CurrentWindow->UIMan->LastMousePosition = Vector2(x, y);
	}

	void Window::OnFocus(GLFWwindow* window, int focus) {
		CurrentWindow->HasFocus = focus == 1;
	}

	void error_callback(int error, const char* description) {
		fputs(description, stderr);
	}

	void Window::PollEvents() {
		EventsMutex.lock();
		CurrentWindow = this;
		glfwPollEvents();
		EventsMutex.unlock();
	}

	bool Window::IsClosing() {
		return glfwWindowShouldClose(this->Handle) == 1;
	}

	void Window::SetTitle(std::string title) {
		glfwSetWindowTitle(this->Handle, title.c_str());
	}

	Vector2 Window::GetSize() {
		int x, y;

		glfwGetWindowSize(this->Handle, &x, &y);
		return Vector2(x, y);
	}

	Vector2 Window::GetMouse() {
		double mouseX, mouseY;
		glfwGetCursorPos(this->Handle, &mouseX, &mouseY);

		return Vector2((float)mouseX, (float)mouseY);
	}

	void Window::UpdateKeyHolding() {
		for (int i = 0; i < GLFW_KEY_LAST; i++) {
			if (this->KeysIn[i] == TL_KEYS_PRESS) {
				this->KeysIn[i] = TL_KEYS_HOLD;
			}
		}

		for (int i = 0; i < GLFW_MOUSE_BUTTON_LAST; i++) {
			if (this->MouseIn[i] == TL_KEYS_PRESS) {
				this->MouseIn[i] = TL_KEYS_HOLD;
			}
		}
	}

	void Window::SwapBuffer() {
		glfwSwapBuffers(this->Handle);
	}

	Window::Window() {
		this->Handle = null;
		this->UIMan = null;
		this->HasFocus = false;

		memset(this->KeysIn, 0, GLFW_KEY_LAST);
		memset(this->MouseIn, 0, GLFW_MOUSE_BUTTON_LAST);

		if (this->CurrentWindow == null) {
			this->CurrentWindow = this;
		}
	}

	Window::~Window() {
		checkGL;

		glfwDestroyWindow(this->Handle);
		glGetError();

		glfwTerminate();
		glGetError();
	}

	void Window::Close() {
		glfwSetWindowShouldClose(this->Handle, GL_TRUE);
	}

	void Window::SetSize(int w, int h) {
		glfwSetWindowSize(this->Handle, w, h);
	}


	void Window::SetPos(int x, int y) {
		glfwSetWindowPos(this->Handle, x, y);
	}

	Vector2 Window::GetPos() {
		int x;
		int y;
		glfwGetWindowPos(this->Handle, &x, &y);

		return Vector2(x, y);
	}

	void Window::SetCallbacks() {
		glfwSetKeyCallback(this->Handle, this->OnKey);
		glfwSetMouseButtonCallback(this->Handle, this->OnMouse);
		glfwSetScrollCallback(this->Handle, this->OnScroll);
		glfwSetCharCallback(this->Handle, this->OnChar);
		glfwSetErrorCallback(error_callback);
		glfwSetCursorPosCallback(this->Handle, this->OnMousePos);
		glfwSetWindowFocusCallback(this->Handle, this->OnFocus);
	}

	bool Window::Create(int w, int h, bool fullscreen, bool resizable) {
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, resizable ? GL_TRUE : GL_FALSE);

		GLFWmonitor* mon = glfwGetPrimaryMonitor();
		if (mon == nullptr) {
			printf("glfwGetPrimaryMonitor failed!\n");
			return false;
		}

		const GLFWvidmode* mode = glfwGetVideoMode(mon);

		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

		this->Handle = glfwCreateWindow(w, h, "", fullscreen ? mon : nullptr, nullptr);
		if (this->Handle == null) {
			printf("glfwCreateWindow failed!\n");
			return false;
		}

		this->SetCallbacks();

		glfwSetWindowPos(this->Handle, mode->width / 2 - w / 2, mode->height / 2 - h / 2);

		glfwMakeContextCurrent(this->Handle);
		glViewport(0, 0, w, h);
		glfwSwapInterval(1); // 0 == infinite FPS, 1 == 60, 2 == 30


		glewExperimental = GL_TRUE;
		GLenum err = glewInit();
		if (err != GLEW_OK) {
			printf("glewInit failed!\n");
			printf("Error: %s\n", glewGetErrorString(err));
			return false;
		}

		GLenum currentError = glGetError(); // Glew always throws a silly error, this is to filter that.
		if (!(currentError == GL_NO_ERROR || currentError == GL_INVALID_ENUM)) {
			printf("something's wrong with glew...%x\n", currentError);
			return false;
		}

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		this->HasFocus = true;
		return true;
	}
}
