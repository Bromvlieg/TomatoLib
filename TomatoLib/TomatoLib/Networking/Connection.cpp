#include "Connection.h"
#include "EzSock.h"
#include "Packet.h"
#include "../Defines.h"
#include "../Graphics/Color.h"
#include "../Utilities/Utilities.h"

#include <thread>
#include <math.h>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <cstring>

#include <chrono>
#include <thread>
#include <GLFW/glfw3.h>

namespace TomatoLib {
	Connection::Connection() {
		this->LastReceivedPacket = time(nullptr);
		this->PIDType = ConnectionPacketIDType::Byte;
		this->DataLengthType = ConnectionPacketDataLengthType::Int;
		this->HitTheBrakes = false;

		this->CallbacksSize = 256;
		this->Callbacks = new IncommingPacketCallback[this->CallbacksSize];

		this->SendThread = nullptr;
		this->RecvThread = nullptr;
	}

	Connection::~Connection() {
		this->HitTheBrakes = true;
		this->Disconnect();

		delete[] this->Callbacks;
	}

	void Connection::SetCallback(int pid, IncommingPacketCallback func) {
		if (this->CallbacksSize <= pid) {
			int olds = this->CallbacksSize;
			
			// we don't want to end up resizing every packet add
			while (this->CallbacksSize <= pid) {
				this->CallbacksSize += 256;
			}

			IncommingPacketCallback* newptr = new IncommingPacketCallback[this->CallbacksSize];
			memcpy(newptr, this->Callbacks, olds * sizeof(IncommingPacketCallback));
			memset(newptr + olds * sizeof(IncommingPacketCallback), 0, (this->CallbacksSize - olds) * sizeof(IncommingPacketCallback));

			delete[] this->Callbacks;
			this->Callbacks = newptr;
		}

		this->Callbacks[pid] = func;
	}

	bool Connection::IsConnected() {
		return this->Sock.state == EzSock::SockState::skCONNECTED;
	}
	
	void Connection::ConnectThreaded(std::string ip, int port, std::function<void(bool)> callback) {
		new std::thread([this, ip, port, callback]() {
			callback(this->Connect(ip, port));
		});
	}

	bool Connection::Connect(std::string ip, int port) {
		this->Disconnect();

		this->Sock.create();

		if (this->Sock.connect(ip.c_str(), port) > 0) return false;

		this->StartThreads();
		this->LastReceivedPacket = time(nullptr);
		return true;
	}

	void Connection::Disconnect() {
		if (this->SendThread == nullptr) return;

		this->HitTheBrakes = true;

		this->Sock.close();

		this->SendThread->join();
		this->RecvThread->join();

		delete this->SendThread;
		delete this->RecvThread;

		this->SendThread = nullptr;
		this->RecvThread = nullptr;

		this->HitTheBrakes = false;
	}

	void Connection::StartThreads() {
		if (this->SendThread != nullptr) return;

		this->SendThread = new std::thread([this] { this->SendThreadFunc(); });
		this->RecvThread = new std::thread([this] { this->RecvThreadFunc(); });
	}

	void Connection::SendPacket(Packet& p) {
		byte* buff = new byte[p.OutPos + 4];

		memcpy(buff, &p.OutPos, 4);
		memcpy(buff + 4, p.OutBuffer, p.OutPos);

		this->SendMutex.lock();
		this->ToSend.Add(buff);
		this->SendMutex.unlock();

		delete[] p.OutBuffer;
		p.OutBuffer = nullptr;
		p.OutPos = 0;
		p.OutSize = 0;
	}

	void Connection::SetTag(void* tag) {
		this->m_pTag = tag;
	}

	void Connection::Update() {
		while (true) {
			this->RecvMutex.lock();
			if (this->ToRecv.Count == 0) {
				this->RecvMutex.unlock();
				break;
			}

			byte* buff = this->ToRecv.RemoveAt(0);
			this->RecvMutex.unlock();

			Packet p;
			p.InBuffer = buff + 4;
			p.InSize = *(int*)buff;

			int pid;
			switch (this->PIDType) {
				case ConnectionPacketIDType::Byte: pid = p.ReadByte(); break;
				case ConnectionPacketIDType::Short: pid = p.ReadShort(); break;
				case ConnectionPacketIDType::Int: pid = p.ReadInt(); break;
			}

			if (pid < this->CallbacksSize && pid > 0 && this->Callbacks[pid] != nullptr) {
				bool ret = this->Callbacks[pid](this->m_pTag, p);
				if (!ret) {
					this->Disconnect();
					this->RecvMutex.lock();
					while (this->ToRecv.Count > 0) delete this->ToRecv.RemoveAt(this->ToRecv.Count - 1);
					this->RecvMutex.unlock();
				}

				if (p.InPos != p.InSize) {
					Utilities::Print("[col=%s]WARNING: Packet 0x%.4X(%d) was only read till %d, while the size is %d", Color::Orange.ToString().c_str(), pid, pid, p.InPos, p.InSize);
				}
			} else {
				Utilities::Print("[col=%s]Unknown packet: 0x%.4X(%d), size %d", Color::Red.ToString().c_str(), pid,  pid, p.InSize);
			}

			this->LastReceivedPacket = time(nullptr);

			p.InBuffer = nullptr;
			p.InSize = 0;
			p.Clear();

			delete[] buff;
		}
	}

	void Connection::SendThreadFunc() {
		while (!this->HitTheBrakes) {
			this->SendMutex.lock();
			if (this->ToSend.Count == 0) {
				this->SendMutex.unlock();

				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				continue;
			}

			byte* buff = this->ToSend.RemoveAt(0);
			this->SendMutex.unlock();

			if (!this->Sock.IsError()) {
				int lenoffset = this->DataLengthType != ConnectionPacketDataLengthType::Int ? 4 - (int)this->DataLengthType : 0;
				this->Sock.SendRaw(buff + 4 + lenoffset, *(int*)buff - lenoffset);
			}
		}
	}

	void Connection::RecvThreadFunc() {
		while (!this->HitTheBrakes) {
			if (!this->IsConnected()) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				continue;
			}

			int lensize = this->DataLengthType != ConnectionPacketDataLengthType::Int ? (int)this->DataLengthType : 4;

			byte plenbytes[4];
			int recamount = 0;
			while (recamount != lensize) {
				int currec = this->Sock.Receive(plenbytes, lensize - recamount, recamount);

				if (currec == -1 || currec == 0) {
					break;
				}

				recamount += currec;
			}

			if (recamount != lensize) continue;

			int psize = 0;
			switch (this->DataLengthType) {
				case ConnectionPacketDataLengthType::Byte: psize = plenbytes[0]; break;
				case ConnectionPacketDataLengthType::Short: psize = *(short*)plenbytes; break;
				case ConnectionPacketDataLengthType::Int: psize = *(int*)plenbytes; break;
			}

			byte* buffer = new byte[psize + 4];
			memcpy(buffer, &psize, 4);

			recamount = 0;
			while (recamount != psize) {
				int currec = this->Sock.Receive(buffer + 4, psize - recamount, recamount);

				if (currec == -1 || currec == 0) {
					break;
				}

				recamount += currec;
			}
			if (recamount != psize) {
				delete[] buffer;
				continue;
			}

			this->RecvMutex.lock();
			this->ToRecv.Add(buffer);
			this->RecvMutex.unlock();

			this->LastReceivedPacket = time(nullptr);
		}
	}
}
