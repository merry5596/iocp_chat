#pragma once

#include "Packet.h"

#include <WinSock2.h>
#include <queue>
#include <mutex>
using namespace std;

namespace ChatClientLib {

	struct Notify {
		UINT16 packetID = 0;
		char name[NAME_LEN];
		char msg[CHAT_MSG_LEN];
	};

	//갑자기 오는 서버 알림 (타유저의 방 입장, 퇴장, 채팅)
	class NotifyManager {
	private:
		queue<Notify> notifyQueue;
		mutex mtx;

	public:
		void AddChatNotify(char* name, char* msg);
		void AddRoomEnterNotify(char* name);
		void AddRoomLeaveNotify(char* name);
		void AddDisconnectNotify();
		Notify GetNotify();
	};

}