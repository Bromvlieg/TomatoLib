#pragma once
#ifndef __TL_SERVER__
#define __TL_SERVER__

#include "EzSock.h"
#include "../Utilities/List.h"
#include "../Utilities/Dictonary.h"

#include <mutex>
#include <thread>

namespace TomatoLib {
	class Connection;

	class Server {
	public:
		typedef bool(*IncommingConnectionCallback)(Server& s, Connection* c);
		typedef void(*PollCallback)(bool worked);

		Server();
		~Server();

		void* Tag;

		void Shutdown();
		bool Listen(short port);

		unsigned char IP[4];
		void PollIP(PollCallback callback);
		unsigned short GetPort() const { return this->m_usPort; }

		void SetConnectionCallback(IncommingConnectionCallback callback);

	private:
		unsigned short m_usPort;

		EzSock Sock;
		IncommingConnectionCallback m_NewConCallback;

		std::mutex NewConMutex;
		std::thread* AcceptorThread;

		void ConAcceptorThreadFunc();
	};
}

#endif
