
#include "PacketManager.h"
#include "UserManager.h"

void PacketManager::Init(const UINT16 CLIENTPOOL_SIZE) {
	userManager = new UserManager;
	cout << CLIENTPOOL_SIZE << endl;
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
	while (isPacketRun) {
		UINT32 clientIndex = DequeueClient();
		if (clientIndex != -1) {
			PacketInfo pktInfo = userManager->GetPacket(clientIndex);
			if (pktInfo.packetID != 0) {
			//	cout << "I got packet" << endl;
				ProcessPacket(pktInfo);
			}
		}
		else {
			this_thread::sleep_for(chrono::milliseconds(1));
		}
	}
}

void PacketManager::OnDataReceive(UINT32 clientIndex, char* data, UINT16 size) {
	userManager->SetPacket(clientIndex, data, size);
	EnqueueClient(clientIndex);
}


void PacketManager::EnqueueClient(UINT32 clientIndex) {
	clientQueue.push(clientIndex);
}

UINT32  PacketManager::DequeueClient() {
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
	printf("[ECHO]packet id: %d, packet size: %d, msg: %s\n", echoPkt->packetID, echoPkt->packetSize, echoPkt->msg);
	SendDataFunc(pktInfo.clientIndex, (char*)echoPkt, echoPkt->packetSize);
}