#pragma once
#ifndef __TL__ASYNC_H_
#define __TL__ASYNC_H_

#include <vector>
#include <functional>

namespace TomatoLib {
	namespace Async {
		extern unsigned long MainThreadID;
		extern unsigned long AsyncThreadID;

		extern unsigned int CallsToDoOnAsyncThreadIndex;
		extern std::vector<std::function<void()>> CallsToDoOnMainThread;
		extern std::vector<std::function<void()>> CallsToDoOnAsyncThread;

		void RunOnMainThread(std::function<void()> func, bool isblocking);
		void RunOnAsyncThread(std::function<void()> func, bool isblocking);
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