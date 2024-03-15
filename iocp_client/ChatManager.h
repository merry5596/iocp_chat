#pragma once
#include "Define.h"
#include "ErrorCode.h"
#include "Packet.h"
#include "PacketBufferManager.h"
#include "ClientNetwork.h"
#include "UserInfo.h"

//채팅 앱의 기능 관련 컨트롤러
class ChatManager : public ClientNetwork {
private:
	UserInfo userInfo;
	unique_ptr<PacketBufferManager> packetBufferManager;

public:
	bool Init(UINT16 SERVER_PORT, const char* SERVER_IP) {
		packetBufferManager = make_unique<PacketBufferManager>();
		packetBufferManager->Init();
		return ClientNetwork::Init(SERVER_PORT, SERVER_IP);
	}

	void Start() {
		packetBufferManager->Start();
		ClientNetwork::Start();
	}

	void End() {
		packetBufferManager->End();
		ClientNetwork::End();
	}

	void OnReceive(char* data, UINT16 size) {
		//printf("[RECV] size: %d\n", size);
		packetBufferManager->OnDataReceive(data, size);
	}

	bool Login(char* name) {
		LoginRequestPacket loginPkt;
		loginPkt.packetID = (UINT16)PACKET_ID::LOGIN_REQUEST;
		loginPkt.packetSize = sizeof(LoginRequestPacket);
		strcpy_s(loginPkt.name, NAME_LEN, name);
		if (SendData((char*)&loginPkt, sizeof(LoginRequestPacket)) == false) {
			return false;
		}
	
		auto result = packetBufferManager->GetLoginResponse();	//Login Response가 오면 그 결과를 반환한다.
		if (result == ERROR_CODE::ALREADY_EXIST_NAME) {	//TODO error code 클래스
			cout << "이미 존재하는 닉네임입니다." << endl;
			return false;
		}

		userInfo.Login(name);
		cout << "로그인 성공" << endl;

		return true;
	}

	bool EchoMsg(string msg) {
		EchoPacket echoPkt;
		echoPkt.packetID = (UINT16)PACKET_ID::ECHO;
		echoPkt.packetSize = sizeof(EchoPacket);
		CopyMemory(echoPkt.msg, msg.c_str(), sizeof(msg));
		return SendData((char*)&echoPkt, sizeof(EchoPacket));
	}

};