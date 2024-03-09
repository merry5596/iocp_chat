#pragma once

#include "IOCPServer.h"

class EchoServer : public IOCPServer {
private:

public:
	virtual void OnReceive(UINT32 clientIndex, char* msg) {
		printf("[RECV]client index: %d, ", clientIndex);
		auto echoPkt = reinterpret_cast<EchoPacket*>(msg);
		printf("packet id: %d, packet size: %d, ", echoPkt->packetID, echoPkt->packetSize);
		printf("msg: %s\n", echoPkt->msg);
		GetClient(clientIndex)->SendMsg(echoPkt->msg);
	}

	virtual void OnSend(UINT32 clientIndex, char* msg) {
		printf("[SEND]client index: %d\n", clientIndex);
	}

	void Init(UINT16 SERVER_PORT) {
		IOCPInit(SERVER_PORT);
	}

	void Start(UINT16 CLIENTPOOL_SIZE) {
		IOCPStart(CLIENTPOOL_SIZE);
	}

	void End() {
		IOCPEnd();
	}
};