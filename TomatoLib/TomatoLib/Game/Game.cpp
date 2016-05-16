#include "Game.h"

#include "Window.h"
#include "../Async/Async.h"
#include "../Graphics/Render.h"

#include "../Defines.h"

#include <string>
#include <thread>
#include <vector>

#include <stdarg.h>
#include <chrono>
#include <thread>

#ifndef _MSC_VER
#include <pthread.h>
#endif

namespace TomatoLib {
	Game::Game() : ShouldShutdown(false),currentFPS(0),currentFPSTime(0) {}
	Game::~Game() {}

	void Game::Init() {}

	void Game::Quit() {
		this->ShouldShutdown = true;
	}

	void Game::Draw(Render& r) { }
	void Game::Update() { }

	void Game::EnterGameLoop() {
		// make buffer for smoothing fps
		std::vector<int> listfps;
		std::vector<float> listtime;
		for (int i = 0; i < 3; i++) listfps.push_back(0);
		for (int i = 0; i < 120; i++) listtime.push_back(0);

		float lasttick = TL_GET_TIME_MS;
		float lastfpssecond = lasttick; // ms
		while (!this->ShouldShutdown) {
			float curtime = TL_GET_TIME_MS;
			float msperfps = 1000.0f / 60.0f;

			if (curtime - lasttick < msperfps) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				continue;
			}

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			float framestarttime = TL_GET_TIME_MS;
			lasttick += msperfps;
			if (curtime - lasttick > 1000) {
				// we try to make up for ticks that take longer so that we still get the target fps
				// but it would be bad if we turn it into a while(true) at some point, gotta give it a break
				lasttick = curtime;
			}

			Async::RunMainThreadCalls();
			if (this->ShouldShutdown) break;

			this->Update();
			if (this->ShouldShutdown) break;

			//checkGL;
			this->Draw(this->RenderObject);
			//checkGL;

			listfps[0]++;
			if (curtime - lastfpssecond >= 1000) {
				currentFPS = 0;
				for (unsigned int i = 0; i < listfps.size(); i++) {
					currentFPS += listfps[i];
				}

				currentFPS /= listfps.size();
				lastfpssecond = curtime;

				listfps.insert(listfps.begin(), 0);
				listfps.pop_back();
			}

			listtime.insert(listtime.begin(), TL_GET_TIME_MS - framestarttime);
			listtime.pop_back();
			currentFPSTime = 0;

			for (unsigned int i = 0; i < listtime.size(); i++) {
				currentFPSTime += listtime[i];
			}

			currentFPSTime /= listtime.size();
		}
	}

	void Game::Run() {
		this->Init();
		this->RegisterConsoleStuff();
		this->EnterGameLoop();
	}

	void Game::RegisterConsoleStuff() {

	}
}
