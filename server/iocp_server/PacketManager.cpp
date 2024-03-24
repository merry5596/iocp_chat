#include "PacketManager.h"

PacketManager::~PacketManager() {
	delete userManager;
}

void PacketManager::Init(const UINT16 clientPoolSize) {
	userManager = new UserManager;
	userManager->Init(clientPoolSize);

	roomManager = new RoomManager;
	roomManager->Init();

	processFuncDic = unordered_map<UINT16, ProcessFunction>();
	processFuncDic[(UINT16)PACKET_ID::ECHO_REQUEST] = &PacketManager::ProcessEchoRequest;
	processFuncDic[(UINT16)PACKET_ID::LOGIN_REQUEST] = &PacketManager::ProcessLoginRequest;
	processFuncDic[(UINT16)PACKET_ID::ROOM_ENTER_REQUEST] = &PacketManager::ProcessRoomEnterRequest;
	processFuncDic[(UINT16)PACKET_ID::ROOM_LEAVE_REQUEST] = &PacketManager::ProcessRoomLeaveRequest;
	processFuncDic[(UINT16)PACKET_ID::CHAT_REQUEST] = &PacketManager::ProcessChatRequest;
	processFuncDic[(UINT16)PACKET_ID::DISCONNECT] = &PacketManager::ProcessDisconnect;
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
	//printf("pktInfo.packetID: %d", pktInfo.packetID);
	auto iter = processFuncDic.find(pktInfo.packetID);
	if (iter != processFuncDic.end())
	{
		(this->*(iter->second))(pktInfo.clientIndex, pktInfo.packetData, pktInfo.packetSize);
	}
}

void PacketManager::ProcessEchoRequest(UINT32 clientIndex, char* data, UINT16 size) {
	auto reqPkt = reinterpret_cast<EchoRequestPacket*>(data);
	EchoResponsePacket resPkt;
	resPkt.packetID = (UINT16)PACKET_ID::ECHO_RESPONSE;
	cout <<"[ECHO RES]" << endl;
	SendData(clientIndex, (char*)&resPkt, size);
}


void PacketManager::ProcessLoginRequest(UINT32 clientIndex, char* data, UINT16 size) {
	auto reqPkt = reinterpret_cast<LoginRequestPacket*>(data);

	LoginResponsePacket resPkt;
	resPkt.packetID = (UINT16)PACKET_ID::LOGIN_RESPONSE;
	resPkt.packetSize = sizeof(LoginResponsePacket);
	//닉네임 중복 검사
	if (!userManager->SetLogin(reqPkt->name, clientIndex)) {	//실패
		resPkt.result = ERROR_CODE::ALREADY_EXIST_NAME;
	}
	else {	//문제없음
		resPkt.result = ERROR_CODE::NONE;
		strcpy_s(resPkt.name, strlen(reqPkt->name) + 1, reqPkt->name);
	}

	cout << "[LOGIN RES]" << endl;
	SendData(clientIndex, (char*)&resPkt, sizeof(LoginResponsePacket));
}

void PacketManager::ProcessRoomEnterRequest(UINT32 clientIndex, char* data, UINT16 size) {
	auto reqPkt = reinterpret_cast<RoomEnterRequestPacket*>(data);

	RoomEnterResponsePacket resPkt;
	resPkt.packetID = (UINT16)PACKET_ID::ROOM_ENTER_RESPONSE;
	resPkt.packetSize = sizeof(RoomEnterResponsePacket);

	//방 입장 가능한 상태?
	if (userManager->GetUserState(clientIndex) != (UINT16)USER_STATE::LOGIN) {
		resPkt.result = ERROR_CODE::USER_STATE_ERROR;
	}
	//랜덤
	else if (reqPkt->roomNum == 0) {
		UINT16 roomNum = roomManager->EnterRandomRoom(clientIndex);
		if (roomNum == 0) {
			resPkt.result = ERROR_CODE::NO_EMPTY_ROOM;
		}
		else {
			userManager->EnterRoom(clientIndex, roomNum);

			resPkt.roomNum = roomNum;
			resPkt.result = ERROR_CODE::NONE;
		}
	}
	//해당 방이 있는지
	else if (roomManager->GetRoom(reqPkt->roomNum) == nullptr) {
		resPkt.result = ERROR_CODE::INVALID_ROOM_NUM;
	}
	//방이 꽉 차지 않았는지
	else if (roomManager->IsFull(reqPkt->roomNum)) {
		resPkt.result = ERROR_CODE::ROOM_FULL;
	}
	else {	//문제없음
		roomManager->EnterRoom(reqPkt->roomNum, clientIndex);
		userManager->EnterRoom(clientIndex, reqPkt->roomNum);

		resPkt.roomNum = reqPkt->roomNum;
		resPkt.result = ERROR_CODE::NONE;
	}

	//Notify
	RoomEnterNotifyPacket ntfPkt;
	ntfPkt.packetID = (UINT16)PACKET_ID::ROOM_ENTER_NOTIFY;
	ntfPkt.packetSize = sizeof(RoomEnterNotifyPacket);
	strcpy_s(ntfPkt.name, NAME_LEN, userManager->GetUser(clientIndex)->GetName());

	unordered_set<UINT32> allUsers = roomManager->GetAllUserIndex(resPkt.roomNum);
	for (auto receiverIndex : allUsers) {
		if (receiverIndex != clientIndex) {
			cout << "[ROOM ENTER NTF]" << endl;
			SendData(receiverIndex, (char*)&ntfPkt, sizeof(RoomEnterNotifyPacket));
		}
	}

	cout << "[ROOM ENTER RES]" << endl;
	SendData(clientIndex, (char*)&resPkt, sizeof(RoomEnterResponsePacket));
}


void PacketManager::ProcessRoomLeaveRequest(UINT32 clientIndex, char* data, UINT16 size) {
	auto reqPkt = reinterpret_cast<RoomLeaveRequestPacket*>(data);

	RoomLeaveResponsePacket resPkt;
	resPkt.packetID = (UINT16)PACKET_ID::ROOM_LEAVE_RESPONSE;
	resPkt.packetSize = sizeof(RoomLeaveResponsePacket);

	//방 입장 가능한 상태?
	if (userManager->GetUserState(clientIndex) != (UINT16)USER_STATE::ROOM) {
		resPkt.result = ERROR_CODE::USER_STATE_ERROR;
	}

	UINT16 roomNum = userManager->LeaveRoom(clientIndex);
	roomManager->LeaveRoom(roomNum, clientIndex);
	resPkt.result = ERROR_CODE::NONE;

	//Notify
	RoomLeaveNotifyPacket ntfPkt;
	ntfPkt.packetID = (UINT16)PACKET_ID::ROOM_LEAVE_NOTIFY;
	ntfPkt.packetSize = sizeof(RoomLeaveNotifyPacket);
	strcpy_s(ntfPkt.name, NAME_LEN, userManager->GetUser(clientIndex)->GetName());


	unordered_set<UINT32> allUsers = roomManager->GetAllUserIndex(roomNum);
	for (auto receiverIndex : allUsers) {
		if (receiverIndex != clientIndex) {
			cout << "[ROOM LEAVE NTF]" << endl;
			SendData(receiverIndex, (char*)&ntfPkt, sizeof(RoomLeaveNotifyPacket));
		}
	}

	cout << "[ROOM LEAVE RES]" << endl;
	SendData(clientIndex, (char*)&resPkt, sizeof(RoomLeaveResponsePacket));
}


void PacketManager::ProcessChatRequest(UINT32 clientIndex, char* data, UINT16 size) {
	auto reqPkt = reinterpret_cast<ChatRequestPacket*>(data);

	//Notify
	ChatNotifyPacket ntfPkt;
	ntfPkt.packetID = (UINT16)PACKET_ID::CHAT_NOTIFY;
	ntfPkt.packetSize = sizeof(ChatNotifyPacket);
	strcpy_s(ntfPkt.name, NAME_LEN, userManager->GetUser(clientIndex)->GetName());
	strcpy_s(ntfPkt.msg, CHAT_MSG_LEN, reqPkt->msg);

	UINT16 roomNum = userManager->GetRoomNum(clientIndex);
	unordered_set<UINT32> allUsers = roomManager->GetAllUserIndex(roomNum);
	for (auto receiverIndex : allUsers) {
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

void PacketManager::ProcessDisconnect(UINT32 clientIndex, char* data, UINT16 size) {
	if (userManager->GetUserState(clientIndex) == (UINT16)USER_STATE::ROOM) {	//방에 있으면 방 나가기 처리 먼저
		auto roomNum = userManager->LeaveRoom(clientIndex);
		roomManager->LeaveRoom(roomNum, clientIndex);
	}
	userManager->SetLogout(clientIndex);
}