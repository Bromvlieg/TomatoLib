#pragma once
#ifndef __WINDOW_H__
#define __WINDOW_H__

// #define TL_ENABLE_GLFW

#include "../Math/Vector2.h"
#include <string>
#include "../Utilities/Dictonary.h"

#include "../Defines.h"
#include "../Config.h"

#ifdef TL_ENABLE_GLFW
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#endif

#include <functional>

namespace TomatoLib {
	class UIManager;

	class Window {
		void SetCallbacks();

#ifdef TL_ENABLE_GLFW
		static void OnChar(GLFWwindow* window, unsigned int ch);
		static void OnKey(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void OnMouse(GLFWwindow* window, int button, int action, int mods);
		static void OnScroll(GLFWwindow* window, double x, double y);
		static void OnMousePos(GLFWwindow* window, double x, double y);
		static void OnFocus(GLFWwindow* window, int focus);
		static void OnResize(GLFWwindow* window, int w, int h);
#endif

	public:
		static Window* CurrentWindow;

		unsigned char KeysIn[GLFW_KEY_LAST];
		unsigned char MouseIn[GLFW_MOUSE_BUTTON_LAST];

		Dictonary<int, int> Hints;

#ifdef TL_ENABLE_GLFW
		GLFWwindow* Handle;
#else
		void* Handle;
#endif

		UIManager* UIMan;
		bool HasFocus;

		std::function<void(Window& window, int width, int height)> OnResizeCallback;

		virtual void SetTitle(std::string title);
		virtual Vector2 GetSize() const;
		virtual Vector2 GetMouse() const;
		virtual void SetMouse(int x, int y);
		virtual void SetMouse(const Vector2& pos);

		virtual bool Create(unsigned int w, unsigned int h, bool fullscreen = false, bool resizable = false);
		virtual void SwapBuffer();
		virtual void PollEvents();
		virtual void Close();
		virtual bool IsClosing() const;

		virtual void UpdateKeyHolding();

		virtual void SetSize(int w, int h);
		virtual void SetPos(int x, int y);
		virtual Vector2 GetPos();

		Window();
		~Window();
	};
}

#endif