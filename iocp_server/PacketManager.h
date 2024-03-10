#pragma once

#include "define.h"
#include "packet.h"
#include "UserManager.h"

#include <thread>
#include <queue>
#include <functional>

class UserManager;
class PacketManager {
private:
	thread packetThread;
	bool isPacketRun;

	queue<UINT32> clientQueue;
	UserManager* userManager;

	void PacketThread();
public:
	PacketManager() {
	}
	~PacketManager() {
	}
	void Init(const UINT16 CLIENTPOOL_SIZE);
	void Start();
	void End();
	void OnDataReceive(UINT32 clientIndex, char* data, UINT16 size);
	void EnqueueClient(UINT32 clientIndex);
	UINT32 DequeueClient();
	void ProcessPacket(PacketInfo pktInfo);
	function<void(UINT32, char*, UINT16)> SendDataFunc;
};

