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

		List<std::function<void()>> CallsToDoOnMainThread;
		List<std::function<void()>> CallsToDoOnAsyncThread;
		Dictonary<unsigned long, std::function<void()>> CallsToDoOnThreads;

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

		bool IsInited() {
			return Inited;
		}


		unsigned long GetThreadID() {
#ifdef _MSC_VER
			return GetCurrentThreadId();
#else
			return pthread_self();
#endif
		}

		bool IsMainThread() {
			return Async::MainThreadID == GetThreadID();
		}

		bool IsAsyncThread() {
#ifdef _MSC_VER
			return Async::AsyncThreadID == GetCurrentThreadId();
#else
			return Async::AsyncThreadID == pthread_self();
#endif
		}

		void Init(int workers) {
			if (Inited) return;
			Inited = true;

#ifdef _MSC_VER
			Async::MainThreadID = GetCurrentThreadId();
#else
			Async::MainThreadID = pthread_self();
#endif

			while (workers-- > 0) {
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

		std::mutex ThreadsSepCallsLock;
		void RunOnThread(std::function<void()> func, unsigned long threadid, bool isblocking, bool forcequeue) {
			if (Async::GetThreadID() == threadid && !forcequeue) {
				func();
				return;
			}

			if (!isblocking) {
				ThreadsSepCallsLock.lock();
				Async::CallsToDoOnThreads.Add(threadid, func);
				ThreadsSepCallsLock.unlock();
			} else {
				bool isdone = false;

				ThreadsSepCallsLock.lock();
				Async::CallsToDoOnThreads.Add(threadid, [&isdone, func]() {
					func();
					isdone = true;
				});
				ThreadsSepCallsLock.unlock();

				while (!isdone) {
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			}
		}

		void RunThreadCalls() {
			unsigned long tid = Async::GetThreadID();
			unsigned int i = 0;

			ThreadsSepCallsLock.lock();
			for (int i = 0; i < Async::CallsToDoOnThreads.Count; i++) {
				if (Async::CallsToDoOnThreads.Keys[i] != tid) continue;

				std::function<void()> func = Async::CallsToDoOnThreads.RemoveAt(i);
				ThreadsSepCallsLock.unlock();

				func();

				ThreadsSepCallsLock.lock();
			}
			ThreadsSepCallsLock.unlock();
		}

		std::mutex ThreadasyncCallsLock;
		int doingAsyncCall = 0;
		void RunAsyncThreadCalls() {
			Async::CallsToDoOnAsyncThreadIndex = 0;
			while (true) {
				ThreadasyncCallsLock.lock();

				if ((unsigned int)Async::CallsToDoOnAsyncThread.size() == 0) {
					ThreadasyncCallsLock.unlock();

					std::this_thread::sleep_for(std::chrono::milliseconds(1));
					break;
				}

				std::function<void()> func = Async::CallsToDoOnAsyncThread.RemoveAt(0);

				doingAsyncCall++;
				ThreadasyncCallsLock.unlock();

				func();

				ThreadasyncCallsLock.lock();
				doingAsyncCall--;
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