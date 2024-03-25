#pragma once

#include "NetworkConfig.h"

#include <winsock2.h>
#include <string>
#include <iostream>
using namespace std;

namespace ChatServerLib {

	struct ChatConfig : public ServerNetLib::NetworkConfig {
		UINT16 MAX_ROOM_CNT;
		UINT16 MAX_ROOM_USER_CNT;

		void LoadConfig() {
			NetworkConfig::LoadConfig();
			cout << "[LOAD CHAT CONFIG]" << endl;

			MAX_ROOM_CNT = GetPrivateProfileInt(L"ChatConfig", L"MAX_ROOM_CNT", 64, configFilePath.c_str());
			cout << "MAX_ROOM_CNT : " << MAX_ROOM_CNT << endl;
			MAX_ROOM_USER_CNT = GetPrivateProfileInt(L"ChatConfig", L"MAX_ROOM_USER_CNT", 64, configFilePath.c_str());
			cout << "MAX_ROOM_USER_CNT : " << MAX_ROOM_USER_CNT << endl;
		}
	};
}
