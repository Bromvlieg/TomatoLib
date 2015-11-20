#include "HTTP.h"

#include "../Networking/Packet.h"
#include "../Networking/EzSock.h"

#include <thread>
#include <vector>
#include <algorithm>

#include <string.h>

namespace TomatoLib {
	string HTTP::UrlEncode(const string& str) {
		string new_str = "";
		char c;
		int ic;
		const char* chars = str.c_str();
		char bufHex[10];
		int len = strlen(chars);

		for (int i = 0; i<len; i++) {
			c = chars[i];
			ic = c;
			// uncomment this if you want to encode spaces with +
			if (c==' ') new_str += '+';
			else if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') new_str += c;
			else {
				sprintf(bufHex, "%X", c);
				if (ic < 16)
					new_str += "%0";
				else
					new_str += "%";
				new_str += bufHex;
			}
		}
		return new_str;
	}

	string HTTP::UrlDecode(const string& str) {
		string ret;
		char ch;
		int i, ii, len = str.length();

		for (i = 0; i < len; i++) {
			if (str[i] != '%') {
				if (str[i] == '+')
					ret += ' ';
				else
					ret += str[i];
			} else {
				sscanf(str.substr(i + 1, 2).c_str(), "%x", &ii);
				ch = static_cast<char>(ii);
				ret += ch;
				i = i + 2;
			}
		}
		return ret;
	}

	Dictonary<std::string, std::string> HTTP::ParseAsUrlEncoded(const std::string& str) {
		Dictonary<std::string, std::string> ret;

		int len = str.length();
		int start = 0;
		std::string key;

		for (int i = 0; i < len; i++) {
			if (str[i] == '=') {
				key = str.substr(start, i - start);
				start = ++i;
			}else if (str[i] == '&') {
				ret[key] = str.substr(start, i - start);
				key = "";
				start = ++i;
			}
		}

		if (start != len) {
			ret[key] = str.substr(start);
		}

		return ret;
	}

	void HTTP::Request(std::string url, std::string method, const std::string& body, const std::string& bodytype, std::function<void(bool success, Dictonary<std::string, std::string>& headers, unsigned char* body, unsigned int bodysize)> callback) {
		if (url.find("http://") == 0) {
			url = url.substr(7);
		}

		new std::thread([url, method, callback, body, bodytype]() {
			Dictonary<std::string, std::string > headers;
			std::string host;
			std::string path;

			size_t pathindex = url.find("/");
			if (pathindex != std::string::npos) {
				host = url.substr(0, pathindex);
				path = url.substr(pathindex);
			} else {
				host = url;
				path = "/";
			}

			EzSock sock;
			Packet p(&sock);

			sock.create();
			if (sock.connect(host.c_str(), 80) != null) {
				callback(false, headers, 0, 0);
				return;
			}

			p.WriteLine((method + " " + path + " HTTP/1.1").c_str());
			p.WriteLine((std::string("Host: ") + host).c_str());
			p.WriteLine((std::string("Content-Length: ") + std::to_string(body.length())).c_str());
			if (bodytype.length() > 0) p.WriteLine((std::string("Content-Type: ") + bodytype).c_str());
			p.WriteLine("");
			p.WriteBytes((byte*)body.c_str(), body.length(), false);

			sock.SendRaw(p.OutBuffer, p.OutPos);
			p.Clear();
			
			const char* headerdata = p.ReadUntil("\r\n\r\n");
			std::string headerstr(headerdata);
			delete[] headerdata;

			size_t curpos = 0;
			size_t oldpos = curpos;
			bool first = false;
			while ((curpos = headerstr.find("\r\n", curpos)) != std::string::npos) {
				std::string line = headerstr.substr(oldpos, curpos - oldpos);
				if (line.size() == 0) break; // end of headers

				if (!first) {
					first = true;
					headers["status"] = line.substr(9);
					continue;
				}

				size_t splitter = line.find(": ");

				std::string key = line.substr(0, splitter);
				std::transform(key.begin(), key.end(), key.begin(), ::tolower);

				std::string value = line.substr(splitter + 2, -2);
				headers[key] = value;

				curpos += 2;
				oldpos = curpos;
			}

			if (headers.ContainsKey("content-length")) {
				unsigned int len = atoi(headers["content-length"].c_str());
				unsigned char* data = new unsigned char[len + 1];
				memcpy(data, p.ReadBytes(len), len);

				data[len] = 0; // add null terminator, just to be nice
				callback(true, headers, data, len);

				delete[] data;
				return;
			} else if (headers.ContainsKey("transfer-encoding")) {
				std::vector<unsigned char> datavec;
				while (true) {
					char* chunklendata = p.ReadUntil("\r\n");
					chunklendata[strlen(chunklendata) - 2] = 0;
					int toreceive = (int)strtol(chunklendata, NULL, 16);
					delete[] chunklendata;

					if (toreceive == 0) break;
					unsigned char* chunkdata = p.ReadBytes(toreceive + 2);
					datavec.reserve(toreceive);
					for (int i = 0; i < toreceive; i++) datavec.push_back(chunkdata[i]);
				}

				datavec.push_back(0); // add null terminator, just to be nice
				callback(true, headers, &datavec[0], datavec.size() - 1);
				return;
			} else {
				// no content-length? fuck this shit.
				callback(false, headers, nullptr, 0);
				return;
			}
		});
	}
}