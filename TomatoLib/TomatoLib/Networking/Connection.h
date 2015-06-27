#pragma once
#ifndef __NWMAN_H__
#define __NWMAN_H__

#include "../Utilities/List.h"
#include "../Utilities/Dictonary.h"
#include "Packet.h"

#include <string>
#include <mutex>
#include <functional>

namespace TomatoLib {
	typedef void(*IncommingPacketCallback)(Packet& p);

	class Connection {
		static void _ReceiveThread(Connection* obj);
		bool CloseThread;
		bool ThreadRunning;

	public:
		bool Connected;

		double LastReceivedPacket;

		Connection();
		~Connection();

		EzSock Sock;
		std::mutex LockObject;
		List<byte*> ReceivedPackets;

		bool IsConnected();
		bool Connect(std::string ip, int port);
		void ConnectThreaded(std::string ip, int port, std::function<void(bool)> callback);
		void Login(std::string user, std::string pass, std::string pic);
		void Update();
		void RefreshReceiveIV();
		void RefreshSendIV();
		void Disconnect();

		void SetCallback(byte pid, IncommingPacketCallback func);

		Dictonary<int, IncommingPacketCallback> Callbacks;
	};
}

#endif
