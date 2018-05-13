#pragma once
#ifndef __TL__ASYNC_H_
#define __TL__ASYNC_H_

#include <vector>
#include <functional>

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
		extern List<std::function<void()>> CallsToDoOnMainThread;
		extern List<std::function<void()>> CallsToDoOnAsyncThread;
		extern Dictonary<unsigned long, std::function<void()>> CallsToDoOnThreads;

		void RunOnThread(std::function<void()> func, unsigned long threadid, bool isblocking = false, bool forcequeue = false);
		void RunOnMainThread(std::function<void()> func, bool isblocking = false, bool forcequeue = false);
		void RunOnAsyncThread(std::function<void()> func, bool isblocking = false, bool forcequeue = false);
		void ClearAsyncThreadCalls();
		void RunMainThreadCalls();
		void RunAsyncThreadCalls();
		void RunThreadCalls();
		bool IsMainThread();
		bool IsAsyncThread();
		void Init(int workers = 1);
		void Shutdown();
	}
}

#endif