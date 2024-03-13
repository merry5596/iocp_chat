
#include "PacketManager.h"
#include "UserManager.h"

PacketManager::PacketManager() {}
PacketManager::~PacketManager() {
	delete userManager;
}

void PacketManager::Init(const UINT16 CLIENTPOOL_SIZE) {
	userManager = new UserManager;
	userManager->Init(CLIENTPOOL_SIZE);
}

void PacketManager::Start() {
	isPacketRun = true;
	packetThread = thread([&]() { PacketThread();  });
}

void PacketManager::End() {
	isPacketRun = false;
	if (packetThread.joinable()) {
		packetThread.join();
	}
}

void PacketManager::PacketThread() {
	bool isIdle;
	while (isPacketRun) {
		isIdle = true;
		UINT32 clientIndex = DequeueClient();
		if (clientIndex != -1) {
			PacketInfo pktInfo = userManager->GetPacket(clientIndex);
			if (pktInfo.packetID != 0) {
				ProcessPacket(pktInfo);
				isIdle = false;
			}
		}
		if (isIdle) {
			this_thread::sleep_for(chrono::milliseconds(1));
		}
	}
}

//멀티스레드에서 접근 (clientQueue, user의 packetBuffer에 락 걸기)
void PacketManager::OnDataReceive(UINT32 clientIndex, char* data, UINT16 size) {
	userManager->SetPacket(clientIndex, data, size);
	EnqueueClient(clientIndex);
}


void PacketManager::EnqueueClient(UINT32 clientIndex) {
	lock_guard<mutex> lock(mtx);
	clientQueue.push(clientIndex);
}

UINT32  PacketManager::DequeueClient() {
	lock_guard<mutex> lock(mtx);
	if (clientQueue.empty()) {
		return -1;
	}
	UINT32 ret = clientQueue.front();
	clientQueue.pop();

	return ret;
}

void PacketManager::ProcessPacket(PacketInfo pktInfo) {
	//현재는 에코만 구현
	auto echoPkt = reinterpret_cast<EchoPacket*>(pktInfo.packetData);
	SendData(pktInfo.clientIndex, (char*)echoPkt, echoPkt->packetSize);
}