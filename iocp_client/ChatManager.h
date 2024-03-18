#pragma once
#include "Define.h"
#include "../common/ErrorCode.h"
#include "../common/Packet.h"
#include "PacketBufferManager.h"
#include "ClientNetwork.h"
#include "UserInfo.h"

//채팅 앱의 기능 관련 컨트롤러
class ChatManager : public ClientNetwork {
private:
	UserInfo userInfo;
	unique_ptr<PacketBufferManager> packetBufferManager;

public:
	bool Init(const UINT16 SERVER_PORT, const char* SERVER_IP) {
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

	void OnReceive(char* data, UINT16 size, bool errflag, UINT32 err) {
		if (errflag) {
			printf("서버 통신 오류\n");
			printf("종료: /exit\n");
		}
		else {
			printf("[RECV] size: %d\n", size);
			packetBufferManager->OnDataReceive(data, size);
		}
	}
	void OnSend(char* data, UINT16 size, bool errflag, UINT32 err) {
		if (errflag) {
			printf("error: %d\n", err);
		}
		else {
			printf("[SEND] size: %d\n", size);
		}
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
		if (msg.length() >= ECHO_MSG_LEN) {
			cout << "메시지가 너무 깁니다." << endl;
			return false;
		}
		EchoPacket echoPkt;
		echoPkt.packetID = (UINT16)PACKET_ID::ECHO;
		echoPkt.packetSize = sizeof(EchoPacket);
		CopyMemory(echoPkt.msg, msg.c_str(), msg.length());
		return SendData((char*)&echoPkt, sizeof(EchoPacket));
	}

	bool ChatMsg(string msg) {
		if (msg.length() >= CHAT_MSG_LEN) {
			cout << "메시지가 너무 깁니다." << endl;	//TODO: 쪼개서 보내기
			return false;
		}
		ChatRequestPacket chatPkt;
		chatPkt.packetID = (UINT16)PACKET_ID::CHAT_REQUEST;
		chatPkt.packetSize = sizeof(ChatRequestPacket);
		CopyMemory(chatPkt.msg, msg.c_str(), msg.length());
		return SendData((char*)&chatPkt, sizeof(ChatRequestPacket));
	}

};