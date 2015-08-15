#include "HTTP.h"

#include "../Networking/Packet.h"
#include "../Networking/EzSock.h"

#include <thread>
#include <vector>
#include <algorithm>

namespace TomatoLib {
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

	void HTTP::Request(std::string url, std::string method, const std::string& body, const std::string& bodytype, std::function<void(bool, Dictonary<std::string, std::string>&, unsigned char*, unsigned int)> callback) {
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

			unsigned char* data = 0;
			unsigned int len = 0;
			if (headers.ContainsKey("content-length")) {
				len = atoi(headers["content-length"].c_str());
				data = p.ReadBytes(len);
			} else if (headers.ContainsKey("transfer-encoding")) {
				std::vector<unsigned char> datavec;
				while (true) {
					const char* chunklendata = p.ReadUntil("\r\n");
					int toreceive = (int)strtol(chunklendata, NULL, 16);
					delete[] chunklendata;

					if (toreceive == 0) break;
					unsigned char* chunkdata = p.ReadBytes(toreceive + 2);
					datavec.reserve(toreceive);
					for (int i = 0; i < toreceive; i++) datavec.push_back(chunkdata[i]);
				}

				callback(true, headers, &datavec[0], datavec.size());
				return;
			} else {
				// no content-length? fuck this shit.
				callback(false, headers, 0, 0);
				return;
			}

			callback(true, headers, data, len);
			return;
		});
	}
}