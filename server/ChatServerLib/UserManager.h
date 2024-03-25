#pragma once

#include "User.h"

#include <vector>
#include <unordered_map>
#include <mutex>
#include <iostream>
using namespace std;

namespace ChatServerLib {

	class UserManager {
	private:
		UINT32 maxUserCnt;
		UINT32 curUserCnt;
		vector<User*> userList;
		unordered_map<string, UINT32> userDic;
		mutex mtx;
	public:
		UserManager() {}
		~UserManager() {
			for (auto user : userList) {
				delete user;
			}
		}
		void Init(const UINT16 CLIENTPOOL_SIZE) {
			for (int i = 0; i < CLIENTPOOL_SIZE; i++) {
				userList.push_back(new User(i));
			}
			maxUserCnt = CLIENTPOOL_SIZE;
			curUserCnt = 0;
		}

		void SetPacket(UINT32 clientIndex, char* data, UINT16 size) {
			userList[clientIndex]->SetPacket(data, size);
		}

		PacketInfo GetPacket(UINT32 clientIndex) {
			return userList[clientIndex]->GetPacket();
		}

		User* GetUser(UINT32 clientIndex) {
			return userList[clientIndex];
		}

		bool SetLogin(char* name, UINT32 clientIndex) {
			if (IsExistingName(name)) {	//중복됨
				return false;
			}
			userList[clientIndex]->SetLogin(name);
			userDic.insert(pair<char*, UINT32>(name, clientIndex));
			curUserCnt++;

			return true;
		}

		void SetLogout(UINT32 clientIndex) {
			userDic.erase(userList[clientIndex]->GetName());
			userList[clientIndex]->SetLogout();
			curUserCnt--;
		}

		void EnterRoom(UINT32 clientIndex, UINT16 roomNum) {
			userList[clientIndex]->EnterRoom(roomNum);
		}

		UINT16 LeaveRoom(UINT32 clientIndex) {
			return userList[clientIndex]->LeaveRoom();
		}

		UINT16 GetRoomNum(UINT32 clientIndex) {
			return userList[clientIndex]->GetRoomNum();
		}

		UINT16 GetUserState(UINT32 clientIndex) {
			return userList[clientIndex]->GetState();
		}

		vector<UINT16> GetAllUserIndex() {
			vector<UINT16> userIndexList;
			for (auto& userPair : userDic) {
				userIndexList.push_back(userPair.second);
			}
			return userIndexList;
		}

	private:
		bool IsExistingName(char* name) {
			auto iter = userDic.find(name);
			if (iter != userDic.end()) {
				return true;
			}
			return false;
		}
	};

}
