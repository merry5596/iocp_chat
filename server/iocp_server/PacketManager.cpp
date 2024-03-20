#include "PacketManager.h"

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
	processFuncDic[(UINT16)PACKET_ID::ROOM_ENTER_REQUEST] = &PacketManager::ProcessRoomEnterRequest;
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
			else if (pktInfo.packetID != 0) {
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
	//printf("pktInfo.packetID: %d", pktInfo.packetID);
	auto iter = processFuncDic.find(pktInfo.packetID);
	if (iter != processFuncDic.end())
	{
		(this->*(iter->second))(pktInfo.clientIndex, pktInfo.packetData, pktInfo.packetSize);
	}
}

void PacketManager::ProcessEchoRequest(UINT16 clientIndex, char* data, UINT16 size) {
	auto reqPkt = reinterpret_cast<EchoRequestPacket*>(data);
	EchoResponsePacket resPkt;
	resPkt.packetID = (UINT16)PACKET_ID::ECHO_RESPONSE;
	cout <<"[ECHO RES]" << endl;
	SendData(clientIndex, (char*)&resPkt, size);
}


void PacketManager::ProcessLoginRequest(UINT16 clientIndex, char* data, UINT16 size) {
	auto reqPkt = reinterpret_cast<LoginRequestPacket*>(data);

	LoginResponsePacket resPkt;
	resPkt.packetID = (UINT16)PACKET_ID::LOGIN_RESPONSE;
	resPkt.packetSize = sizeof(LoginResponsePacket);
	//닉네임 중복 검사
	if (!userManager->AddUser(reqPkt->name, clientIndex)) {	//add 실패
		resPkt.result = ERROR_CODE::ALREADY_EXIST_NAME;
	}
	else {	//문제없음
		resPkt.result = ERROR_CODE::NONE;	//error 코드 추후에 작성
	}

	cout << "[LOGIN RES]" << endl;
	SendData(clientIndex, (char*)&resPkt, sizeof(LoginResponsePacket));
}

void PacketManager::ProcessRoomEnterRequest(UINT16 clientIndex, char* data, UINT16 size) {
	auto reqPkt = reinterpret_cast<RoomEnterRequestPacket*>(data);

	RoomEnterResponsePacket resPkt;
	resPkt.packetID = (UINT16)PACKET_ID::ROOM_ENTER_RESPONSE;
	resPkt.packetSize = sizeof(RoomEnterResponsePacket);

	//랜덤을 원한다면 (좀따구현)
	if (reqPkt->roomNum == 0) {
		//UINT16 roomNum = roomManager.EnterRandomRoom();
		//if (roomNum == 0) {
		//	resPkt.result == ERROR_CODE::NO_EMPTY_ROOM;
		//}
		//else {
		//	resPkt.roomNum = roomNum;
		//	resPkt.result == ERROR_CODE::NONE;
		//}
	}
	//해당 방이 있는지
	else if (false) {	
		resPkt.result = ERROR_CODE::INVALID_ROOM_NUM;
	}
	//방이 꽉 차지 않았는지
	else if (false) {
		resPkt.result = ERROR_CODE::ROOM_FULL;
	}
	else {	//문제없음
		userManager->EnterRoom(clientIndex, reqPkt->roomNum);
		//roomManager->EnterRoom(clientIndex);

		resPkt.roomNum = reqPkt->roomNum;
		resPkt.result = ERROR_CODE::NONE;
	}

	cout << "[ROOM ENTER RES]" << endl;
	SendData(clientIndex, (char*)&resPkt, sizeof(RoomEnterResponsePacket));
}


void PacketManager::ProcessChatRequest(UINT16 clientIndex, char* data, UINT16 size) {
	auto reqPkt = reinterpret_cast<ChatRequestPacket*>(data);

	ChatNotifyPacket ntfPkt;
	ntfPkt.packetID = (UINT16)PACKET_ID::CHAT_NOTIFY;
	ntfPkt.packetSize = sizeof(ChatNotifyPacket);
	CopyMemory(ntfPkt.sender, userManager->GetUser(clientIndex)->GetName(), NAME_LEN);
	CopyMemory(ntfPkt.msg, reqPkt->msg, CHAT_MSG_LEN);
	for (auto receiverIndex : userManager->GetAllUserIndex()) {
		if (receiverIndex != clientIndex) {
			cout << "[CHAT NTF]" << endl;
			SendData(receiverIndex, (char*)&ntfPkt, sizeof(ChatNotifyPacket));
		}
	}

	ChatResponsePacket resPkt;
	resPkt.packetID = (UINT16)PACKET_ID::CHAT_RESPONSE;
	resPkt.packetSize = sizeof(ChatResponsePacket);
	resPkt.result = ERROR_CODE::NONE;

	cout << "[CHAT RES]" << endl;
	SendData(clientIndex, (char*)&resPkt, sizeof(ChatResponsePacket));
}