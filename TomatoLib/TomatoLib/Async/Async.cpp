#include "Async.h"

#include <mutex>
#include <chrono>
#include <thread>

#ifdef _MSC_VER
#include <Windows.h>
#endif

namespace TomatoLib {
	namespace Async {
#ifdef _MSC_VER
		unsigned long MainThreadID;
		unsigned long AsyncThreadID;
#else
		pthread_t MainThreadID;
		pthread_t AsyncThreadID;
#endif
		bool Inited = false;
		bool ShouldShutdown = false;

		unsigned int CallsToDoOnAsyncThreadIndex = 0;

		std::vector<std::function<void()>> CallsToDoOnMainThread;
		std::vector<std::function<void()>> CallsToDoOnAsyncThread;

		std::mutex ThreadCallsLock;
		void RunMainThreadCalls() {
			unsigned int i = 0;
			while (true) {
				ThreadCallsLock.lock();
				if (i >= Async::CallsToDoOnMainThread.size()) {
					Async::CallsToDoOnMainThread.clear();
					ThreadCallsLock.unlock();
					break;
				}

				std::function<void()>& func = Async::CallsToDoOnMainThread[i++];
				ThreadCallsLock.unlock();

				func();
			}
		}

		void Init() {
			if (Inited) return;
			Inited = true;

#ifdef _MSC_VER
			Async::MainThreadID = GetCurrentThreadId();
#else
			Async::MainThreadID = pthread_self();
#endif

			new std::thread([]() {
#ifdef _MSC_VER
				Async::AsyncThreadID = GetCurrentThreadId();
#else
				Async::AsyncThreadID = pthread_self();
#endif

				while (!Async::ShouldShutdown) {
					Async::RunAsyncThreadCalls();
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			});
		}

		void Shutdown() {
			Async::ShouldShutdown = true;
		}

		void RunOnMainThread(std::function<void()> func, bool isblocking, bool forcequeue) {
			if (Async::IsMainThread() && !forcequeue) {
				func();
				return;
			}

			if (!isblocking) {
				ThreadCallsLock.lock();
				Async::CallsToDoOnMainThread.push_back(func);
				ThreadCallsLock.unlock();
			} else {
				bool isdone = false;

				ThreadCallsLock.lock();
				Async::CallsToDoOnMainThread.push_back([&isdone, func]() {
					func();
					isdone = true;
				});
				ThreadCallsLock.unlock();

				while (!isdone) {
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			}
		}

		bool IsMainThread() {
#ifdef _MSC_VER
			return Async::MainThreadID == GetCurrentThreadId();
#else
			return Async::MainThreadID == pthread_self();
#endif
		}

		bool IsAsyncThread() {
#ifdef _MSC_VER
			return Async::AsyncThreadID == GetCurrentThreadId();
#else
			return Async::AsyncThreadID == pthread_self();
#endif
		}

		std::mutex ThreadasyncCallsLock;
		bool doingAsyncCall = false;
		void RunAsyncThreadCalls() {
			Async::CallsToDoOnAsyncThreadIndex = 0;
			while (true) {
				ThreadasyncCallsLock.lock();

				if (Async::CallsToDoOnAsyncThreadIndex >= (unsigned int)Async::CallsToDoOnAsyncThread.size()) {
					if (Async::CallsToDoOnAsyncThreadIndex > 0) {
						Async::CallsToDoOnAsyncThread.clear();
						Async::CallsToDoOnAsyncThreadIndex = 0;
					}

					ThreadasyncCallsLock.unlock();
					break;
				}

				std::function<void()> func = Async::CallsToDoOnAsyncThread[Async::CallsToDoOnAsyncThreadIndex++];
				doingAsyncCall = true;
				ThreadasyncCallsLock.unlock();

				func();

				ThreadasyncCallsLock.lock();
				doingAsyncCall = false;
				ThreadasyncCallsLock.unlock();
			}
		}

		void ClearAsyncThreadCalls() {
			ThreadasyncCallsLock.lock();
			while (doingAsyncCall || Async::CallsToDoOnAsyncThread.size() > 0) {
				ThreadasyncCallsLock.unlock();

				std::this_thread::sleep_for(std::chrono::milliseconds(1));

				ThreadasyncCallsLock.lock();
			}
			ThreadasyncCallsLock.unlock();
		}
		
		void RunOnAsyncThread(std::function<void()> func, bool isblocking, bool forcequeue) {
			if (Async::IsAsyncThread() && !forcequeue) {
				func();
				return;
			}

			if (!isblocking) {
				ThreadasyncCallsLock.lock();
				Async::CallsToDoOnAsyncThread.push_back(func);
				ThreadasyncCallsLock.unlock();
			} else {
				bool isdone = false;

				ThreadasyncCallsLock.lock();
				Async::CallsToDoOnAsyncThread.push_back([&isdone, func]() {
					func();
					isdone = true;
				});
				ThreadasyncCallsLock.unlock();

				while (!isdone) {
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			}
		}
	}
}