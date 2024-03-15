#pragma once
#include "Define.h"
#include "Packet.h"
#include "PacketBufferManager.h"
#include "ClientNetwork.h"
#include "UserInfo.h"

class ChatManager : public ClientNetwork {
private:
	unique_ptr<PacketBufferManager> packetBufferManager;
	bool loginRequestCompleted;
	UserInfo userInfo;
public:
	void Init(UINT16 SERVER_PORT, const char* SERVER_IP) {
		packetBufferManager = make_unique<PacketBufferManager>();
		packetBufferManager->Init();
		ClientNetwork::Init(SERVER_PORT, SERVER_IP);
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
		printf("[RECV] size: %d", size);
		packetBufferManager->OnDataReceive(data, size);
	}

	bool Login(char* name) {
		LoginRequestPacket loginPkt;
		loginPkt.packetID = (UINT16)PACKET_ID::LOGIN_REQUEST;
		loginPkt.packetSize = sizeof(LoginRequestPacket);
		strcpy_s(loginPkt.name, NAME_LEN, name);
		bool ret = SendData((char*)&loginPkt, sizeof(LoginRequestPacket));
		if (ret == false) {
			cout << "로그인 요청 실패\n" << endl;
			return false;
		}
	
		auto result = packetBufferManager->GetLoginResponse();	//Login Response가 오면 그 결과를 반환한다.
		if (result != 0) {	//TODO error code 클래스
			cout << "로그인 실패. 에러코드: " << result << endl;
			return false;
		}

		userInfo.SetName(name);
		//	userInfo.SetStatus(LOGIN);
		cout << "로그인 성공." << endl;
		return true;
	}

	bool EchoMsg(string msg) {
		EchoPacket echoPkt;
		echoPkt.packetID = (UINT16)PACKET_ID::ECHO_REQUEST;
		echoPkt.packetSize = sizeof(EchoPacket);
		CopyMemory(echoPkt.msg, msg.c_str(), sizeof(msg));
		return SendData((char*)&echoPkt, sizeof(EchoPacket));
	}

};