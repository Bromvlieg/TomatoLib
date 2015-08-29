#pragma once
#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "../Math/Vector2.h"
#include <string>
#include <GLFW/glfw3.h>

namespace TomatoLib {
	class UIManager;

	class Window {
		void SetCallbacks();

		static void OnChar(GLFWwindow* window, unsigned int ch);
		static void OnKey(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void OnMouse(GLFWwindow* window, int button, int action, int mods);
		static void OnScroll(GLFWwindow* window, double x, double y);
		static void OnMousePos(GLFWwindow* window, double x, double y);
		static void OnFocus(GLFWwindow* window, int focus);

	public:
		static Window* CurrentWindow;

		unsigned char KeysIn[GLFW_KEY_LAST];
		unsigned char MouseIn[GLFW_MOUSE_BUTTON_LAST];

		GLFWwindow* Handle;
		UIManager* UIMan;
		bool HasFocus;

		void SetTitle(std::string title);
		Vector2 GetSize();
		Vector2 GetMouse();
		void SetMouse(int x, int y);
		void SetMouse(const Vector2& pos);

		bool Create(int w, int h, bool fullscreen = false, bool resizable = false);
		void SwapBuffer();
		void PollEvents();
		void Close();
		bool IsClosing();

		void UpdateKeyHolding();

		void SetSize(int w, int h);
		void SetPos(int x, int y);
		Vector2 GetPos();

		Window();
		~Window();
	};
}

#endif
