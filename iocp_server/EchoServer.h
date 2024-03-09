#pragma once

#include "IOCPServer.h"

class EchoServer : public IOCPServer {
private:

public:
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