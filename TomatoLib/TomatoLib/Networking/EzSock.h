#ifndef __EzSock_H__
#define __EzSock_H__

#define _WINSOCKAPI_

#include <sstream>
#include <vector>
#include <fcntl.h>
#include <ctype.h>
#include <cinttypes>

#ifdef _MSC_VER
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif

namespace TomatoLib {
	class Packet;

	using namespace std;

	class EzSock {
	public:
		enum SockState {
			skDISCONNECTED = 0,
			skUNDEF1, //Not implemented
			skLISTENING,
			skUNDEF3, //Not implemented
			skUNDEF4, //Not implemented
			skUNDEF5, //Not implemented
			skUNDEF6, //Not implemented
			skCONNECTED,
			skERROR
		};

		bool blocking = true;
		bool Valid = false;

		struct sockaddr_in addr;
		struct sockaddr_in fromAddr;
		unsigned long fromAddr_len = 0;

		SockState state = skDISCONNECTED;
		int sock = -1;

		int lastCode = 0;

		EzSock();
		~EzSock();

		bool create();
		bool create(int Protocol);
		bool create(int Protocol, int Type);
		bool bind(unsigned short port);
		bool listen();
		bool accept(EzSock* socket);
		int connect(const char* host, unsigned short port);
		void close();

		uint64_t uAddr();
		bool IsError();

		bool CanRead();

		int Receive(unsigned char* buffer, int size, int spos = 0);
		int SendRaw(const unsigned char* data, int dataSize);
		int SendUDP(const unsigned char* buffer, int size, sockaddr_in* to);
		int ReceiveUDP(unsigned char* buffer, int size, sockaddr_in* from);
		void setTimeout(int miliseconds);

	private:
#ifdef _MSC_VER
		WSADATA wsda;
#endif
		int MAXCON;

		fd_set  *scks;
		timeval *times;

		unsigned int totaldata;
		bool check();
	};
}

#endif
