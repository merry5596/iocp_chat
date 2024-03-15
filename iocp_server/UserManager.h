#pragma once

#include "User.h"
#include <vector>
#include <unordered_map>
#include <mutex>

class UserManager {
private:
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

	bool AddUser(char* name, UINT32 clientIndex) {
		if (IsExistingName(name)) {	//중복됨
			return false;
		}
		userList[clientIndex]->SetLogin(name);
		userDic.insert(pair<char*, UINT32>(name, clientIndex));

		return true;
	}

	void DeleteUser(UINT32 clientIndex) {
		userDic.erase(userList[clientIndex]->GetName());
		userList[clientIndex]->SetLogout();
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