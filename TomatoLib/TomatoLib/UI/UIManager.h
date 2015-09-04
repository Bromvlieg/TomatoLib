#pragma once
#ifndef __UIMAN_H__
#define __UIMAN_H__

#include "../Graphics/Render.h"
#include "../Graphics/Texture.h"
#include "../Graphics/Shader.h"
#include "../Math/Vector2.h"

#include <GL/glew.h>

struct GLFWcursor;

namespace TomatoLib {
	class UIConsole;
	class UIGraph;
	class UIBase;

	class UIManager {
	public:
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
		bool HandleMouseInteraction(int x, int y, int button, bool pressed, int mods);
		bool HandleKeyboardInteraction(int key, unsigned char pressed, int mods);
		bool HandleScrollInteraction(int x, int y);
		bool HandleCharInteraction(int ch);

		int CursorShape;
		GLFWcursor* CurrentCursor;

		UIBase* HoldPanel;
		UIBase* FocusPanel;
		UIConsole* Console;
		bool ShowCursor;
		bool OverrideShowCursor;
	};
}

#endif