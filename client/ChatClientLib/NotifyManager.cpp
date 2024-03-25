#include "NotifyManager.h"

namespace ChatClientLib {

	void NotifyManager::AddChatNotify(char* name, char* msg) {
		Notify ntf;
		ntf.packetID = (UINT16)PACKET_ID::CHAT_NOTIFY;
		strcpy_s(ntf.name, NAME_LEN, name);
		strcpy_s(ntf.msg, CHAT_MSG_LEN, msg);
		lock_guard<mutex> lock(mtx);
		notifyQueue.push(ntf);
	}

	void NotifyManager::AddRoomEnterNotify(char* name) {
		Notify ntf;
		ntf.packetID = (UINT16)PACKET_ID::ROOM_ENTER_NOTIFY;
		strcpy_s(ntf.name, NAME_LEN, name);
		lock_guard<mutex> lock(mtx);
		notifyQueue.push(ntf);
	}

	void NotifyManager::AddRoomLeaveNotify(char* name) {
		Notify ntf;
		ntf.packetID = (UINT16)PACKET_ID::ROOM_LEAVE_NOTIFY;
		strcpy_s(ntf.name, NAME_LEN, name);
		lock_guard<mutex> lock(mtx);
		notifyQueue.push(ntf);
	}

	void NotifyManager::AddDisconnectNotify() {
		Notify ntf;
		ntf.packetID = (UINT16)PACKET_ID::DISCONNECT;
		lock_guard<mutex> lock(mtx);
		notifyQueue.push(ntf);
	}

	Notify NotifyManager::GetNotify() {
		lock_guard<mutex> lock(mtx);
		if (notifyQueue.empty()) {
			return Notify();
		}
		Notify ntf = notifyQueue.front();
		notifyQueue.pop();
		return ntf;
	}

}