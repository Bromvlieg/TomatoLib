#include "Connection.h"
#include "EzSock.h"
#include "../Defines.h"
#include "../Graphics/Color.h"
#include "../Utilities/Utilities.h"

#include <thread>
#include <math.h>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

#include <chrono>
#include <thread>
#include <GLFW/glfw3.h>

namespace TomatoLib {
	Connection::Connection() {
		this->LastReceivedPacket = TL_GET_TIME_MS;
		this->Connected = false;
		this->PIDType = ConnectionPacketIDType::Byte;
	}

	Connection::~Connection() {
		this->Disconnect();
	}

	void Connection::SetCallback(byte pid, IncommingPacketCallback func) {
		this->Callbacks[pid] = func;
	}

	bool Connection::IsConnected() {
		return this->Sock.state == EzSock::SockState::skCONNECTED;
	}

	void Connection::_ReceiveThread(Connection* man) {
		man->ThreadRunning = true;

		while (!man->CloseThread) {
			if (!man->IsConnected() || man->ReceiveFunction == null) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				continue;
			}
			
			man->ReceiveFunction(man);
		}

		man->ThreadRunning = false;
	}
	
	void Connection::AddReceivedPacket(int len, byte* data) {
		byte* buffer = new byte[len + 4];
		memcpy(buffer, &len, 4);
		memcpy(buffer + 4, data, len);

		this->LockObject.lock();
		this->ReceivedPackets.Add(buffer);
		this->LockObject.unlock();
	}

	void Connection::ConnectThreaded(std::string ip, int port, std::function<void(bool)> callback) {
		new std::thread([this, ip, port, callback]() {
			bool ret = this->Connect(ip, port);
			callback(ret);
		});
	}

	bool Connection::Connect(std::string ip, int port) {
		if (this->ThreadRunning) {
			this->CloseThread = true;
			this->Sock.close();
			while (this->ThreadRunning) std::this_thread::sleep_for(std::chrono::milliseconds(1));
			this->CloseThread = false;
		}

		this->Sock = EzSock();
		this->Sock.create();

		if (this->Sock.connect(ip.c_str(), port) > 0) return false;

		this->LastReceivedPacket = TL_GET_TIME_MS;
		this->Connected = true;
		new std::thread(Connection::_ReceiveThread, this);
		return true;
	}

	void Connection::Disconnect() {
		this->CloseThread = true;
		this->Sock.close();
		while (this->ThreadRunning) std::this_thread::sleep_for(std::chrono::milliseconds(1));
		this->CloseThread = false;

		this->Connected = false;
	}

	void Connection::Update() {
		while (true) {
			byte* data = null;

			this->LockObject.lock();
			if (this->ReceivedPackets.Count > 0) {
				data = this->ReceivedPackets.RemoveAt(0);
			}
			this->LockObject.unlock();

			if (data == null)
				break;

			Packet p(&this->Sock);
			p.InBuffer = data + 4;
			p.InSize = *(int*)data;

			int pid;
			switch (this->PIDType) {
				case ConnectionPacketIDType::Byte: pid = p.ReadByte(); break;
				case ConnectionPacketIDType::Short: pid = p.ReadShort(); break;
				case ConnectionPacketIDType::Int: pid = p.ReadInt(); break;
			}

			if (this->Callbacks.ContainsKey(pid)) {
				this->Callbacks[pid](p);
				if (p.InPos != p.InSize) {
					Utilities::Print("[col=%s]WARNING: Packet 0x%.2X(%d) was only read till %d, while the size is %d", Color::Orange.ToString().c_str(), pid, pid, p.InPos, p.InSize);
				}
			} else {
				Utilities::Print("[col=%s]Unknown packet: 0x%.2X(%d), size %d", Color::Red.ToString().c_str(), pid,  pid, p.InSize);
			}

			this->LastReceivedPacket = TL_GET_TIME_MS;

			p.InBuffer = null;
			p.InSize = 0;
			p.Clear();

			delete[] data;
		}
	}
}
