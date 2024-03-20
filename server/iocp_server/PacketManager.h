#pragma once

#include "../ServerNetLib/Define.h"
#include "Packet.h"
#include "UserManager.h"

#include <thread>
#include <queue>
#include <mutex>
#include <functional>
#include <unordered_map>
#include <iostream>
using namespace std;

class UserManager;
class PacketManager {
private:
	thread packetThread;
	bool isPacketRun;

	queue<UINT32> clientQueue;
	mutex mtx;
	UserManager* userManager;
	
	typedef void (PacketManager::*ProcessFunction)(UINT16, char*, UINT16);
	unordered_map<UINT16, ProcessFunction> processFuncDic;

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
	void ProcessConnect(UINT32 clientIndex);
	void ProcessDisconnect(UINT32 clientIndex);
	void ProcessEchoRequest(UINT16 clientIndex, char* data, UINT16 size);
	void ProcessLoginRequest(UINT16 clientIndex, char* data, UINT16 size);
	void ProcessChatRequest(UINT16 clientIndex, char* data, UINT16 size);
};

