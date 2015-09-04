#pragma once
#ifndef __NWMAN_H__
#define __NWMAN_H__

#include "EzSock.h"
#include "../Utilities/List.h"
#include "../Utilities/Dictonary.h"

#include <string>
#include <mutex>
#include <functional>

namespace std {
	class thread;
}

namespace TomatoLib {
	class Packet;

	typedef void(*IncommingPacketCallback)(Packet& p);

	enum class ConnectionPacketIDType {
		Byte, Short, Int
	};

	enum class ConnectionPacketDataLengthType {
		Byte, Short, Int,
	};

	class Connection {
		static void _ReceiveThread(Connection* obj);
		bool CloseThread;
		bool ThreadRunning;

		int CallbacksSize;
		IncommingPacketCallback* Callbacks;

		List<unsigned char*> ToSend;
		List<unsigned char*> ToRecv;

		std::thread* SendThread;
		std::thread* RecvThread;

		std::mutex SendMutex;
		std::mutex RecvMutex;

		EzSock Sock;

		void RecvThreadFunc();
		void SendThreadFunc();

		bool HitTheBrakes;

	public:
		float LastReceivedPacket;
		ConnectionPacketIDType PIDType;
		ConnectionPacketDataLengthType DataLengthType;

		Connection();
		~Connection();

		bool IsConnected();
		bool Connect(std::string ip, int port);
		void ConnectThreaded(std::string ip, int port, std::function<void(bool)> callback);
		void Update();
		void Disconnect();

		void SetCallback(int pid, IncommingPacketCallback func);

		void SendPacket(Packet* p) { this->SendPacket(*p); }
		void SendPacket(Packet& p);
	};
}

#endif
