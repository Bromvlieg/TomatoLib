#pragma once
#ifndef __TL__GAME_H_
#define __TL__GAME_H_

#include "../Graphics/Render.h"

namespace TomatoLib {
	class Game {
	public:
		bool ShouldShutdown;
		Render RenderObject;

		int currentFPS;
		float currentFPSTime;
		float m_fTargetFPS;

		virtual void Init();
		virtual void RegisterConsoleStuff();
		virtual void Quit();

		// called as much as possible, free from FPS lock
		virtual void Tick();

		// called acordingly with the FPS lock
		virtual void Update();

		// called acordingly with the FPS lock
		virtual void Draw(Render& r);

		void EnterGameLoop();
		void Run();

		Game();
		~Game();
	};
}

#endif
