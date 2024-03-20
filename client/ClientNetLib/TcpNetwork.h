#pragma once

#include "Define.h"

#include <thread>
#include <errno.h>
#include <iostream>
using namespace std;

namespace ClientNetLib {

	class TcpNetwork {
	private:
		SOCKET sock;
		thread recvThread;
		bool isRecvRun;

	public:
		~TcpNetwork();
		bool Init(const UINT16 SERVER_PORT, const char* SERVER_IP);
		void Start();
		void End();
		bool Refresh(const UINT16 SERVER_PORT, const char* SERVER_IP);
		bool SendData(char* data, UINT16 size);
		void RecvThread();
		virtual void OnSend(char* data, UINT16 size, bool errflag, UINT32 err) {}
		virtual void OnReceive(char* data, UINT16 size, bool errflag, UINT32 err) {}
	private:
		bool CreateSocket();
		bool Connect(const UINT16 SERVER_PORT, const char* SERVER_IP);
	};

}