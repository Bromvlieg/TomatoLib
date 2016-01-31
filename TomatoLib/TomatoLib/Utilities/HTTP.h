#pragma once
#ifndef __HTTP_H__
#define __HTTP_H__

#include <string>
#include <functional>
#include "Dictonary.h"

namespace TomatoLib {
	class HTTP {
	public:
		static std::string UrlEncode(const std::string& str);
		static std::string UrlDecode(const std::string& str);
		static void Request(std::string url, std::string method, const std::string& body, const std::string& bodytype, std::function<void(bool, Dictonary<std::string, std::string>&, unsigned char*, unsigned int)> callback);
		static void Request(std::string url, std::string method, const std::string& body, const std::string& bodytype, Dictonary<std::string, std::string>& headers, std::function<void(bool, Dictonary<std::string, std::string>&, unsigned char*, unsigned int)> callback);
		static Dictonary < std::string, std::string> ParseAsUrlEncoded(const std::string& urlencodeddata);
		
		static size_t DecodeHTML(char* dest, const char* src);
	};
}
#endif
