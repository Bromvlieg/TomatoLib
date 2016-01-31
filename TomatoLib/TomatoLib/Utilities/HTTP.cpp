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

		for (int i = 0; i < len; i++) {
			c = chars[i];
			ic = c;
			// uncomment this if you want to encode spaces with +
			if (c == ' ') new_str += '+';
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
			} else if (str[i] == '&') {
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
		Dictonary<std::string, std::string> headers;
		HTTP::Request(url, method, body, bodytype, headers, callback);
	}

	void HTTP::Request(std::string url, std::string method, const std::string& body, const std::string& bodytype, Dictonary<std::string, std::string>& reqheaders, std::function<void(bool success, Dictonary<std::string, std::string>& headers, unsigned char* body, unsigned int bodysize)> callback) {
		if (url.find("http://") == 0) {
			url = url.substr(7);
		}

		new std::thread([url, method, callback, body, bodytype, reqheaders]() {
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
			if (sock.connect(host.c_str(), 80) != 0) {
				callback(false, headers, 0, 0);
				return;
			}

			p.WriteLine((method + " " + path + " HTTP/1.1").c_str());
			p.WriteLine((std::string("Host: ") + host).c_str());
			p.WriteLine((std::string("Content-Length: ") + std::to_string(body.length())).c_str());
			if (bodytype.length() > 0) p.WriteLine((std::string("Content-Type: ") + bodytype).c_str());

			for (int i = 0; i < reqheaders.Count; i++) {
				p.WriteLine((reqheaders.Keys[i] + ": " + reqheaders.Values[i]).c_str());
			}

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
				// no content-length, give up
				callback(false, headers, nullptr, 0);
				return;
			}
		});
	}


	/*
	for the HTML decode
	Copyright 2012 Christoph Gärtner
	Distributed under the Boost Software License, Version 1.0
	*/
#pragma warning( push )
#pragma warning( disable : 4566)

	#define UNICODE_MAX 0x10FFFFul
	static const char *const NAMED_ENTITIES[][2] = {
			{"AElig;", "Æ"},
			{"Aacute;", "Á"},
			{"Acirc;", "Â"},
			{"Agrave;", "À"},
			{"Alpha;", "Α"},
			{"Aring;", "Å"},
			{"Atilde;", "Ã"},
			{"Auml;", "Ä"},
			{"Beta;", "Β"},
			{"Ccedil;", "Ç"},
			{"Chi;", "Χ"},
			{"Dagger;", "‡"},
			{"Delta;", "Δ"},
			{"ETH;", "Ð"},
			{"Eacute;", "É"},
			{"Ecirc;", "Ê"},
			{"Egrave;", "È"},
			{"Epsilon;", "Ε"},
			{"Eta;", "Η"},
			{"Euml;", "Ë"},
			{"Gamma;", "Γ"},
			{"Iacute;", "Í"},
			{"Icirc;", "Î"},
			{"Igrave;", "Ì"},
			{"Iota;", "Ι"},
			{"Iuml;", "Ï"},
			{"Kappa;", "Κ"},
			{"Lambda;", "Λ"},
			{"Mu;", "Μ"},
			{"Ntilde;", "Ñ"},
			{"Nu;", "Ν"},
			{"OElig;", "Œ"},
			{"Oacute;", "Ó"},
			{"Ocirc;", "Ô"},
			{"Ograve;", "Ò"},
			{"Omega;", "Ω"},
			{"Omicron;", "Ο"},
			{"Oslash;", "Ø"},
			{"Otilde;", "Õ"},
			{"Ouml;", "Ö"},
			{"Phi;", "Φ"},
			{"Pi;", "Π"},
			{"Prime;", "″"},
			{"Psi;", "Ψ"},
			{"Rho;", "Ρ"},
			{"Scaron;", "Š"},
			{"Sigma;", "Σ"},
			{"THORN;", "Þ"},
			{"Tau;", "Τ"},
			{"Theta;", "Θ"},
			{"Uacute;", "Ú"},
			{"Ucirc;", "Û"},
			{"Ugrave;", "Ù"},
			{"Upsilon;", "Υ"},
			{"Uuml;", "Ü"},
			{"Xi;", "Ξ"},
			{"Yacute;", "Ý"},
			{"Yuml;", "Ÿ"},
			{"Zeta;", "Ζ"},
			{"aacute;", "á"},
			{"acirc;", "â"},
			{"acute;", "´"},
			{"aelig;", "æ"},
			{"agrave;", "à"},
			{"alefsym;", "ℵ"},
			{"alpha;", "α"},
			{"amp;", "&"},
			{"and;", "∧"},
			{"ang;", "∠"},
			{"apos;", "'"},
			{"aring;", "å"},
			{"asymp;", "≈"},
			{"atilde;", "ã"},
			{"auml;", "ä"},
			{"bdquo;", "„"},
			{"beta;", "β"},
			{"brvbar;", "¦"},
			{"bull;", "•"},
			{"cap;", "∩"},
			{"ccedil;", "ç"},
			{"cedil;", "¸"},
			{"cent;", "¢"},
			{"chi;", "χ"},
			{"circ;", "ˆ"},
			{"clubs;", "♣"},
			{"cong;", "≅"},
			{"copy;", "©"},
			{"crarr;", "↵"},
			{"cup;", "∪"},
			{"curren;", "¤"},
			{"dArr;", "⇓"},
			{"dagger;", "†"},
			{"darr;", "↓"},
			{"deg;", "°"},
			{"delta;", "δ"},
			{"diams;", "♦"},
			{"divide;", "÷"},
			{"eacute;", "é"},
			{"ecirc;", "ê"},
			{"egrave;", "è"},
			{"empty;", "∅"},
			{"emsp;", " "},
			{"ensp;", " "},
			{"epsilon;", "ε"},
			{"equiv;", "≡"},
			{"eta;", "η"},
			{"eth;", "ð"},
			{"euml;", "ë"},
			{"euro;", "€"},
			{"exist;", "∃"},
			{"fnof;", "ƒ"},
			{"forall;", "∀"},
			{"frac12;", "½"},
			{"frac14;", "¼"},
			{"frac34;", "¾"},
			{"frasl;", "⁄"},
			{"gamma;", "γ"},
			{"ge;", "≥"},
			{"gt;", ">"},
			{"hArr;", "⇔"},
			{"harr;", "↔"},
			{"hearts;", "♥"},
			{"hellip;", "…"},
			{"iacute;", "í"},
			{"icirc;", "î"},
			{"iexcl;", "¡"},
			{"igrave;", "ì"},
			{"image;", "ℑ"},
			{"infin;", "∞"},
			{"int;", "∫"},
			{"iota;", "ι"},
			{"iquest;", "¿"},
			{"isin;", "∈"},
			{"iuml;", "ï"},
			{"kappa;", "κ"},
			{"lArr;", "⇐"},
			{"lambda;", "λ"},
			{"lang;", "〈"},
			{"laquo;", "«"},
			{"larr;", "←"},
			{"lceil;", "⌈"},
			{"ldquo;", "“"},
			{"le;", "≤"},
			{"lfloor;", "⌊"},
			{"lowast;", "∗"},
			{"loz;", "◊"},
			{"lrm;", "\xE2\x80\x8E"},
			{"lsaquo;", "‹"},
			{"lsquo;", "‘"},
			{"lt;", "<"},
			{"macr;", "¯"},
			{"mdash;", "—"},
			{"micro;", "µ"},
			{"middot;", "·"},
			{"minus;", "−"},
			{"mu;", "μ"},
			{"nabla;", "∇"},
			{"nbsp;", " "},
			{"ndash;", "–"},
			{"ne;", "≠"},
			{"ni;", "∋"},
			{"not;", "¬"},
			{"notin;", "∉"},
			{"nsub;", "⊄"},
			{"ntilde;", "ñ"},
			{"nu;", "ν"},
			{"oacute;", "ó"},
			{"ocirc;", "ô"},
			{"oelig;", "œ"},
			{"ograve;", "ò"},
			{"oline;", "‾"},
			{"omega;", "ω"},
			{"omicron;", "ο"},
			{"oplus;", "⊕"},
			{"or;", "∨"},
			{"ordf;", "ª"},
			{"ordm;", "º"},
			{"oslash;", "ø"},
			{"otilde;", "õ"},
			{"otimes;", "⊗"},
			{"ouml;", "ö"},
			{"para;", "¶"},
			{"part;", "∂"},
			{"permil;", "‰"},
			{"perp;", "⊥"},
			{"phi;", "φ"},
			{"pi;", "π"},
			{"piv;", "ϖ"},
			{"plusmn;", "±"},
			{"pound;", "£"},
			{"prime;", "′"},
			{"prod;", "∏"},
			{"prop;", "∝"},
			{"psi;", "ψ"},
			{"quot;", "\""},
			{"rArr;", "⇒"},
			{"radic;", "√"},
			{"rang;", "〉"},
			{"raquo;", "»"},
			{"rarr;", "→"},
			{"rceil;", "⌉"},
			{"rdquo;", "”"},
			{"real;", "ℜ"},
			{"reg;", "®"},
			{"rfloor;", "⌋"},
			{"rho;", "ρ"},
			{"rlm;", "\xE2\x80\x8F"},
			{"rsaquo;", "›"},
			{"rsquo;", "’"},
			{"sbquo;", "‚"},
			{"scaron;", "š"},
			{"sdot;", "⋅"},
			{"sect;", "§"},
			{"shy;", "\xC2\xAD"},
			{"sigma;", "σ"},
			{"sigmaf;", "ς"},
			{"sim;", "∼"},
			{"spades;", "♠"},
			{"sub;", "⊂"},
			{"sube;", "⊆"},
			{"sum;", "∑"},
			{"sup1;", "¹"},
			{"sup2;", "²"},
			{"sup3;", "³"},
			{"sup;", "⊃"},
			{"supe;", "⊇"},
			{"szlig;", "ß"},
			{"tau;", "τ"},
			{"there4;", "∴"},
			{"theta;", "θ"},
			{"thetasym;", "ϑ"},
			{"thinsp;", " "},
			{"thorn;", "þ"},
			{"tilde;", "˜"},
			{"times;", "×"},
			{"trade;", "™"},
			{"uArr;", "⇑"},
			{"uacute;", "ú"},
			{"uarr;", "↑"},
			{"ucirc;", "û"},
			{"ugrave;", "ù"},
			{"uml;", "¨"},
			{"upsih;", "ϒ"},
			{"upsilon;", "υ"},
			{"uuml;", "ü"},
			{"weierp;", "℘"},
			{"xi;", "ξ"},
			{"yacute;", "ý"},
			{"yen;", "¥"},
			{"yuml;", "ÿ"},
			{"zeta;", "ζ"},
			{"zwj;", "\xE2\x80\x8D"},
			{"zwnj;", "\xE2\x80\x8C"}
	};
#pragma warning( pop ) 

	static int cmp(const void *key, const void *value) {
		return strncmp((const char *)key, *(const char **)value,
					   strlen(*(const char **)value));
	}

	static const char *get_named_entity(const char *name) {
		const char *const *entity = (const char *const *)bsearch(name,
																 NAMED_ENTITIES, sizeof NAMED_ENTITIES / sizeof *NAMED_ENTITIES,
																 sizeof *NAMED_ENTITIES, cmp);

		return entity ? entity[1] : NULL;
	}

	static size_t putc_utf8(unsigned long cp, char *buffer) {
		unsigned char *bytes = (unsigned char *)buffer;

		if (cp <= 0x007Ful) {
			bytes[0] = (unsigned char)cp;
			return 1;
		}

		if (cp <= 0x07FFul) {
			bytes[1] = (unsigned char)((2 << 6) | (cp & 0x3F));
			bytes[0] = (unsigned char)((6 << 5) | (cp >> 6));
			return 2;
		}

		if (cp <= 0xFFFFul) {
			bytes[2] = (unsigned char)((2 << 6) | (cp & 0x3F));
			bytes[1] = (unsigned char)((2 << 6) | ((cp >> 6) & 0x3F));
			bytes[0] = (unsigned char)((14 << 4) | (cp >> 12));
			return 3;
		}

		if (cp <= 0x10FFFFul) {
			bytes[3] = (unsigned char)((2 << 6) | (cp & 0x3F));
			bytes[2] = (unsigned char)((2 << 6) | ((cp >> 6) & 0x3F));
			bytes[1] = (unsigned char)((2 << 6) | ((cp >> 12) & 0x3F));
			bytes[0] = (unsigned char)((30 << 3) | (cp >> 18));
			return 4;
		}

		return 0;
	}

	static bool parse_entity(
		const char *current, char **to, const char **from) {
		const char *end = strchr(current, ';');
		if (!end) return 0;

		if (current[1] == '#') {
			char *tail = NULL;
			int errno_save = errno;
			bool hex = current[2] == 'x' || current[2] == 'X';

			errno = 0;
			unsigned long cp = strtoul(
				current + (hex ? 3 : 2), &tail, hex ? 16 : 10);

			bool fail = errno || tail != end || cp > UNICODE_MAX;
			errno = errno_save;
			if (fail) return 0;

			*to += putc_utf8(cp, *to);
			*from = end + 1;

			return 1;
		} else {
			const char *entity = get_named_entity(&current[1]);
			if (!entity) return 0;

			size_t len = strlen(entity);
			memcpy(*to, entity, len);

			*to += len;
			*from = end + 1;

			return 1;
		}
	}

	size_t HTTP::DecodeHTML(char* dest, const char* src) {
		if (!src) src = dest;

		char *to = dest;
		const char *from = src;

		for (const char *current; (current = strchr(from, '&'));) {
			memmove(to, from, (size_t)(current - from));
			to += current - from;

			if (parse_entity(current, &to, &from))
				continue;

			from = current;
			*to++ = *from++;
		}

		size_t remaining = strlen(from);

		memmove(to, from, remaining);
		to += remaining;
		*to = 0;

		return (size_t)(to - dest);
	}
}