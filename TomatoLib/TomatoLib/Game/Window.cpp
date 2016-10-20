#include "Window.h"

#include "../UI/UIManager.h"
#include "../Defines.h"

#include <mutex>
#include <cstring>


#ifdef TL_ENABLE_GLFW
#include <GLFW/glfw3.h>
#endif

namespace TomatoLib {
	Window* Window::CurrentWindow = nullptr;

#ifdef TL_ENABLE_GLFW
	void Window::OnKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (key < 0 || key > GLFW_KEY_LAST) return; // filter out hardware that wants to be special and has special keys.
		if (CurrentWindow == nullptr) return;

		bool ret = CurrentWindow->UIMan != nullptr && CurrentWindow->UIMan->HandleKeyboardInteraction(key, action, mods);
		if (CurrentWindow == nullptr) return;

		if (action == GLFW_REPEAT) return;
		CurrentWindow->KeysIn[key] = 0;

		if (ret) return;
		CurrentWindow->KeysIn[key] = action != GLFW_RELEASE ? 1 : 0;
	}

	void Window::OnMouse(GLFWwindow* window, int button, int action, int mods) {
		if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) return; // filter out hardware that wants to be special and has special keys.
		if (CurrentWindow == nullptr) return;

		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		bool ret = CurrentWindow->UIMan != nullptr && CurrentWindow->UIMan->HandleMouseInteraction((int)mouseX, (int)mouseY, button, action == GLFW_PRESS, mods);
		if (CurrentWindow == nullptr) return;

		CurrentWindow->MouseIn[button] = 0;
		if (ret || button > 2) return;
		CurrentWindow->MouseIn[button] = action == GLFW_PRESS ? 1 : 0;
	}

	void Window::OnChar(GLFWwindow* window, unsigned int ch) {
		if (CurrentWindow == nullptr) return;
		if (CurrentWindow->UIMan == nullptr) return;
		CurrentWindow->UIMan->HandleCharInteraction(ch);
	}

	void Window::OnScroll(GLFWwindow* window, double sx, double sy) {
		if (CurrentWindow == nullptr) return;
		int w, h;
		double x, y;

		glfwGetCursorPos(window, &x, &y);
		glfwGetWindowSize(window, &w, &h);

		if (x < 0 || y < 0 || x > w || y > h) return;

		if (CurrentWindow->UIMan == nullptr) return;
		CurrentWindow->UIMan->HandleScrollInteraction((int)sx * 10, (int)sy * 10);
	}

	void Window::OnMousePos(GLFWwindow* window, double x, double y) {
		if (CurrentWindow == nullptr) return;
		if (CurrentWindow->UIMan == nullptr) return;
		CurrentWindow->UIMan->LastMousePosition = Vector2(x, y);
	}

	void Window::OnFocus(GLFWwindow* window, int focus) {
		if (CurrentWindow == nullptr) return;
		CurrentWindow->HasFocus = focus == 1;
	}

	void Window::OnResize(GLFWwindow* window, int w, int h) {
		if (CurrentWindow == nullptr) return;
		if (CurrentWindow->OnResizeCallback != nullptr) CurrentWindow->OnResizeCallback(*CurrentWindow, w, h);
	}

	void error_callback(int error, const char* description) {
		printf("GLFW error %d: '%s'\n", error, description);
	}
#endif

	void Window::PollEvents() {
		static std::mutex EventsMutex;

		EventsMutex.lock();
		CurrentWindow = this;

#ifdef TL_ENABLE_GLFW
		glfwPollEvents();
#endif

		EventsMutex.unlock();
	}

	bool Window::IsClosing() const {
#ifdef TL_ENABLE_GLFW
		return glfwWindowShouldClose(this->Handle) == 1;
#else
		return false;
#endif
	}

	void Window::SetTitle(std::string title) {
#ifdef TL_ENABLE_GLFW
		glfwSetWindowTitle(this->Handle, title.c_str());
#endif
	}

	Vector2 Window::GetSize() const {
		int x = 0, y = 0;

#ifdef TL_ENABLE_GLFW
		glfwGetWindowSize(this->Handle, &x, &y);
#endif
		return Vector2(x, y);
	}

	Vector2 Window::GetMouse() const {
		double mouseX = 0, mouseY = 0;
#ifdef TL_ENABLE_GLFW
		glfwGetCursorPos(this->Handle, &mouseX, &mouseY);
#endif

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
#ifdef TL_ENABLE_GLFW
		glfwSwapBuffers(this->Handle);
#endif
	}

	Window::Window() {
		this->Handle = nullptr;
		this->UIMan = nullptr;
		this->HasFocus = false;

		memset(this->KeysIn, 0, GLFW_KEY_LAST);
		memset(this->MouseIn, 0, GLFW_MOUSE_BUTTON_LAST);

		if (this->CurrentWindow == nullptr) {
			this->CurrentWindow = this;
		}

#ifdef TL_ENABLE_GLFW
		this->Hints[GLFW_CONTEXT_VERSION_MAJOR] = 3;
		this->Hints[GLFW_CONTEXT_VERSION_MINOR] = 2;
		this->Hints[GLFW_OPENGL_PROFILE] = GLFW_OPENGL_CORE_PROFILE;
		this->Hints[GLFW_OPENGL_FORWARD_COMPAT] = GL_TRUE; // for Mac OS X
#endif
	}

	Window::~Window() {
		checkGL;

#ifdef TL_ENABLE_GLFW
		glfwDestroyWindow(this->Handle);
		glGetError();

		glfwTerminate();
		glGetError();
#endif
	}

	void Window::Close() {

#ifdef TL_ENABLE_GLFW
		glfwSetWindowShouldClose(this->Handle, GL_TRUE);
#endif

		if (CurrentWindow == this) {
			CurrentWindow = nullptr;
		}
	}

	void Window::SetSize(int w, int h) {

#ifdef TL_ENABLE_GLFW
		glfwSetWindowSize(this->Handle, w, h);
#endif
	}


	void Window::SetPos(int x, int y) {
#ifdef TL_ENABLE_GLFW
		glfwSetWindowPos(this->Handle, x, y);
#endif
	}

	Vector2 Window::GetPos() {
		int x = 0;
		int y = 0;

#ifdef TL_ENABLE_GLFW
		glfwGetWindowPos(this->Handle, &x, &y);
#endif

		return Vector2(x, y);
	}

	void Window::SetMouse(const Vector2& pos) { this->SetMouse((int)pos.X, (int)pos.Y); }
	void Window::SetMouse(int x, int y) {
#ifdef TL_ENABLE_GLFW
		glfwSetCursorPos(this->Handle, x, y);
#endif
	}

	void Window::SetCallbacks() {
#ifdef TL_ENABLE_GLFW
		glfwSetKeyCallback(this->Handle, this->OnKey);
		glfwSetMouseButtonCallback(this->Handle, this->OnMouse);
		glfwSetScrollCallback(this->Handle, this->OnScroll);
		glfwSetCharCallback(this->Handle, this->OnChar);
		glfwSetCursorPosCallback(this->Handle, this->OnMousePos);
		glfwSetWindowFocusCallback(this->Handle, this->OnFocus);
		glfwSetWindowSizeCallback(this->Handle, this->OnResize);
#endif
	}

	bool Window::Create(unsigned int w, unsigned int h, bool fullscreen, bool resizable) {
#ifdef TL_ENABLE_GLFW
		if (glfwInit() != GL_TRUE) {
			printf("glfwInit failed!\n");
			return false;
		}

		glfwSetErrorCallback(error_callback);

		GLFWmonitor* mon = glfwGetPrimaryMonitor();
		if (mon == nullptr) {
			printf("glfwGetPrimaryMonitor failed!\n");
			return false;
		}

		const GLFWvidmode* mode = glfwGetVideoMode(mon);

		if (fullscreen) {
			w = mode->width;
			h = mode->height;
		} else if (w == mode->width && h == mode->height) {
			fullscreen = true;
		}

		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

		this->Hints[GLFW_RESIZABLE] = resizable ? GL_TRUE : GL_FALSE;
		for (int i = 0; i < this->Hints.Count; i++) {
			glfwWindowHint(this->Hints.Keys[i], this->Hints.Values[i]);
		}

		this->Handle = glfwCreateWindow(w, h, "", fullscreen ? mon : nullptr, nullptr);
		if (this->Handle == nullptr) {
			printf("glfwCreateWindow failed!\n");
			return false;
		}

		this->SetCallbacks();

		if (!fullscreen) glfwSetWindowPos(this->Handle, mode->width / 2 - w / 2, mode->height / 2 - h / 2);

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
#else
		return false;
#endif
	}
}
