#pragma once

#include "Define.h"
#include "ClientConnection.h"

#include <vector>
#include <thread>
#include <iostream>
using namespace std;

namespace ServerNetLib {

	class IOCPNetwork {
	private:
		HANDLE IOCPHandle;
		SOCKET listenSocket;

		vector<ClientConnection*> clientPool;

		thread accepterThread;
		vector<thread> workerThreadPool;
		bool isAccepterRun;
		bool isWorkerRun;

	public:
		~IOCPNetwork();
		bool IOCPInit(UINT16 SERVER_PORT, UINT16 CLIENTPOOL_SIZE);
		void IOCPStart();
		void IOCPEnd();
		void SendData(UINT32 clientIndex, char* data, UINT16 size);

		virtual void OnConnect(UINT32 clientIndex) {}
		virtual void OnReceive(UINT32 clientIndex, char* data, UINT16 size) {}
		virtual void OnSend(UINT32 clientIndex, UINT16 size) {}
		virtual void OnDisconnect(UINT32 clientIndex) {}

	private:
		void CreateClientPool(UINT16 CLIENTPOOL_SIZE);
		void AccepterThread();
		void WorkerThread();

	};
}