#pragma once

#include "Packet.h"
#include "User.h"

#include <unordered_set>
#include <string>
using namespace std;

class Room {
private:
	UINT16 roomNum;
	UINT16 maxUserCnt;
	UINT16 curUserCnt;
	unordered_set<UINT32> userDic;

public:
	Room(UINT16 roomNum, UINT16 maxUserCnt) : roomNum(roomNum), maxUserCnt(maxUserCnt), curUserCnt(0) {}
	
	void EnterRoom(UINT32 clientIndex) {
		userDic.insert(clientIndex);
		curUserCnt++;
		cout << roomNum << "번 방 인원: " << userDic.size() << endl;
	}

	void LeaveRoom(UINT32 clientIndex) {
		userDic.erase(clientIndex);
		curUserCnt--;
		cout << roomNum << "번 방 인원: " << userDic.size() << endl;
	}

	UINT16 GetCurUserCnt() const {
		return curUserCnt;
	}

	bool IsFull() const {
		return (curUserCnt == maxUserCnt) ? true : false;
	}
};