#pragma once

#include <mutex>
#include <vector>

namespace TomatoLib {
	template<class T>
	class ringbuffer {
		size_t m_size;
		size_t m_readpos = 0;
		size_t m_writepos = 0;
		std::mutex m_read_mutex {};
		std::mutex m_write_mutex {};
		std::vector<T> m_buffer;

	public:
		ringbuffer() = default;

		ringbuffer(size_t size) : m_size(size), m_buffer() {
			m_buffer.reserve(m_size);
		}

		void reserve(size_t size) {
			this->m_size = size;
			m_buffer.reserve(size);
		}

		size_t size() {
			return this->m_size;
		}

		void clear() {
			m_buffer.clear();

			this->m_readpos = 0;
			this->m_writepos = 0;
		}

		void push(const T& func) {
			std::lock_guard<std::mutex> lock {m_write_mutex};
			while ((m_writepos + 1) % m_size == m_readpos) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}

			m_buffer[m_writepos++] = func;
			m_writepos = m_writepos % m_size;
		}

		T pop() {
			std::lock_guard<std::mutex> lock {m_read_mutex};
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
}
