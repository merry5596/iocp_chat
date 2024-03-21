#pragma once

#include "Packet.h"
#include "User.h"

#include <unordered_map>
using namespace std;

class Room {
private:
	UINT16 roomNum;
	UINT16 maxUserCnt;
	UINT16 curUserCnt;
	//userlist;

public:
	Room(UINT16 roomNum, UINT16 maxUserCnt) : roomNum(roomNum), maxUserCnt(maxUserCnt), curUserCnt(0) {
		//userList 초기화
	}
	
	void EnterRoom(UINT32 clientIndex) {
		//userList 추가
		curUserCnt++;
	}

	UINT16 GetCurUserCnt() const {
		return curUserCnt;
	}

	bool IsFull() const {
		return (curUserCnt == maxUserCnt) ? true : false;
	}
};