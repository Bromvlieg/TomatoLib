#include "Connection.h"
#include "EzSock.h"
#include "Packet.h"
#include "../Defines.h"
#include "../Graphics/Color.h"
#include "../Async/Async.h"
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

#ifdef TL_ENABLE_SSL
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#endif

namespace TomatoLib {
	Connection::Connection() {
		this->LastReceivedPacket = time(nullptr);
		this->PIDType = ConnectionPacketIDType::Byte;
		this->DataLengthType = ConnectionPacketDataLengthType::Int;
		this->HitTheBrakes = false;
		this->m_bBufferPackets = true;

		this->Callback = nullptr;
		this->CallbacksSize = 256;
		this->Callbacks = new IncommingPacketCallback[this->CallbacksSize];
		memset(this->Callbacks, 0, this->CallbacksSize * sizeof(IncommingPacketCallback));

		this->SendThread = nullptr;
		this->RecvThread = nullptr;

#ifdef TL_ENABLE_SSL
		this->m_pSSL = nullptr;
		this->m_pSSLCtx = nullptr;

		this->VerifySSLCertificateCallback = nullptr;
#endif
	}

	Connection::~Connection() {
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
			TomatoLib::Async::SetThreadName("TL:connection_connect");
			bool ret = this->Connect(ip, port);

			Async::RunOnMainThread([ret, callback]() {
				callback(ret);
			});
		});
	}

	bool Connection::Connect(std::string ip, int port, bool usessl) {
		this->Disconnect();

		this->Sock.create();

		if (this->Sock.connect(ip.c_str(), port) > 0) return false;

		if (usessl) {
#ifndef TL_ENABLE_SSL
			this->Sock.close();
			return false;
#else
			static bool firstload = true;
			if (firstload) {
				OpenSSL_add_ssl_algorithms();
				SSL_load_error_strings();
				firstload = false;
			}

			const SSL_METHOD* meth = DTLS_client_method();
			this->m_pSSLCtx = SSL_CTX_new(meth);

			this->m_pSSL = SSL_new(this->m_pSSLCtx);
			if (this->m_pSSL == nullptr) {
				this->Sock.close();

				SSL_free(this->m_pSSL);
				SSL_CTX_free(this->m_pSSLCtx);

				this->m_pSSL = nullptr;
				this->m_pSSLCtx = nullptr;

				return false;
			}

			SSL_set_fd(this->m_pSSL, this->Sock.sock);
			if (SSL_connect(this->m_pSSL) == -1) {
				this->Sock.close();

				SSL_free(this->m_pSSL);
				SSL_CTX_free(this->m_pSSLCtx);

				this->m_pSSL = nullptr;
				this->m_pSSLCtx = nullptr;

				return false;
			}

			X509* server_cert = SSL_get_peer_certificate(this->m_pSSL);
			if (server_cert == nullptr) {
				this->Sock.close();

				SSL_free(this->m_pSSL);
				SSL_CTX_free(this->m_pSSLCtx);

				this->m_pSSL = nullptr;
				this->m_pSSLCtx = nullptr;

				return false;
			}

			if (this->VerifySSLCertificateCallback != nullptr) {
				if (!this->VerifySSLCertificateCallback(*this, *server_cert)) {
					X509_free(server_cert);
					this->Sock.close();

					SSL_free(this->m_pSSL);
					SSL_CTX_free(this->m_pSSLCtx);

					this->m_pSSL = nullptr;
					this->m_pSSLCtx = nullptr;

					return false;
				}
			}

			X509_free(server_cert);
#endif
		}

		this->StartThreads();
		this->LastReceivedPacket = time(nullptr);
		return true;
	}

	void Connection::Disconnect() {
		if (this->SendThread == nullptr) return;

		this->HitTheBrakes = true;
		this->Sock.close();

		if (this->SendThread->joinable()) this->SendThread->join();
		if (this->RecvThread->joinable()) this->RecvThread->join();

		delete this->SendThread;
		delete this->RecvThread;

		this->SendThread = nullptr;
		this->RecvThread = nullptr;

		this->HitTheBrakes = false;

#ifdef TL_ENABLE_SSL
		if (this->m_pSSL != nullptr) {
			SSL_shutdown(this->m_pSSL);
			SSL_free(this->m_pSSL);
			this->m_pSSL = nullptr;
		}

		if (this->m_pSSLCtx != nullptr) {
			SSL_CTX_free(this->m_pSSLCtx);
			this->m_pSSLCtx = nullptr;
		}
#endif

		// backwards delete is faster due no need to move elements forward
		while (this->ToSend.Count > 0) delete[] this->ToSend.RemoveAt(this->ToSend.Count - 1);
		while (this->ToRecv.Count > 0) delete[] this->ToRecv.RemoveAt(this->ToRecv.Count - 1);
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

			//Utilities::Print("TL_CONNECTION: packet: 0x%.4X(%d), size %d", pid, pid, p.InSize);
			if (pid < this->CallbacksSize && pid >= 0 && this->Callbacks[pid] != nullptr) {
				bool ret = this->Callbacks[pid](this->m_pTag, p);
				if (!ret) {
					this->Disconnect();
					this->RecvMutex.lock();
					while (this->ToRecv.Count > 0) delete this->ToRecv.RemoveAt(this->ToRecv.Count - 1);
					this->RecvMutex.unlock();
				}

				if (p.InPos != p.InSize) {
					Utilities::Print("TL_CONNECTION: WARNING: Packet 0x%.4X(%d) was only read till %d, while the size is %d", pid, pid, p.InPos, p.InSize);
				}
			} else {
				Utilities::Print("TL_CONNECTION: Unknown packet: 0x%.4X(%d), size %d", pid,  pid, p.InSize);
			}

			this->LastReceivedPacket = time(nullptr);

			p.InBuffer = nullptr;
			p.InSize = 0;
			p.Clear();

			delete[] buff;
		}
	}

	void Connection::SendThreadFunc() {
		TomatoLib::Async::SetThreadName("TL:connection_writer");

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
				
#ifdef TL_ENABLE_SSL
				if (this->m_pSSL != nullptr) {
					SSL_write(this->m_pSSL, buff + lenoffset, *(int*)buff - lenoffset + 4);
				} else {
					this->Sock.SendRaw(buff + lenoffset, *(int*)buff - lenoffset + 4);
				}
#else
				this->Sock.SendRaw(buff + lenoffset, *(int*)buff - lenoffset + 4);
#endif
			}
		}
	}

	void Connection::RecvThreadFunc() {
		TomatoLib::Async::SetThreadName("TL:connection_reader");

		while (!this->HitTheBrakes) {
			if (!this->IsConnected()) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				continue;
			}

			int lensize = this->DataLengthType != ConnectionPacketDataLengthType::Int ? (int)this->DataLengthType : 4;

			byte plenbytes[4];
			int recamount = 0;
			while (recamount != lensize) {
#ifdef TL_ENABLE_SSL
				int currec = this->m_pSSL != nullptr ? SSL_read(this->m_pSSL, plenbytes + recamount, lensize - recamount) : this->Sock.Receive(plenbytes, lensize - recamount, recamount);
#else
				int currec = this->Sock.Receive(plenbytes, lensize - recamount, recamount);
#endif

				if (currec == -1 || currec == 0) {
					break;
				}

				recamount += currec;
			}

			if (recamount != lensize) {
				this->Sock.close();

#ifdef TL_ENABLE_SSL
				if (this->m_pSSL != nullptr) delete this->m_pSSL;
				if (this->m_pSSLCtx != nullptr) delete this->m_pSSLCtx;

				this->m_pSSL = nullptr;
				this->m_pSSLCtx = nullptr;
#endif
				continue;
			}

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
#ifdef TL_ENABLE_SSL
				int currec = this->m_pSSL != nullptr ? SSL_read(this->m_pSSL, buffer + 4 + recamount, psize - recamount) : this->Sock.Receive(buffer + 4, psize - recamount, recamount);
#else
				int currec = this->Sock.Receive(buffer + 4, psize - recamount, recamount);
#endif

				if (currec == -1 || currec == 0) {
					break;
				}

				recamount += currec;
			}

			if (recamount != psize) {
				delete[] buffer;
				this->Sock.close();

#ifdef TL_ENABLE_SSL
				if (this->m_pSSL != nullptr) delete this->m_pSSL;
				if (this->m_pSSLCtx != nullptr) delete this->m_pSSLCtx;

				this->m_pSSL = nullptr;
				this->m_pSSLCtx = nullptr;
#endif
				continue;
			}

			if (!this->m_bBufferPackets) {
				Packet p;
				p.InBuffer = buffer + 4;
				p.InSize = *(int*)buffer;

				int pid;
				switch (this->PIDType) {
					case ConnectionPacketIDType::Byte: pid = p.ReadByte(); break;
					case ConnectionPacketIDType::Short: pid = p.ReadShort(); break;
					case ConnectionPacketIDType::Int: pid = p.ReadInt(); break;
				}

				this->LastReceivedPacket = time(nullptr);

				if (this->Callback != nullptr) {
					bool ret = this->Callback(this->m_pTag, p);
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
					Utilities::Print("[col=%s]No packet callback for non-buffered: 0x%.4X(%d), size %d", Color::Red.ToString().c_str(), pid, pid, p.InSize);
				}

				p.InBuffer = nullptr;
				p.InSize = 0;
				p.Clear();

				delete[] buffer;
			} else {
				this->RecvMutex.lock();
				this->ToRecv.Add(buffer);
				this->RecvMutex.unlock();
			}

			this->LastReceivedPacket = time(nullptr);
		}
	}
}
