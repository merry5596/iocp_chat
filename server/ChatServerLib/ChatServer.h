#pragma once

#include "IOCPNetwork.h"
#include "PacketManager.h"
#include "UserManager.h"

namespace ChatServerLib {

	class ChatServer : public ServerNetLib::IOCPNetwork {
	private:
		unique_ptr<PacketManager> packetManager;
		unique_ptr<ServerConfig> serverConfig;
	public:
		void Init();
		void Start();
		void End();
		virtual void OnConnect(UINT32 clientIndex);
		virtual void OnReceive(UINT32 clientIndex, char* data, UINT16 size);
		virtual void OnSend(UINT32 clientIndex, UINT16 size);
		virtual void OnDisconnect(UINT32 clientIndex);
	};

}
