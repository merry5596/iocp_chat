#pragma once

#include <winsock2.h>
#include <string>
#include <iostream>
using namespace std;

namespace ServerNetLib {

	struct NetworkConfig {
		wstring configFilePath;
		UINT16 SERVER_PORT;
		UINT16 CLIENT_POOL_SIZE;
		UINT16 BACK_LOG;
		UINT16 THREAD_POOL_SIZE;
		UINT16 BUFFER_SIZE;

		void LoadConfig() {
			cout << "[LOAD NETWORK CONFIG]" << endl;
			configFilePath = L"C:\\iocp\\server\\iocp_server\\config.ini";
			SERVER_PORT = GetPrivateProfileInt(L"NetworkConfig", L"SERVER_PORT", 8080, configFilePath.c_str());
			cout << "SERVER_PORT : " << SERVER_PORT << endl;
			CLIENT_POOL_SIZE = GetPrivateProfileInt(L"NetworkConfig", L"CLIENT_POOL_SIZE", 90, configFilePath.c_str());
			cout << "CLIENT_POOL_SIZE : " << CLIENT_POOL_SIZE << endl;
			BACK_LOG = GetPrivateProfileInt(L"NetworkConfig", L"BACK_LOG", 3, configFilePath.c_str());
			cout << "BACK_LOG : " << BACK_LOG << endl;
			THREAD_POOL_SIZE = GetPrivateProfileInt(L"NetworkConfig", L"THREAD_POOL_SIZE", 2, configFilePath.c_str());
			cout << "THREAD_POOL_SIZE : " << THREAD_POOL_SIZE << endl;
			BUFFER_SIZE = GetPrivateProfileInt(L"NetworkConfig", L"BUFFER_SIZE", 64, configFilePath.c_str());
			cout << "BUFFER_SIZE : " << BUFFER_SIZE << endl;
		}
	};
}
