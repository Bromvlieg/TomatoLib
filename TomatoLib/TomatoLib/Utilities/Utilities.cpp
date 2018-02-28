#include "Utilities.h"
#include "../Config.h"
#include "../UI/UIManager.h"
#include "../UI/Menus/UIConsole.h"
#include "../Async/Async.h"

#include <chrono>
#include <string>
#include <sstream>
#include <cstring>
#include <cstdarg>

#ifdef LINUX
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#else
#include <winsock.h>
#include <time.h>
#include <windows.h>

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif
#endif

#ifdef TL_ENABLE_GLFW
#include <GLFW/glfw3.h>
#endif

namespace TomatoLib {
	namespace Utilities {
		struct timezone {
			int  tz_minuteswest; /* minutes W of Greenwich */
			int  tz_dsttime;     /* type of dst correction */
		};

#ifdef _MSC_VER
		int gettimeofday(struct timeval *tv, struct timezone *tz) {
			FILETIME ft;
			unsigned __int64 tmpres = 0;
			static int tzflag = 0;

			if (nullptr != tv) {
				GetSystemTimeAsFileTime(&ft);

				tmpres |= ft.dwHighDateTime;
				tmpres <<= 32;
				tmpres |= ft.dwLowDateTime;

				tmpres /= 10;  /*convert into microseconds*/
				/*converting file time to unix epoch*/
				tmpres -= DELTA_EPOCH_IN_MICROSECS;
				tv->tv_sec = (long)(tmpres / 1000000UL);
				tv->tv_usec = (long)(tmpres % 1000000UL);
			}

			if (nullptr != tz) {
				if (!tzflag) {
					_tzset();
					tzflag++;
				}
				tz->tz_minuteswest = _timezone / 60;
				tz->tz_dsttime = _daylight;
			}

			return 0;
		}
#endif

		bool randinited = false;
		int GetRandom(int min, int max) {
			if (!randinited) {
				randinited = true;

				srand((unsigned int)time(nullptr));
			}

			return rand() % (max - min) + min;
		}
		
		time_t starttime = 0;
		void ResetTime(){
			struct timeval start;

			time_t mtime, seconds, useconds;

			gettimeofday(&start, nullptr);

			seconds  = start.tv_sec;
			useconds = start.tv_usec;

			mtime = (time_t)(((seconds)* 1000 + useconds / 1000.0) + 0.5);
			
			starttime = mtime;
		}

		float GetTimeMS() {
			static bool inited = false;
			if (!inited) {
				ResetTime();
				inited = true;
			}

			struct timeval start;

			time_t mtime, seconds, useconds;

			gettimeofday(&start, nullptr);

			seconds = start.tv_sec;
			useconds = start.tv_usec;

			mtime = (time_t)(((seconds)* 1000 + useconds / 1000.0) + 0.5);

			return (float)(mtime - starttime);
		}

		int Clamp(int val, int min, int max) { return val < min ? min : val > max ? max : val; }
		float Clamp(float val, float min, float max) { return val < min ? min : val > max ? max : val; }
		double Clamp(double val, double min, double max) { return val < min ? min : val > max ? max : val; }

		float Lerp(float val, float val2, float time){ return val + time * (val2 - val); }

		std::string GetFormattedImlp(const char* format, ...) {
			static const int initial_buf_size = 128;

			va_list arglist;
			va_start(arglist, format);
			char buf1[initial_buf_size];
			const int len = vsnprintf(buf1, initial_buf_size, format, arglist) + 1;
			va_end(arglist);

			if (len < initial_buf_size) {
				return buf1;
			} else {
				char* buf2 = new char[len];

				va_start(arglist, format);
				vsnprintf(buf2, len, format, arglist);
				va_end(arglist);

				std::string ret = buf2;
				delete[] buf2;

				return ret;
			}
		}

		std::string GetDurationFormated(time_t seconds, int secondMode, int minuteMode, int hourMode, int dayMode, int weekMode, int monthMode, int yearMode) {
			tm* ltime = gmtime(&seconds);
			ltime->tm_year -= 70;
			ltime->tm_mday -= 1;

			std::string str;
			bool force = false;

			if (yearMode == 2 || ltime->tm_year > 0) force = true;
			if (force) str += Utilities::GetNumberPadded(ltime->tm_year, 2, '0') + ":";

			if (monthMode == 2 || ltime->tm_mon > 0) force = true;
			if (force) str += Utilities::GetNumberPadded(ltime->tm_mon, 2, '0') + ":";

			if (weekMode == 2 || (ltime->tm_mday - ltime->tm_wday - 1) / 7 > 0) force = true;
			if (force) str += Utilities::GetNumberPadded((ltime->tm_mday - ltime->tm_wday - 1) / 7, 2, '0') + ":";

			if (dayMode == 2 || ltime->tm_mday - 1 > 0) force = true;
			if (force) str += Utilities::GetNumberPadded(ltime->tm_mday, 2, '0') + ":";

			if (hourMode == 2 || ltime->tm_hour > 0) force = true;
			if (force) str += Utilities::GetNumberPadded(ltime->tm_hour, 2, '0') + ":";

			if (minuteMode == 2 || ltime->tm_min > 0) force = true;
			if (force) str += Utilities::GetNumberPadded(ltime->tm_min, 2, '0') + ":";

			if (secondMode == 2 || ltime->tm_sec > 0) force = true;
			if (force) str += Utilities::GetNumberPadded(ltime->tm_sec, 2, '0');

			return str;
		}

		std::string StringReplace(const std::string& haystack, const std::string& needle, const std::string& replacement) {
			if (needle.empty()) return "";
			std::string ret(haystack);

			size_t start_pos = 0;
			while ((start_pos = ret.find(needle, start_pos)) != std::string::npos) {
				ret.replace(start_pos, needle.length(), replacement);
				start_pos += replacement.length();
			}

			return ret;
		}

		std::string GetDateFormated(int unixtimeraw, const std::string& format) {
			time_t unixtime = (time_t)unixtimeraw;
			struct tm ltime;
			ltime = *localtime(&unixtime);

			char* str = new char[format.size() + 512];
			strftime(str, (int)format.size() + 512, format.c_str(), &ltime);

			std::string ret(str);
			delete[] str;

			return ret;
		}

		std::string GetNumberPadded(int value, int padding, char letter) {
			char* unpadded = new char[12];
			char* padded = new char[padding + 1];
			char* unpadbegin = nullptr;
			int   unpadlength;

#ifdef _MSC_VER
			_itoa(value, unpadded, 10);
#else
			snprintf(unpadded, 12, "%d", value);
#endif

			unpadlength = strlen(unpadded);

			if (unpadlength > padding) {
				std::string ret = unpadded;

				delete[] padded;
				delete[] unpadded;

				return ret;
			} else {
				unpadbegin = unpadded;
			}

			memset((void*)padded, letter, padding - unpadlength);
			padded[padding - unpadlength] = '\0';

			strcat(padded, unpadbegin);

			std::string ret = padded;

			delete[] padded;
			delete[] unpadded;

			return ret;
		}

		List<std::string> Split(const std::string &s, char delim) {
			List<std::string> elems;
			std::stringstream ss(s);
			std::string item;
			while (std::getline(ss, item, delim)) {
				elems.Add(item);
			}
			return elems;
		}

		std::string GetConsoleVar(const std::string& name) {
			return TomatoLib::UIBase::DefaultUImanager->Console->GetConsoleVar(name);
		}

		std::string loglocation;
		void SetLogLocation(const std::string& filepath) {
			loglocation = filepath;
		}


		void PrintImpl(const char* format, ...) {
			int buffsize = 512;
			char* szBuffer = new char[buffsize];

			// Get the args list and do a vsprintf to get the right format.
			va_list vL;
			va_start(vL, format);

			// Make sure we don't overflow the buffer by checking against result length
			int iPrintSize = vsnprintf(szBuffer, buffsize, format, vL);
			while (iPrintSize == -1 || buffsize <= iPrintSize) { //fail delete buffer and try again
				if (iPrintSize == -1) buffsize *= 2;
				delete[] szBuffer;

				szBuffer = new char[buffsize + 1]; // +1 for \0
				iPrintSize = vsnprintf(szBuffer, buffsize + 1, format, vL);
			}
			va_end(vL);

			if (TomatoLib::UIBase::DefaultUImanager != nullptr) {
				if (TomatoLib::UIBase::DefaultUImanager->Console != nullptr) {
					if (TomatoLib::Async::IsMainThread()) {
						TomatoLib::UIBase::DefaultUImanager->Console->Print(szBuffer);
					} else if (Async::IsInited()) {
						std::string strcopy = szBuffer;
						Async::RunOnMainThread([strcopy]() {
							TomatoLib::UIBase::DefaultUImanager->Console->Print(strcopy);
						});
					}
				}
			}

			printf("%s\n", szBuffer);

			if (loglocation.size() > 0) {
				FILE* log = fopen(loglocation.c_str(), "a");
				if (log != nullptr) {
					char timebuff[100];
					time_t now = time(nullptr);
					
					int len = strftime(timebuff, 100, "[%Y-%m-%d %H:%M:%S] ", localtime(&now));

					fwrite(timebuff, 1, len, log);
					fwrite(szBuffer, 1, iPrintSize, log);
					fwrite("\n", 1, 1, log);

					fclose(log);
				}
			}

			delete[] szBuffer;
		}
	}
}