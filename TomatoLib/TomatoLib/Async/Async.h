#pragma once
#ifndef __TL__ASYNC_H_
#define __TL__ASYNC_H_

#include <mutex>
#include <chrono>
#include <vector>
#include <thread>
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
		class ringbuffer {
			using Func = std::function<void()>;

			size_t m_size;
			size_t m_readpos = 0;
			size_t m_writepos = 0;
			std::mutex m_read_mutex{};
			std::mutex m_write_mutex{};
			std::vector<Func> m_buffer;

			public:
			ringbuffer(size_t size) : m_size(size), m_buffer() {
				m_buffer.reserve(m_size);
			}

			void push(const Func& func) {
				std::lock_guard<std::mutex> lock{m_write_mutex};
				while ((m_writepos + 1) % m_size == m_readpos) {
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}

				m_buffer[m_writepos++] = func;
				m_writepos = m_writepos % m_size;
			}

			Func pop() {
				std::lock_guard<std::mutex> lock{m_read_mutex};
				while (m_readpos == m_writepos) {
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}

				size_t readpos = m_readpos++;
				m_readpos = m_readpos % m_size;

				return m_buffer[readpos];
			}

			bool available() {
				return m_readpos != m_writepos;
			}
			
			size_t available_count() {
				return ((m_writepos + m_size) - m_readpos) % m_size;
			}
		};


		extern bool IsInited();
		extern unsigned int CallsToDoOnAsyncThreadIndex;
		extern ringbuffer CallsToDoOnMainThread;
		extern List<std::function<void()>> CallsToDoOnAsyncThread;
		extern Dictonary<unsigned long, std::function<void()>> CallsToDoOnThreads;

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
