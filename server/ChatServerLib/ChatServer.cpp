#include "ChatServer.h"

namespace ChatServerLib {

	void ChatServer::Init() {
		config = std::make_unique<ChatConfig>();
		config->LoadConfig();

		packetManager = std::make_unique<PacketManager>();
		packetManager->Init(config.get());
		packetManager->SendData = [&](UINT32 clientIndex, char* data, UINT16 size) { SendData(clientIndex, data, size); };

		IOCPInit(config.get());
	}

	void ChatServer::Start() {
		packetManager->Start();
		IOCPStart();
		spdlog::info("Chat Server Start!");
		//printf("Chat Server Start! ");
	}

	void ChatServer::End() {
		packetManager->End();
		IOCPEnd();
		spdlog::info("Chat Server End...");
		//printf("Chat Server End...");
	}

	void ChatServer::OnConnect(UINT32 clientIndex) {
		spdlog::info("[ACCEPT]client index: {}", clientIndex);
		//printf("[ACCEPT]client index: %d\n", clientIndex);
	}

	void ChatServer::OnReceive(UINT32 clientIndex, char* data, UINT16 size) {
		spdlog::info("[RECV]client index: {0}, data size: {1}", clientIndex, size);
		//printf("[RECV]client index: %d, data size: %d\n", clientIndex, size);
		packetManager->OnDataReceive(clientIndex, data, size);
	}

	void ChatServer::OnSend(UINT32 clientIndex, UINT16 size) {
		spdlog::info("[SEND]client index: {0}, data size: {1}", clientIndex, size);
		//printf("[SEND]client index: %d, data size: %d\n", clientIndex, size);
	}

	void ChatServer::OnDisconnect(UINT32 clientIndex) {
		spdlog::info("[CLOSE]client index:{}\n", clientIndex);
		//printf("[CLOSE]client index:%d\n", clientIndex);
		DisconnectPacket pkt;
		pkt.packetID = (UINT16)PACKET_ID::DISCONNECT;
		pkt.packetSize = sizeof(DisconnectPacket);
		packetManager->OnDataReceive(clientIndex, (char*)&pkt, sizeof(DisconnectPacket));
	}

}