#pragma once

#include "Define.h"

#include <mutex>
#include <queue>
#include <iostream>
using namespace std;

namespace ServerNetLib {

	enum class CONNECTION_STATUS : UINT16 {
		READY = 0,
		WAITING_FOR_ACCEPT = 1,
		CONNECTING = 2,
	};

	class ClientConnection {
	private:
		UINT32 index;
		UINT8 status;
		SOCKET acceptSocket;

		char acceptBuffer[BUFFER_SIZE];
		WSAOverlappedEx acceptOverlappedEx;

		char recvBuffer[BUFFER_SIZE];
		WSAOverlappedEx recvOverlappedEx;

		queue<WSAOverlappedEx*> sendingQueue;
		mutex sendMtx;

		//	UINT32 latestClosedTime = 0;
	public:
		ClientConnection(UINT32 index);

		bool PostAccept(SOCKET listenSocket);
		bool ConnectIOCP(HANDLE IOCPHandle);
		bool PostReceive();
		void SendData(char* data, UINT16 size);
		void PostSend();
		void SendCompleted();
		void CloseSocket(bool isForce = false);

		UINT32 GetIndex() const;
		UINT8 GetStatus() const;
	};

}
