#pragma once
#pragma comment(lib, "ws2_32")

#include "Client.h"
#include <vector>
#include <thread>

class IOCPServer {
private:
	HANDLE IOCPHandle;
	SOCKET listenSocket;
	vector<Client*> clientPool;

	thread accepterThread;
	vector<thread> workerThreadPool;
	bool isAccepterRun;
	bool isWorkerRun;
public:
	~IOCPServer() {
		WSACleanup();
		for (auto client : clientPool) {
			delete client;
		}
	}

	int IOCPInit(UINT16 SERVER_PORT, UINT16 CLIENTPOOL_SIZE) {
		//Winsock 사용
		WSAData wsaData;
		auto ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (ret != 0) {
			printf("[ERROR]WSAStartup() error: %d", WSAGetLastError());
			return false;
		}

		//리슨 소켓 생성
		listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (listenSocket == INVALID_SOCKET) {
			printf("[ERROR]WSASocket() error: %d", WSAGetLastError());
			return false;
		}

		//리슨 소켓 주소연결
		SOCKADDR_IN addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(SERVER_PORT);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		ret = bind(listenSocket, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN));
		if (ret != 0) {	//실패
			printf("[ERROR]bind() error: %d", WSAGetLastError());
			return false;
		}

		//리슨 소켓 등록
		ret = listen(listenSocket, 5);	//backlog(접속대기큐): 5
		if (ret != 0) {	//실패
			printf("[ERROR]listen() error: %d", WSAGetLastError());
			return false;
		}

		//IOCP 핸들러 생성
		IOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, THREADPOOL_SIZE);
		if (IOCPHandle == NULL) {
			printf("[ERROR]CreateIoCompletionPort()(create) error: %d", WSAGetLastError());
		}

		//IOCP 핸들러에 등록(GQCS 받겠다)
		HANDLE retHandle = CreateIoCompletionPort((HANDLE)listenSocket, IOCPHandle, 0, 0);
		if (retHandle == NULL) {
			printf("[ERROR]CreateIoCompletionPort()(bind listener) error: %d", WSAGetLastError());
		}

		//커넥션 풀 생성
		CreateClientPool(CLIENTPOOL_SIZE);

		return true;
	}

	void IOCPStart() {
		//Receive 스레드 시작
		isWorkerRun = true;
		for (int i = 0; i < THREADPOOL_SIZE; i++) {
			workerThreadPool.emplace_back([this]() { WorkerThread(); });
		}

		//Accept 스레드 시작
		isAccepterRun = true;
		accepterThread = thread([this]() { AccepterThread(); });
	}

	void IOCPEnd() {
		isWorkerRun = false;
		CloseHandle(IOCPHandle);
		for (auto& workerThread : workerThreadPool) {
			if (workerThread.joinable()) {
				workerThread.join();
			}
		}

		isAccepterRun = false;
		closesocket(listenSocket);
		if (accepterThread.joinable()) {
			accepterThread.join();
		}

	}

	void SendData(UINT32 clientIndex, char* data, UINT16 size) {
		clientPool[clientIndex]->SendData(data, size);
	}

	virtual void OnConnect(UINT32 clientIndex) {}
	virtual void OnReceive(UINT32 clientIndex, char* data, UINT16 size) {}
	virtual void OnSend(UINT32 clientIndex, UINT16 size) {}
	virtual void OnDisconnect(UINT32 clientIndex) {}

private:
	void CreateClientPool(UINT16 CLIENTPOOL_SIZE) {
		for (int i = 0; i < CLIENTPOOL_SIZE; i++) {
			clientPool.push_back(new Client(i));
		}
	}

	void AccepterThread() {
		while (isAccepterRun) {
			for (auto client : clientPool) {
				if (client->GetStatus() == CONNECTION_STATUS::READY) {
					client->PostAccept(listenSocket);
				}
			}
			this_thread::sleep_for(chrono::milliseconds(32));
		}
	}

	void WorkerThread() {
		DWORD recvBytes = 0;
		Client* client = nullptr;
		LPOVERLAPPED lpOverlapped = nullptr;
		while (isWorkerRun) {
			bool ret = GetQueuedCompletionStatus(IOCPHandle, &recvBytes, (PULONG_PTR)&client, &lpOverlapped, INFINITE);

			//IOCPHandle Close 되면 ret == false, lpOverlapped = NULL 반환
			if (lpOverlapped == NULL) {
				continue;
			}

			//연결 끊김
			if (ret == false) {
				client->CloseSocket();
				OnDisconnect(client->GetIndex());
				continue;
			}

			WSAOverlappedEx* wsaOverlappedEx = (WSAOverlappedEx*)lpOverlapped;
			if (wsaOverlappedEx->operation == IOOperation::ACCEPT) {
				OnConnect(wsaOverlappedEx->clientIndex);
				//IOCP에 등록
				auto client = clientPool[wsaOverlappedEx->clientIndex];
				ret = client->ConnectIOCP(IOCPHandle);
				if (ret == false) {
					continue;
				}
			}
			else if (wsaOverlappedEx->operation == IOOperation::RECV) {
				OnReceive(wsaOverlappedEx->clientIndex, wsaOverlappedEx->wsaBuf.buf, recvBytes);
				client->PostReceive();
			}
			else if (wsaOverlappedEx->operation == IOOperation::SEND) {
				OnSend(wsaOverlappedEx->clientIndex, wsaOverlappedEx->wsaBuf.len);
				client->SendCompleted();
			}
			else {
				printf("[EXCEPTION]client index: %d\n", wsaOverlappedEx->clientIndex);
			}
		}
	}
};