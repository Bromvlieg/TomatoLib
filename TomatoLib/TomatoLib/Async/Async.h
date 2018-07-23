#pragma once
#ifndef __TL__ASYNC_H_
#define __TL__ASYNC_H_

#include <mutex>
#include <chrono>
#include <vector>
#include <thread>
#include <functional>
#include <map>
#include "../Utilities/ringbuffer.h"

#ifndef _MSC_VER
#include <pthread.h>
#endif

#include "../Utilities/Dictonary.h"

namespace TomatoLib {
	namespace Async {
#ifdef _MSC_VER
		extern unsigned long MainThreadID;
		extern unsigned long AsyncThreadID;
#else
		extern pthread_t MainThreadID;
		extern pthread_t AsyncThreadID;
#endif

		extern bool IsInited();
		extern unsigned int CallsToDoOnAsyncThreadIndex;
		extern ringbuffer<std::function<void()>> CallsToDoOnMainThread;
		extern ringbuffer<std::function<void()>> CallsToDoOnAsyncThread;
		extern std::map<unsigned long, ringbuffer<std::function<void()>>> CallsToDoOnThreads;

		void RunOnThread(const std::function<void()>& func, unsigned long threadid, bool isblocking = false, bool forcequeue = false);
		void RunOnMainThread(const std::function<void()>& func, bool isblocking = false, bool forcequeue = false);
		void RunOnAsyncThread(const std::function<void()>& func, bool isblocking = false, bool forcequeue = false);
		void ClearAsyncThreadCalls();
		void RunMainThreadCalls();
		void RunAsyncThreadCalls();
		void RunThreadCalls();
		bool IsMainThread();
		bool IsAsyncThread();
		void Init(int workers = 1);
		void Shutdown();

		void SetThreadName(std::thread* thread, const std::string& threadName);
		void SetThreadName(const std::string& threadName);
	}
}

#endif
