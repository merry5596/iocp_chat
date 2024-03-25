#pragma once
#include "TcpNetwork.h"
#include "ErrorCode.h"
#include "Packet.h"
#include "PacketBufferManager.h"
#include "UserInfo.h"

#include <iostream>
using namespace std;

namespace ChatClientLib {

	class ChatManager : public ClientNetLib::TcpNetwork {
	private:
		unique_ptr<PacketBufferManager> packetBufferManager;
		unique_ptr<NotifyManager> notifyManager;

	public:
		bool Init(const UINT16 SERVER_PORT, const char* SERVER_IP);
		void Start();
		void End();
		void OnReceive(char* data, UINT16 size, bool errflag, UINT32 err);
		void OnSend(char* data, UINT16 size, bool errflag, UINT32 err);
		UINT16 Login(const char* name);
		UINT16 EnterRoom(UINT16 roomNum);
		UINT16 LeaveRoom();
		bool EchoMsg(const char* msg);
		bool ChatMsg(const char* msg);
		Notify GetNotify();
	};

}