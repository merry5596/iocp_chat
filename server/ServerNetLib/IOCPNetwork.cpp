#include "IOCPNetwork.h"

namespace ServerNetLib {

	IOCPNetwork::~IOCPNetwork() {
		WSACleanup();
		for (auto client : clientPool) {
			delete client;
		}
	}

	bool IOCPNetwork::IOCPInit(NetworkConfig* config) {
		//Winsock 사용
		WSAData wsaData;
		int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);	//2.2 버전으로 초기화, wsaData에 저장
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
		addr.sin_port = htons(config->SERVER_PORT);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		ret = bind(listenSocket, (const SOCKADDR*)&addr, (int)sizeof(SOCKADDR_IN));
		if (ret != 0) {
			printf("[ERROR]bind() error: %d", WSAGetLastError());
			return false;
		}

		//리슨 소켓 등록
		ret = listen(listenSocket, config->BACK_LOG);
		if (ret != 0) {
			printf("[ERROR]listen() error: %d", WSAGetLastError());
			return false;
		}

		//IOCP 핸들러 생성
		threadPoolSize = config->THREAD_POOL_SIZE;
		IOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, threadPoolSize);
		if (IOCPHandle == NULL) {
			printf("[ERROR]CreateIoCompletionPort()(create) error: %d", WSAGetLastError());
		}

		//IOCP 핸들러에 소켓 등록(GQCS 받겠다)
		HANDLE retHandle = CreateIoCompletionPort((HANDLE)listenSocket, IOCPHandle, 0, 0);
		if (retHandle == NULL) {
			printf("[ERROR]CreateIoCompletionPort()(bind listener) error: %d", WSAGetLastError());
		}

		//커넥션 풀 생성
		clientPoolSize = config->CLIENT_POOL_SIZE;
		CreateClientPool(clientPoolSize, config->BUFFER_SIZE);

		return true;
	}

	void IOCPNetwork::IOCPStart() {
		//Worker 스레드 시작
		isWorkerRun = true;
		for (int i = 0; i < threadPoolSize; i++) {
			workerThreadPool.emplace_back([this]() { WorkerThread(); });
		}

		//Accept 스레드 시작
		isAccepterRun = true;
		accepterThread = thread([this]() { AccepterThread(); });
	}

	void IOCPNetwork::IOCPEnd() {
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

	void IOCPNetwork::SendData(UINT32 clientIndex, char* data, UINT16 size) {
		clientPool[clientIndex]->SendData(data, size);
	}

	void IOCPNetwork::CreateClientPool(UINT16 clientPoolSize, UINT16 bufferSize) {
		for (int i = 0; i < clientPoolSize; i++) {
			clientPool.push_back(new ClientConnection(i, bufferSize));
		}
	}

	void IOCPNetwork::AccepterThread() {
		while (isAccepterRun) {
			for (auto client : clientPool) {
				if (client->GetStatus() == (UINT16)CONNECTION_STATUS::READY) {
					client->PostAccept(listenSocket);
				}
			}
			this_thread::sleep_for(chrono::milliseconds(32));
		}
	}

	void IOCPNetwork::WorkerThread() {
		DWORD recvBytes = 0;
		ClientConnection* client = nullptr;
		LPOVERLAPPED lpOverlapped = nullptr;
		while (isWorkerRun) {
			bool ret = GetQueuedCompletionStatus(IOCPHandle, &recvBytes, (PULONG_PTR)&client, &lpOverlapped, INFINITE);
			if (lpOverlapped == NULL) {	//IOCPHandle Close 되면 ret == false, lpOverlapped = NULL 반환
				continue;
			}
			if (ret == false) {	//연결 끊김
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

}
