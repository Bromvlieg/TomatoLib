#include "Connection.h"
#include "EzSock.h"
#include "../Defines.h"
#include "../Graphics/Color.h"

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
			if (!man->IsConnected()) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				continue;
			}

			byte plenbytes[4];
			int recamount = 0;
			while (recamount != 4) {
				int currec = man->Sock.Receive(plenbytes, 4 - recamount, recamount);

				if (currec == -1 || currec == 0) {
					break;
				}

				recamount += currec;
			}
			if (recamount != 4) break;

			int psize = (plenbytes[0] + (plenbytes[1] << 8)) ^ (plenbytes[2] + (plenbytes[3] << 8));

			byte* buffer = new byte[psize + 4];
			memcpy(buffer, &psize, 4);

			recamount = 0;
			while (recamount != psize) {
				int currec = man->Sock.Receive(buffer, psize - recamount, recamount + 4);

				if (currec == -1 || currec == 0) {
					break;
				}

				recamount += currec;
			}
			if (recamount != psize) break;

			man->LockObject.lock();
			man->ReceivedPackets.Add(buffer);
			man->LockObject.unlock();
		}

		man->ThreadRunning = false;
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

			short pid = p.ReadShort();

			if (this->Callbacks.ContainsKey(pid)) {
				this->Callbacks[pid](p);
				if (p.InPos != p.InSize) {
					printf("[col=%s]WARNING: Packet '%.4X' was only read till %d, while the size is %d", Color::Orange.ToString().c_str(), pid, p.InPos, p.InSize);
				}
			} else {
				printf("[col=%s]Unknown packet: %.4X, size %d", Color::Red.ToString().c_str(), pid, p.InSize);
			}

			this->LastReceivedPacket = TL_GET_TIME_MS;

			p.InBuffer = null;
			p.InSize = 0;
			p.Clear();

			delete[] data;
		}
	}
}
