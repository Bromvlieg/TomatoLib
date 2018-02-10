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
		void SetLogLocation(const std::string& filepath);

		int GetRandom(int min, int max);
		std::string GetNumberPadded(int value, int padding, char letter);
		std::string GetDurationFormated(time_t seconds, int secondMode = TL_DURATIONF_GTHNZ, int minuteMode = TL_DURATIONF_GTHNZ, int hourMode = TL_DURATIONF_GTHNZ, int dayMode = TL_DURATIONF_GTHNZ, int weekMode = TL_DURATIONF_GTHNZ, int monthMode = TL_DURATIONF_GTHNZ, int yearMode = TL_DURATIONF_GTHNZ);
		std::string GetDateFormated(int unixtime, const std::string& format);
		std::string StringReplace(const std::string& haystack, const std::string& needle, const std::string& replacement);

		void PrintImpl(const char* format, ...);
		std::string GetFormattedImlp(const char* format, ...);

		template<typename... Args>
		std::string GetFormatted(const std::string& format, Args... args) {
			return GetFormattedImlp(format.c_str(), args...);
		}

		template<typename... Args>
		void Print(const std::string& format, Args... args) {
			PrintImpl(format.c_str(), args...);
		}

		List<std::string> Split(const std::string &s, char delim);
		std::string GetConsoleVar(const std::string& name);

		void ResetTime();
		float GetTimeMS();

		int Clamp(int val, int min, int max);
		float Clamp(float val, float min, float max);
		double Clamp(double val, double min, double max);
	}
}

#endif