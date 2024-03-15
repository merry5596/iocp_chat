#pragma once

#include "Define.h"
#include "Packet.h"

#include <mutex>
#include <queue>
#pragma comment (lib, "mswsock.lib")

enum CONNECTION_STATUS {
	READY = 0,
	WAITING_FOR_ACCEPT = 1,
	CONNECTING = 2,
};

class Client {
private:
	UINT32 index;
	UINT8 status;
	SOCKET acceptSocket;
	char acceptBuffer[BUFFER_SIZE];
	WSAOverlappedEx acceptOverlappedEx;

	char recvBuffer[BUFFER_SIZE];
	WSAOverlappedEx recvOverlappedEx;

	//char sendBuffer[BUFFER_SIZE];
	//WSAOverlappedEx sendOverlappedEx;

	queue<WSAOverlappedEx*> sendingQueue;
	mutex sendMtx;

	bool isSending;
	//	UINT32 latestClosedTime = 0;
public:
	Client();
	Client(UINT32 index) {
		this->index = index;
		status = CONNECTION_STATUS::READY;
		isSending = false;
	}
	UINT8 GetStatus() const {
		return status;
	}
	UINT32 GetIndex() const {
		return index;
	}
	bool PostAccept(SOCKET listenSocket) {	//AccepterThread에서 접근(단일스레드)
		acceptSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (acceptSocket == INVALID_SOCKET) {
			
			printf("[ERROR]WSASocket() error: %d\n", WSAGetLastError());
			return false;
		}

		DWORD bytes = 0;
		ZeroMemory(acceptBuffer, BUFFER_SIZE);
		ZeroMemory(&acceptOverlappedEx, sizeof(WSAOverlappedEx));
		acceptOverlappedEx.operation = IOOperation::ACCEPT;
		acceptOverlappedEx.clientIndex = index;
		acceptOverlappedEx.wsaBuf.buf = nullptr;
		acceptOverlappedEx.wsaBuf.len = 0;
		bool ret = AcceptEx(listenSocket, acceptSocket, acceptBuffer, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &bytes, (LPOVERLAPPED) & acceptOverlappedEx);
		if (ret == false && WSAGetLastError() != ERROR_IO_PENDING) {
			printf("[ERROR]AcceptEx() error: %d\n", WSAGetLastError());
			return false;
		}
		status = CONNECTION_STATUS::WAITING_FOR_ACCEPT;

		return true;
	}

	bool ConnectIOCP(HANDLE IOCPHandle) {
		auto retHandle = CreateIoCompletionPort((HANDLE)acceptSocket, IOCPHandle, (ULONG_PTR)this, 0);
		if (retHandle == NULL) {
			printf("[ERROR]CreateIoCompletionPort()(bind accepter) error: %d\n", WSAGetLastError());
			return false;
		}

		bool ret = PostReceive();
		if (ret == false) {
			return false;
		}
		
		return true;
	}

	bool PostReceive() {	//accept 성공 직후 첫 호출. accpet는 한번이므로 앞으로는 WSARecv 완료시에만 이 함수가 호출. 즉, 멀티스레드 접근 불가
		DWORD bufCnt = 1;	//버퍼 개수. 일반적으로 1개로 설정
		DWORD bytes = 0;
		DWORD flags = 0;
		ZeroMemory(recvBuffer, BUFFER_SIZE);
		ZeroMemory(&recvOverlappedEx, sizeof(WSAOverlappedEx));
		recvOverlappedEx.operation = IOOperation::RECV;
		recvOverlappedEx.clientIndex = index;
		recvOverlappedEx.wsaBuf.len = BUFFER_SIZE;
		recvOverlappedEx.wsaBuf.buf = recvBuffer;
		int ret = WSARecv(acceptSocket, &(recvOverlappedEx.wsaBuf), bufCnt, &bytes, &flags, (LPWSAOVERLAPPED) & recvOverlappedEx, NULL);
		if (ret != 0 && WSAGetLastError() != ERROR_IO_PENDING) {
			printf("[ERROR]WSARecv() error: %d\n", WSAGetLastError());
			return false;
		}
		status = CONNECTION_STATUS::CONNECTING;

		return true;
	}

	void SendData(char* data, UINT16 size) {	//PacketThread가 접근
		char* sendBuffer = new char[BUFFER_SIZE];
		CopyMemory(sendBuffer, data, size);
		WSAOverlappedEx* sendOverlappedEx = new WSAOverlappedEx;
		ZeroMemory(sendOverlappedEx, sizeof(WSAOverlappedEx));
		sendOverlappedEx->operation = IOOperation::SEND;
		sendOverlappedEx->clientIndex = index;
		sendOverlappedEx->wsaBuf.len = size;
		sendOverlappedEx->wsaBuf.buf = sendBuffer;

		lock_guard<mutex> lock(sendMtx);
		sendingQueue.push(sendOverlappedEx);
		if (sendingQueue.size() == 1) {
			PostSend();
		}
	}

	bool PostSend() {	//SenderThread가 접근. 함수 호출 전부터 뮤텍스 걸려있는 상태
		auto sendOverlappedEx = sendingQueue.front();
		DWORD bufCnt = 1;	//버퍼 개수. 일반적으로 1개로 설정
		DWORD bytes = 0;
		DWORD flags = 0;
		int ret = WSASend(acceptSocket, &(sendOverlappedEx->wsaBuf), bufCnt, &bytes, flags, (LPWSAOVERLAPPED)sendOverlappedEx, NULL);
		if (ret != 0 && WSAGetLastError() != ERROR_IO_PENDING) {
			printf("[ERROR]WSASend() error: %d\n", WSAGetLastError());
			return false;
		}

		return true;
	}

	void SendCompleted() {	//workerThread가 접근
		lock_guard<mutex> lock(sendMtx);
		auto completedThing = sendingQueue.front();
		delete completedThing->wsaBuf.buf;
		delete completedThing;

		sendingQueue.pop();
		if (sendingQueue.size() == 1) {
			PostSend();
		}
	}

	void CloseSocket(bool isForce = false) {
		linger lingerOpt = { 0, 0 };
		//강제 종료 시, 대기 안하고 즉시 종료
		if (isForce) {
			lingerOpt.l_onoff = 1;
		}

		shutdown(acceptSocket, SD_BOTH);	//송수신 both 중단
		setsockopt(acceptSocket, SOL_SOCKET, SO_LINGER, (const char*) & lingerOpt, sizeof(linger));
		closesocket(acceptSocket);

		status = CONNECTION_STATUS::READY;
	}
};