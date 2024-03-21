#pragma once

#include "Room.h"

#include <vector>
using namespace std;

const UINT16 MAX_ROOM_CNT = 30;
const UINT16 MAX_ROOM_USER_CNT = 4;

class RoomManager {
private:
	vector<Room*> roomList;

public:
	void Init() {
		roomList = vector<Room*>(MAX_ROOM_CNT + 1);
		for (int i = 1; i <= MAX_ROOM_CNT; i++) {
			roomList[i] = new Room(i, MAX_ROOM_USER_CNT);
		}
	}

	void EnterRoom(UINT16 roomNum, UINT32 clientIndex) {
		roomList[roomNum]->EnterRoom(clientIndex);
	}

	void LeaveRoom(UINT16 roomNum, UINT32 clientIndex) {
		roomList[roomNum]->LeaveRoom(clientIndex);
	}

	UINT16 EnterRandomRoom(UINT32 clientIndex) {
		UINT16 roomNum = FindOptimalRoomNum();
		if (roomNum != 0) {
			roomList[roomNum]->EnterRoom(clientIndex);
		}
		return roomNum;
	}

	UINT16 FindOptimalRoomNum() {
		UINT16 userCntMin = MAX_ROOM_USER_CNT;
		UINT16 userCntMinRN = 0;	//방배정 1순위 RoomNum
		UINT16 userCntZeroRN = 0;	//방배정 2순위 RoomNum

		for (int i = 1; i <= MAX_ROOM_CNT; i++) {
			int userCnt = roomList[i]->GetCurUserCnt();
			if (userCnt == 0) {
				if (userCntZeroRN == 0) {	//아직 zeroCntRoom 못찾음. 즉 최초 발견
					userCntZeroRN = i;
				}
			}
			else if (userCnt < userCntMin) {
				userCntMin = userCnt;
				userCntMinRN = i;
			}
		}
		if (userCntMin == MAX_ROOM_USER_CNT) {
			if (userCntZeroRN == 0) {
				return 0;	//모든 방이 꽉 찼다.
			}
			else {
				return userCntZeroRN;	//0명 있는 방을 주자.
			}
		}
		return userCntMinRN; //min명 있는 방을 주자.
	}

	Room* GetRoom(UINT16 roomNum) {
		if (roomNum < 1 || roomNum > MAX_ROOM_CNT) {
			return nullptr;
		}
		return roomList[roomNum];
	}

	bool IsFull(UINT16 roomNum) {
		return roomList[roomNum]->IsFull();
	}
};