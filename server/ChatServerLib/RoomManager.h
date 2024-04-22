#pragma once

#include "Room.h"

#include <vector>
using namespace std;

namespace ChatServerLib {

	class RoomManager {
	private:
		vector<Room*> roomList;
		UINT16 maxRoomCnt;
		UINT16 maxRoomUserCnt;
		shared_mutex rwMtx;

	public:
		void Init(const UINT16 maxRoomCnt, const UINT16 maxRoomUserCnt) {
			this->maxRoomCnt = maxRoomCnt;
			this->maxRoomUserCnt = maxRoomUserCnt;
			roomList = vector<Room*>(maxRoomCnt + 1);
			for (int i = 1; i <= maxRoomCnt; i++) {
				roomList[i] = new Room(i, maxRoomUserCnt);
			}
		}

		void EnterRoom(UINT16 roomNum, UINT32 clientIndex) {
			//writelock
			unique_lock<shared_mutex> lock(rwMtx);
			roomList[roomNum]->EnterRoom(clientIndex);
		}

		void LeaveRoom(UINT16 roomNum, UINT32 clientIndex) {
			//writelock
			unique_lock<shared_mutex> lock(rwMtx);
			roomList[roomNum]->LeaveRoom(clientIndex);
		}

		UINT16 EnterRandomRoom(UINT32 clientIndex) {
			//writelock
			unique_lock<shared_mutex> lock(rwMtx);
			UINT16 roomNum = FindOptimalRoomNum();
			if (roomNum != 0) {
				roomList[roomNum]->EnterRoom(clientIndex);
			}
			return roomNum;
		}

		UINT16 FindOptimalRoomNum() {
			UINT16 userCntMin = maxRoomUserCnt;
			UINT16 userCntMinRN = 0;	//방배정 1순위 RoomNum
			UINT16 userCntZeroRN = 0;	//방배정 2순위 RoomNum

			for (int i = 1; i <= maxRoomCnt; i++) {
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
			if (userCntMin == maxRoomUserCnt) {
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
			if (roomNum < 1 || roomNum > maxRoomCnt) {
				return nullptr;
			}
			//readlock
			shared_lock<shared_mutex> lock(rwMtx);
			return roomList[roomNum];
		}

		bool IsFull(UINT16 roomNum) {
			//readlock
			shared_lock<shared_mutex> lock(rwMtx);
			return roomList[roomNum]->IsFull();
		}

		unordered_set<UINT32> GetAllUserIndex(UINT16 roomNum) {
			//readlock
			shared_lock<shared_mutex> lock(rwMtx);
			return roomList[roomNum]->GetAllUserIndex();
		}
	};

}
