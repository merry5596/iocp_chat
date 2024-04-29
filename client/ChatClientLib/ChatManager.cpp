#include "ChatManager.h"

namespace ChatClientLib {

	bool ChatManager::Init(const UINT16 SERVER_PORT, const char* SERVER_IP) {
		userInfo = make_unique<UserInfo>();
		notifyManager = make_unique<NotifyManager>();
		packetBufferManager = make_unique<PacketBufferManager>();
		packetBufferManager->Init(notifyManager.get(), userInfo.get());
		return TcpNetwork::Init(SERVER_PORT, SERVER_IP);
	}

	void ChatManager::Start() {
		packetBufferManager->Start();
		TcpNetwork::Start();
		isRun = true;
	}

	void ChatManager::End() {
		if (isRun) {
			packetBufferManager->End();
			TcpNetwork::End();
			isRun = false;
		}
	}

	void ChatManager::OnReceive(char* data, UINT16 size, bool errflag, UINT32 err) {
		if (errflag) {
			spdlog::info("서버 통신 종료");
			notifyManager->AddDisconnectNotify();
		}
		else {
			spdlog::debug("[RECV] size: {}", size);
			packetBufferManager->OnDataReceive(data, size);
		}
	}

	void ChatManager::OnSend(char* data, UINT16 size, bool errflag, UINT32 err) {
		if (errflag) {
			spdlog::error("error : {}", err);
		}
		else {
			spdlog::debug("[SEND] size: {}", size);
		}
	}

	UINT16 ChatManager::Login(const char* name) {
		LoginRequestPacket loginPkt;
		loginPkt.packetID = (UINT16)PACKET_ID::LOGIN_REQUEST;
		loginPkt.packetSize = sizeof(LoginRequestPacket);
		strcpy_s(loginPkt.name, NAME_LEN, name);

		if (SendData((char*)&loginPkt, sizeof(LoginRequestPacket)) == false) {
			return false;
		}
		//결과 기다렸다가 반환
		return packetBufferManager->GetLoginResult();
	}

	UINT16 ChatManager::EnterRoom(UINT16 roomNum) {
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

	UINT16 ChatManager::LeaveRoom() {
		RoomLeaveRequestPacket reqPkt;
		reqPkt.packetID = (UINT16)PACKET_ID::ROOM_LEAVE_REQUEST;
		reqPkt.packetSize = sizeof(RoomLeaveRequestPacket);

		if (SendData((char*)&reqPkt, sizeof(RoomLeaveRequestPacket)) == false) {
			return false;
		}
		//결과 기다렸다가 반환
		return packetBufferManager->GetRoomLeaveResult();
	}

	bool ChatManager::EchoMsg(const char* msg) {
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

	bool ChatManager::ChatMsg(const char* msg) {
		if (strlen(msg) > -CHAT_MSG_LEN) {
			cout << "메시지가 너무 깁니다." << endl;	//TODO: 쪼개서 보내기
			return false;
		}
		ChatRequestPacket chatPkt;
		chatPkt.packetID = (UINT16)PACKET_ID::CHAT_REQUEST;
		chatPkt.packetSize = sizeof(ChatRequestPacket);
		CopyMemory(chatPkt.msg, msg, strlen(msg) + 1);
		return SendData((char*)&chatPkt, sizeof(ChatRequestPacket));
	}

	Notify ChatManager::GetNotify() {
		return notifyManager->GetNotify();
	}

	UINT16 ChatManager::GetUserState() const {
		return userInfo->GetState();
	}
	char* ChatManager::GetUserNickName() {
		return userInfo->GetName();
	}
	UINT16 ChatManager::GetUserRoomNum() const {
		return userInfo->GetRoomNum();
	}
}