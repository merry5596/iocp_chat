#pragma once
#include "Define.h"
#include "TcpNetwork.h"
#include "ErrorCode.h"
#include "Packet.h"
#include "PacketBufferManager.h"
#include "UserInfo.h"

#include <iostream>
using namespace std;

//채팅 앱의 기능 관련 컨트롤러
class ChatManager : public ClientNetLib::TcpNetwork {
private:
	unique_ptr<PacketBufferManager> packetBufferManager;

public:
	bool Init(const UINT16 SERVER_PORT, const char* SERVER_IP) {
		packetBufferManager = make_unique<PacketBufferManager>();
		packetBufferManager->Init();
		return TcpNetwork::Init(SERVER_PORT, SERVER_IP);
	}

	void Start() {
		packetBufferManager->Start();
		TcpNetwork::Start();
	}

	void End() {
		packetBufferManager->End();
		TcpNetwork::End();
	}

	void OnReceive(char* data, UINT16 size, bool errflag, UINT32 err) {
		if (errflag) {
			printf("서버 통신이 종료됨\n");
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

	bool Login(const char* name) {
		LoginRequestPacket loginPkt;
		loginPkt.packetID = (UINT16)PACKET_ID::LOGIN_REQUEST;
		loginPkt.packetSize = sizeof(LoginRequestPacket);
		strcpy_s(loginPkt.name, strlen(name) + 1, name);

		if (SendData((char*)&loginPkt, sizeof(LoginRequestPacket)) == false) {
			return false;
		}
		//결과 기다렸다가 반환
		return packetBufferManager->GetLoginResult();
	}

	bool EnterRoom(UINT16 roomNum) {
		RoomEnterRequestPacket roomEnterPkt;
		roomEnterPkt.packetID = (UINT16)PACKET_ID::ROOM_ENTER_REQUEST;
		roomEnterPkt.packetSize = sizeof(RoomEnterRequestPacket);
		roomEnterPkt.roomNum = roomNum;

		if (SendData((char*)&roomEnterPkt, sizeof(RoomEnterRequestPacket)) == false) {
			return false;
		}
		//결과 기다렸다가 반환
		return packetBufferManager->GetRoomEnterResult();
	}

	bool LeaveRoom() {
		RoomLeaveRequestPacket reqPkt;
		reqPkt.packetID = (UINT16)PACKET_ID::ROOM_LEAVE_REQUEST;
		reqPkt.packetSize = sizeof(RoomLeaveRequestPacket);

		if (SendData((char*)&reqPkt, sizeof(RoomLeaveRequestPacket)) == false) {
			return false;
		}
		//결과 기다렸다가 반환
		return packetBufferManager->GetRoomLeaveResult();
	}

	bool EchoMsg(const char* msg) {
		if (strlen(msg) >= ECHO_MSG_LEN) {
			cout << "메시지가 너무 깁니다." << endl;
			return false;
		}
		EchoRequestPacket echoPkt;
		echoPkt.packetID = (UINT16)PACKET_ID::ECHO_REQUEST;
		echoPkt.packetSize = sizeof(EchoRequestPacket);
		CopyMemory(echoPkt.msg, msg, strlen(msg) + 1);
		return SendData((char*)&echoPkt, sizeof(EchoRequestPacket));
	}

	bool ChatMsg(const char* msg) {
		if (strlen(msg) >- CHAT_MSG_LEN) {
			cout << "메시지가 너무 깁니다." << endl;	//TODO: 쪼개서 보내기
			return false;
		}
		ChatRequestPacket chatPkt;
		chatPkt.packetID = (UINT16)PACKET_ID::CHAT_REQUEST;
		chatPkt.packetSize = sizeof(ChatRequestPacket);
		CopyMemory(chatPkt.msg, msg, strlen(msg) + 1);
		return SendData((char*)&chatPkt, sizeof(ChatRequestPacket));
	}

};