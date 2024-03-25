#include "ChatServer.h"

namespace ChatServerLib {

	void ChatServer::Init() {
		serverConfig = std::make_unique<ServerConfig>();
		serverConfig->LoadConfig();

		packetManager = std::make_unique<PacketManager>();
		packetManager->Init(serverConfig->CLIENT_POOL_SIZE);
		packetManager->SendData = [&](UINT32 clientIndex, char* data, UINT16 size) { SendData(clientIndex, data, size); };

		IOCPInit(serverConfig.get());
	}

	void ChatServer::Start() {
		packetManager->Start();
		IOCPStart();
	}

	void ChatServer::End() {
		packetManager->End();
		IOCPEnd();
		printf("Chat Server End...");
	}

	void ChatServer::OnConnect(UINT32 clientIndex) {
		printf("[ACCEPT]client index: %d\n", clientIndex);
	}

	void ChatServer::OnReceive(UINT32 clientIndex, char* data, UINT16 size) {
		printf("[RECV]client index: %d, data size: %d\n", clientIndex, size);
		packetManager->OnDataReceive(clientIndex, data, size);
	}

	void ChatServer::OnSend(UINT32 clientIndex, UINT16 size) {
		printf("[SEND]client index: %d, data size: %d\n", clientIndex, size);
	}

	void ChatServer::OnDisconnect(UINT32 clientIndex) {
		printf("[CLOSE]client index:%d\n", clientIndex);
		DisconnectPacket pkt;
		pkt.packetID = (UINT16)PACKET_ID::DISCONNECT;
		pkt.packetSize = sizeof(DisconnectPacket);
		packetManager->OnDataReceive(clientIndex, (char*)&pkt, sizeof(DisconnectPacket));
	}

}