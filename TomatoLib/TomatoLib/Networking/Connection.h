#pragma once
#ifndef __NWMAN_H__
#define __NWMAN_H__

#include "EzSock.h"
#include "../Utilities/List.h"
#include "../Utilities/Dictonary.h"

#include <string>
#include <mutex>
#include <functional>
#include <thread>

#include "../Config.h"

#ifdef TL_ENABLE_SSL
#include <openssl/x509.h>
#endif

namespace TomatoLib {
	class Packet;
	class Connection;

	typedef bool(*IncommingPacketCallback)(void* Tag, Packet& p);

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
		IncommingPacketCallback Callback;

		List<unsigned char*> ToSend;
		List<unsigned char*> ToRecv;

		std::thread* SendThread;
		std::thread* RecvThread;

		std::mutex SendMutex;
		std::mutex RecvMutex;

		void RecvThreadFunc();
		void SendThreadFunc();

		bool HitTheBrakes;
		void* m_pTag;

	public:
		EzSock Sock;

#ifdef TL_ENABLE_SSL
		SSL_CTX* m_pSSLCtx;
		SSL*     m_pSSL;

		std::function<bool(Connection& con, X509& cert)> VerifySSLCertificateCallback;
#endif

		time_t LastReceivedPacket;
		ConnectionPacketIDType PIDType;
		ConnectionPacketDataLengthType DataLengthType;
		bool m_bBufferPackets; // false makes packet callbacks run on TL::Async::main thread

		Connection();
		~Connection();

		void StartThreads();

		bool IsConnected();
		bool Connect(std::string ip, int port, bool usessl = false);
		void ConnectThreaded(std::string ip, int port, std::function<void(bool)> callback);
		void Update();
		void Disconnect();

		void SetCallback(int pid, IncommingPacketCallback func);

		void SetTag(void* ptr);
		void SendPacket(Packet* p) { this->SendPacket(*p); }
		void SendPacket(Packet& p);
	};
}

#endif
