#pragma once

#include "Packet.h"

#include <WinSock2.h>
#include <queue>
#include <mutex>
using namespace std;

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
	void AddChatNotify(char* name, char* msg) {
		Notify ntf;
		ntf.packetID = (UINT16)PACKET_ID::CHAT_NOTIFY;
		strcpy_s(ntf.name, NAME_LEN, name);
		strcpy_s(ntf.msg, CHAT_MSG_LEN, msg);
		lock_guard<mutex> lock(mtx);
		notifyQueue.push(ntf);
	}

	void AddRoomEnterNotify(char* name) {
		Notify ntf;
		ntf.packetID = (UINT16)PACKET_ID::ROOM_ENTER_NOTIFY;
		strcpy_s(ntf.name, NAME_LEN, name);
		lock_guard<mutex> lock(mtx);
		notifyQueue.push(ntf);
	}

	void AddRoomLeaveNotify(char* name) {
		Notify ntf;
		ntf.packetID = (UINT16)PACKET_ID::ROOM_LEAVE_NOTIFY;
		strcpy_s(ntf.name, NAME_LEN, name);
		lock_guard<mutex> lock(mtx);
		notifyQueue.push(ntf);
	}

	void AddDisconnectNotify() {
		Notify ntf;
		ntf.packetID = (UINT16)PACKET_ID::DISCONNECT;
		lock_guard<mutex> lock(mtx);
		notifyQueue.push(ntf);
	}

	Notify GetNotify() {
		lock_guard<mutex> lock(mtx);
		if (notifyQueue.empty()) {
			return Notify();
		}
		Notify ntf = notifyQueue.front();
		notifyQueue.pop();
		return ntf;
	}
};