#pragma once

#include "Define.h"
#include "ClientConnection.h"
#include "ServerConfig.h"

#include <vector>
#include <thread>
using namespace std;

namespace ServerNetLib {

	class IOCPNetwork {
	private:
		HANDLE IOCPHandle;
		SOCKET listenSocket;

		vector<ClientConnection*> clientPool;
		UINT16 clientPoolSize;

		thread accepterThread;
		bool isAccepterRun;

		vector<thread> workerThreadPool;
		UINT16 threadPoolSize;
		bool isWorkerRun;

	public:
		~IOCPNetwork();
		bool IOCPInit(ServerConfig* serverConfig);
		void IOCPStart();
		void IOCPEnd();
		void SendData(UINT32 clientIndex, char* data, UINT16 size);

		virtual void OnConnect(UINT32 clientIndex) {}
		virtual void OnReceive(UINT32 clientIndex, char* data, UINT16 size) {}
		virtual void OnSend(UINT32 clientIndex, UINT16 size) {}
		virtual void OnDisconnect(UINT32 clientIndex) {}

	private:
		void CreateClientPool(const UINT16 clientPoolSize, const UINT16 bufferSize);
		void AccepterThread();
		void WorkerThread();

	};
}