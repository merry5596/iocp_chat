
#include "PacketManager.h"
#include "UserManager.h"

PacketManager::PacketManager() {}
PacketManager::~PacketManager() {
	delete userManager;
}

void PacketManager::Init(const UINT16 CLIENTPOOL_SIZE) {
	userManager = new UserManager;
	userManager->Init(CLIENTPOOL_SIZE);

	processFuncDic = unordered_map<UINT16, ProcessFunction>();
	processFuncDic[(UINT16)PACKET_ID::ECHO_REQUEST] = &PacketManager::ProcessEchoRequest;
	processFuncDic[(UINT16)PACKET_ID::LOGIN_REQUEST] = &PacketManager::ProcessLoginRequest;
	processFuncDic[(UINT16)PACKET_ID::CHAT_REQUEST] = &PacketManager::ProcessChatRequest;
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
			if (pktInfo.packetID == (UINT16)PACKET_ID::DISCONNECT) {	//DISCONNECT
				userManager->DeleteUser(clientIndex);
			}
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
	printf("pktInfo.packetID: %d", pktInfo.packetID);
	auto iter = processFuncDic.find(pktInfo.packetID);
	if (iter != processFuncDic.end())
	{
		(this->*(iter->second))(pktInfo.clientIndex, pktInfo.packetData, pktInfo.packetSize);
	}
}

void PacketManager::ProcessEchoRequest(UINT16 clientIndex, char* data, UINT16 size) {
	printf("[ECHO]\n");

	auto echoPkt = reinterpret_cast<EchoPacket*>(data);
	SendData(clientIndex, (char*)echoPkt, size);
}


void PacketManager::ProcessLoginRequest(UINT16 clientIndex, char* data, UINT16 size) {
	printf("[LOGINREQUEST]\n");

	auto reqPkt = reinterpret_cast<LoginRequestPacket*>(data);

	ResponsePacket resPkt;
	resPkt.packetID = (UINT16)PACKET_ID::LOGIN_RESPONSE;
	resPkt.packetSize = sizeof(ResponsePacket);
	//닉네임 중복 검사
	if (!userManager->AddUser(reqPkt->name, clientIndex)) {	//add 실패
		resPkt.result = 1;	//error 코드 추후 작성
	}
	else {	//문제없음
		resPkt.result = 0;	//error 코드 추후에 작성
	}
	SendData(clientIndex, (char*)&resPkt, sizeof(ResponsePacket));
}

void PacketManager::ProcessChatRequest(UINT16 clientIndex, char* data, UINT16 size) {
}