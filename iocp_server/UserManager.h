#pragma once

#include "User.h"
#include <vector>

class UserManager {
private:
	vector<User*> userList;
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
};