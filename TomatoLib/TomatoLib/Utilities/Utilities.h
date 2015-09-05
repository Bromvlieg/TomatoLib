#pragma once
#ifndef __TL__UTILITIES_H_
#define __TL__UTILITIES_H_

#include <string>
#include <vector>
#include "List.h"

#define TL_DURATIONF_NEVER 0
#define TL_DURATIONF_GTHNZ 1
#define TL_DURATIONF_ALWAYS 2

namespace TomatoLib {
	namespace Utilities {
		int GetRandom(int min, int max);
		std::string GetNumberPadded(int value, int padding, char letter);
		std::string GetDurationFormated(time_t seconds, int secondMode = TL_DURATIONF_GTHNZ, int minuteMode = TL_DURATIONF_GTHNZ, int hourMode = TL_DURATIONF_GTHNZ, int dayMode = TL_DURATIONF_GTHNZ, int weekMode = TL_DURATIONF_GTHNZ, int monthMode = TL_DURATIONF_GTHNZ, int yearMode = TL_DURATIONF_GTHNZ);
		std::string GetDateFormated(int unixtime, std::string format);
		std::string StringReplace(const std::string& haystack, const std::string& needle, const std::string& replacement);

		std::string GetFormatted(std::string format, ...);
		List<std::string> Split(const std::string &s, char delim);
		void Print(std::string format, ...);
		std::string GetConsoleVar(std::string name);
	}
}

#endif