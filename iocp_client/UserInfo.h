#pragma once

#include "Define.h"
#include "Packet.h"

enum class USER_STATUS {
	

};

class UserInfo {
private:
	UINT16 status;
	char name[NAME_LEN];
	UINT16 roomIndex;
public:
	void SetName(char* name) {
		strcpy_s(this->name, NAME_LEN, name);
	}

	char* GetName() {
		return name;
	}
};