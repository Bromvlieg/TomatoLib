#include "Server.h"
#include "Connection.h"
#include "../Utilities/Utilities.h"
#include "../Utilities/HTTP.h"

namespace TomatoLib {
	Server::Server() {
		this->IP[0] = 127;
		this->IP[1] = 0;
		this->IP[2] = 0;
		this->IP[3] = 1;

		this->m_NewConCallback = nullptr;
		this->m_usPort = 0;
	}

	Server::~Server() {
		this->Shutdown();
	}

	void Server::Shutdown() {
		if (this->Sock.IsError()) return;

		this->Sock.close();

		this->AcceptorThread->join();
		delete this->AcceptorThread;
	}

	void Server::SetConnectionCallback(IncommingConnectionCallback callback) {
		this->m_NewConCallback = callback;
	}

	void Server::PollIP(PollCallback callback) {
		HTTP::Request("http://ipv4.icanhazip.com/", "GET", "", "text/plain", [this, callback](bool success, Dictonary<std::string, std::string>& headers, unsigned char* body, unsigned int bodysize) {
			if (body == nullptr || bodysize == 0) {
				callback(false);
				return;
			}

			body[bodysize - 1] = 0;
			std::string str = (char*)body;

			auto bytes = Utilities::Split(str, '.');
			if (bytes.Count != 4) {
				callback(false);
				return;
			}

			for (int i = 0; i < 4; i++) {
				this->IP[i] = atoi(bytes[i].c_str());
			}
			callback(true);
		});
	}

	bool Server::Listen(short port) {
		if (this->m_NewConCallback == nullptr) return false;

		if (!this->Sock.create() || !this->Sock.bind(port) || !this->Sock.listen()) {
			this->Sock.close();
			return false;
		}

		this->m_usPort = port;
		this->AcceptorThread = new std::thread([this] { this->ConAcceptorThreadFunc(); });
		return true;
	}

	void Server::ConAcceptorThreadFunc() {
		Connection* mcon = nullptr;

		while (!this->Sock.IsError()) {
			mcon = new Connection();

			bool ret = this->Sock.accept(&mcon->Sock);
			if (!ret) {
				delete mcon;
				continue;
			}

			bool handled = this->m_NewConCallback(*this, mcon);
			if (!handled) {
				delete mcon;
				continue;
			}

			mcon->StartThreads();
		}
	}
}