#pragma once
#ifndef __UIMAN_H__
#define __UIMAN_H__

#include <functional>
#include "../Graphics/Render.h"
#include "../Graphics/Texture.h"
#include "../Graphics/Shader.h"
#include "../Math/Vector2.h"

#include "../Config.h"

#ifndef TL_ENABLE_EGL
#include <GL/glew.h>
#else
#include "GLES/gl.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"
#endif

struct GLFWcursor;

namespace TomatoLib {
	class UIConsole;
	class UIGraph;
	class UIBase;

	class UIManager {
	public:
		// return true to prevent UI interactions
		std::function<bool(int x, int y, int button, bool pressed, int mods)> OnBeforeMouseInteracton = [](int x, int y, int button, bool pressed, int mods) { return false; };
		std::function<bool(int key, bool, int mods)> OnBeforeKeyboardInteracton = [](int key, bool, int mods) { return false; };
		std::function<bool(int x, int y)> OnBeforeScrollInteracton = [](int x, int y) { return false; };
		std::function<bool(int ch)> OnBeforeCharInteraction = [](int ch) { return false; };
		
		// called when UI didn't handle interactions, return true to inform window that it even to the Ui didnt handle it, it was infact handled
		std::function<bool(int x, int y, int button, bool pressed, int mods)> OnAfterMouseInteracton = [](int x, int y, int button, bool pressed, int mods) { return false; };
		std::function<bool(int key, bool, int mods)> OnAfterKeyboardInteracton = [](int key, bool, int mods) { return false; };
		std::function<bool(int x, int y)> OnAfterScrollInteracton = [](int x, int y) { return false; };
		std::function<bool(int ch)> OnAfterCharInteraction = [](int ch) { return false; };

		Render& Drawer;
		std::vector<UIBase*> Children;

		Vector2 LastMousePosition;

		UIManager(Render& Drawer);
		~UIManager();

		void Draw(Render& r);
		void Update();
		void _CheckClick(UIBase* panel, int x, int y);

		UIBase* AddChild(UIBase* obj);
		void RemoveChild(UIBase* obj);
		void KillChildren();

		bool HandleMouseInteraction(int x, int y, int button, bool pressed, int mods);
		bool HandleKeyboardInteraction(int key, bool, int mods);
		bool HandleScrollInteraction(int x, int y);
		bool HandleCharInteraction(int ch);

		int CurrentCursorShape;
		GLFWcursor* CurrentCursor;

		UIBase* HoldPanel;
		UIBase* FocusPanel;
		UIConsole* Console;
		bool ShowCursor;
		bool OverrideShowCursor;
	};
}

#endif