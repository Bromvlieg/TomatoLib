#include "Utilities.h"
#include "../UI/UIManager.h"
#include "../UI/Menus/UIConsole.h"

#include <chrono>
#include <string>
#include <sstream>
#include <cstring>
#include <cstdarg>

namespace TomatoLib {
	namespace Utilities {
		bool randinited = false;
		int GetRandom(int min, int max) {
			if (!randinited) {
				randinited = true;

				srand((unsigned int)time(NULL));
			}

			return rand() % (max - min) + min;
		}

		int Clamp(int val, int min, int max) { return val < min ? min : val > max ? max : val; }
		float Clamp(float val, float min, float max) { return val < min ? min : val > max ? max : val; }
		double Clamp(double val, double min, double max) { return val < min ? min : val > max ? max : val; }

		std::string GetFormatted(std::string format, ...) {
			int size = 512;
			char* buffer = new char[size];
			const char* fbuff = format.c_str();

			va_list vl;
			va_start(vl, format);

			int nsize = vsnprintf(buffer, size, fbuff, vl);
			while (nsize == -1 || size <= nsize) { //fail delete buffer and try again
				delete[] buffer;

				if (nsize == -1) size *= 2;

				buffer = new char[size + 1]; //+1 for /0
				nsize = vsnprintf(buffer, size, fbuff, vl);
			}

			va_end(vl);

			std::string ret = buffer;
			delete[] buffer;
			return ret;
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

		std::string GetDateFormated(int unixtimeraw, std::string format) {
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
			char* unpadbegin = NULL;
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

		std::string GetConsoleVar(std::string name) { return TomatoLib::UIBase::DefaultUImanager->Console->GetConsoleVar(name); }
		void Print(std::string format, ...) {
			int buffsize = 512;
			char* szBuffer = new char[buffsize];

			// Get the args list and do a vsprintf to get the right format.
			va_list vL;
			va_start(vL, format);

			// Make sure we don't overflow the buffer by checking against result length
			int iPrintSize = vsnprintf(szBuffer, buffsize, format.c_str(), vL);
			while (iPrintSize == -1 || buffsize <= iPrintSize) { //fail delete buffer and try again
				if (iPrintSize == -1) buffsize *= 2;
				delete[] szBuffer;

				szBuffer = new char[buffsize + 1]; // +1 for \0
				iPrintSize = vsnprintf(szBuffer, buffsize + 1, format.c_str(), vL);
			}
			va_end(vL);

			if (TomatoLib::UIBase::DefaultUImanager != nullptr) {
				if (TomatoLib::UIBase::DefaultUImanager->Console != nullptr) {
					TomatoLib::UIBase::DefaultUImanager->Console->Print(szBuffer);
				}
			}

			printf("%s\n", szBuffer);

			delete[] szBuffer;
		}
	}
}