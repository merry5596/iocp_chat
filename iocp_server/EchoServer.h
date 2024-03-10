#pragma once

#include "IOCPServer.h"
#include "PacketManager.h"
#include "UserManager.h"

class EchoServer : public IOCPServer {
private:
	unique_ptr<PacketManager> packetManager;	
public:
	void Init(const UINT16 SERVER_PORT, const UINT16 CLIENTPOOL_SIZE) {
		packetManager = std::make_unique<PacketManager>();
		packetManager->Init(CLIENTPOOL_SIZE);
		packetManager->SendDataFunc = [&](UINT32 clientIndex, char* data, UINT16 size) { SendData(clientIndex, data, size); };
		IOCPInit(SERVER_PORT, CLIENTPOOL_SIZE);
	}

	void Start() {
		packetManager->Start();
		IOCPStart();
	}

	void End() {
		packetManager->End();
		IOCPEnd();
	}
	virtual void OnConnect(UINT32 clientIndex) {
		printf("[ACCEPT]client index: %d\n", clientIndex);
	}

	virtual void OnReceive(UINT32 clientIndex, char* data, UINT16 size) {
		printf("[RECV]client index: %d, data size: %d\n", clientIndex, size);
		packetManager->OnDataReceive(clientIndex, data, size);
	}

	virtual void OnSend(UINT32 clientIndex, UINT16 size) {
		printf("[SEND]client index: %d, data size: %d\n", clientIndex, size);
	}

	virtual void OnDisconnect(UINT32 clientIndex) {
		printf("[CLOSE]client index:%d\n", clientIndex);
	}
};