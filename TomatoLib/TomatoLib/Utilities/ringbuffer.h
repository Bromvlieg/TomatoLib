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
		ringbuffer(size_t size) : m_size(size) {
			this->m_buffer.resize(m_size);
		}

		void reserve(size_t size) {
			std::lock_guard<std::mutex> lockread {this->m_read_mutex};
			std::lock_guard<std::mutex> lockwrite {this->m_write_mutex};
			this->m_size = size;
			this->m_buffer.resize(size);
		}

		size_t size() {
			return this->m_size;
		}

		void clear() {
			std::lock_guard<std::mutex> lockread {this->m_read_mutex};
			std::lock_guard<std::mutex> lockwrite {this->m_write_mutex};
			this->m_buffer.clear();

			this->m_readpos = 0;
			this->m_writepos = 0;
		}

		void push(const T& func) {
			std::lock_guard<std::mutex> lock {this->m_write_mutex};
			while ((this->m_writepos + 1) % this->m_size == this->m_readpos) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}

			// store new index
			size_t writepos = this->m_writepos + 1;
			writepos %= this->m_size;
			this->m_buffer[writepos] = func;

			// save it
			this->m_writepos = writepos % this->m_size;
		}

		T pop() {
			std::lock_guard<std::mutex> lock {this->m_read_mutex};
			while (this->m_readpos == this->m_writepos) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}

			// store read index
			size_t readpos = this->m_readpos + 1;
			readpos %= this->m_size;
			auto& obj = this->m_buffer[readpos];

			this->m_readpos = readpos;
			return obj;
		}

		bool available() {
			return this->m_readpos != this->m_writepos;
		}

		size_t available_count() {
			return ((this->m_writepos + this->m_size) - this->m_readpos) % this->m_size;
		}
	};
}
