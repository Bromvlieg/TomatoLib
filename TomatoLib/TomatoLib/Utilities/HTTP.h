#pragma once
#ifndef __HTTP_H__
#define __HTTP_H__

#include <string>
#include <functional>
#include "Dictonary.h"

namespace TomatoLib {
	class HTTP {
	public:
		static void Request(std::string url, std::string method, const std::string& body, const std::string& bodytype, std::function<void(bool, Dictonary<std::string, std::string>&, unsigned char*, unsigned int)> callback);
		static Dictonary < std::string, std::string> ParseAsUrlEncoded(const std::string& urlencodeddata);
	};
}
#endif
