#include "Async.h"

#include <mutex>
#include <chrono>
#include <thread>
#include <string>

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
				int i = workers;
				new std::thread([i]() {
					SetThreadName("TL:async_#" + std::to_string(i));

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

		void RunOnMainThread(const std::function<void()>& func, bool isblocking, bool forcequeue) {
			if (Async::IsMainThread() && !forcequeue) {
				func();
				return;
			}

			if (!isblocking) {
				ThreadCallsLock.lock();
				Async::CallsToDoOnMainThread.push_back(func);
				ThreadCallsLock.unlock();

				return;
			}

			bool isdone = false;

			ThreadCallsLock.lock();
			Async::CallsToDoOnMainThread.push_back([&isdone, &func]() {
				func();
				isdone = true;
			});

			ThreadCallsLock.unlock();

			while (!isdone) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		}

		std::mutex ThreadsSepCallsLock;
		void RunOnThread(const std::function<void()>& func, unsigned long threadid, bool isblocking, bool forcequeue) {
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
		
		void RunOnAsyncThread(const std::function<void()>& func, bool isblocking, bool forcequeue) {
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

#ifdef _WIN32
#include <windows.h>
		const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push,8)
		typedef struct tagTHREADNAME_INFO {
			DWORD dwType; // Must be 0x1000.
			LPCSTR szName; // Pointer to name (in user addr space).
			DWORD dwThreadID; // Thread ID (-1=caller thread).
			DWORD dwFlags; // Reserved for future use, must be zero.
		} THREADNAME_INFO;
#pragma pack(pop)

		void SetThreadName(uint32_t dwThreadID, const std::string& threadName) {
			THREADNAME_INFO info;
			info.dwType = 0x1000;
			info.szName = threadName.c_str();
			info.dwThreadID = dwThreadID;
			info.dwFlags = 0;

			__try {
				RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
			} __except (EXCEPTION_EXECUTE_HANDLER) {
			}
		}
		void SetThreadName(const std::string& threadName) {
			SetThreadName(GetCurrentThreadId(), threadName);
		}

		void SetThreadName(std::thread* thread, const std::string& threadName) {
			DWORD threadId = GetThreadId(static_cast<HANDLE>(thread->native_handle()));
			SetThreadName(threadId, threadName);
		}

#else
		void SetThreadName(std::thread* thread, const std::string& threadName) {
			auto handle = thread->native_handle();
			pthread_setname_np(handle, threadName.c_str());
		}

#include <sys/prctl.h>
		void SetThreadName(const std::string& threadName) {
			prctl(PR_SET_NAME, threadName.c_str(), 0, 0, 0);
		}
#endif
	}
}