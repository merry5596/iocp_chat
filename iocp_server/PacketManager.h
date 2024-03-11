#pragma once

#include "define.h"
#include "packet.h"
#include "UserManager.h"

#include <thread>
#include <queue>
#include <mutex>
#include <functional>

class UserManager;
class PacketManager {
private:
	thread packetThread;
	bool isPacketRun;

	queue<UINT32> clientQueue;
	mutex mtx;
	UserManager* userManager;
public:
	PacketManager();
	~PacketManager();
	void Init(const UINT16 CLIENTPOOL_SIZE);
	void Start();
	void End();
	void OnDataReceive(UINT32 clientIndex, char* data, UINT16 size);	//멀티스레드가 접근하는 함수
	function<void(UINT32, char*, UINT16)> SendData;
private:
	void PacketThread();
	void EnqueueClient(UINT32 clientIndex);
	UINT32 DequeueClient();
	void ProcessPacket(PacketInfo pktInfo);
};

