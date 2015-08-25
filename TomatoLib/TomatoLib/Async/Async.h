#pragma once
#ifndef __TL__ASYNC_H_
#define __TL__ASYNC_H_

#include <vector>
#include <functional>

#ifndef _MSC_VER
#include <pthread.h>
#endif

namespace TomatoLib {
	namespace Async {
#ifdef _MSC_VER
		extern unsigned long MainThreadID;
		extern unsigned long AsyncThreadID;
#else
		extern pthread_t MainThreadID;
		extern pthread_t AsyncThreadID;
#endif

		extern unsigned int CallsToDoOnAsyncThreadIndex;
		extern std::vector<std::function<void()>> CallsToDoOnMainThread;
		extern std::vector<std::function<void()>> CallsToDoOnAsyncThread;

		void RunOnMainThread(std::function<void()> func, bool isblocking = false, bool forcequeue = false);
		void RunOnAsyncThread(std::function<void()> func, bool isblocking = false, bool forcequeue = false);
		void ClearAsyncThreadCalls();
		void RunMainThreadCalls();
		void RunAsyncThreadCalls();
		bool IsMainThread();
		bool IsAsyncThread();
		void Init();
		void Shutdown();
	}
}

#endif