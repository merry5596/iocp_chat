#pragma once

#include "Packet.h"

enum class USER_STATE : UINT16 {
	NONE = 0,
	LOGIN = 1,
	ROOM = 2,
};

class UserInfo {
private:
	UINT16 state;
	char name[NAME_LEN];
	UINT16 roomNum;

public:
	UserInfo() : state((UINT16)USER_STATE::NONE) {
		ZeroMemory(name, NAME_LEN);
		//roomNum = 0;
	}
	~UserInfo() {}

	void Login(const char* name) {
		strcpy_s(this->name, NAME_LEN, name);
		this->state = (UINT16)USER_STATE::LOGIN;
	}

	void EnterRoom(UINT16 roomNum) {
		this->roomNum = roomNum;
		this->state = (UINT16)USER_STATE::ROOM;
	}

	void LeaveRoom() {
		this->roomNum = 0;
		this->state = (UINT16)USER_STATE::LOGIN;
	}

	//Getters
	UINT16 GetState() const { return state; }
	char* GetName() { return name; }
};