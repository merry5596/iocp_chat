#pragma once
#include "Define.h"

#include <thread>
#include <errno.h>

class ClientNetwork {
private:
	SOCKET sock;
	thread recvThread;

public:
	bool isRecvRun;
	~ClientNetwork() {
		WSACleanup();
	}

	bool Init(const UINT16 SERVER_PORT, const char* SERVER_IP) {
		//WinSock 사용
		WSADATA wsaData;
		int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (ret != 0) {
			cout << "[ERROR]WSAStartup() error: " << WSAGetLastError() << endl;
			return false;
		}

		//소켓 생성
		if (CreateSocket() == false) {
			return false;
		}

		//연결
		return Connect(SERVER_PORT, SERVER_IP);
	}

	void Start() {
		//Receive 스레드 시작
		recvThread = thread([&]() { RecvThread(); });
	}

	void End() {
		isRecvRun = false;
		closesocket(sock);
		if (recvThread.joinable()) {
			recvThread.join();
		}
	}

	bool Refresh(const UINT16 SERVER_PORT, const char* SERVER_IP) {
		End();
		bool ret = CreateSocket();
		if (ret == false) {
			return false;
		}
		
		ret = Connect(SERVER_PORT, SERVER_IP);
		if (ret == false) {
			return false;
		}
		Start();
		return true;
	}

	bool SendData(char* data, UINT16 size) {
		bool errflag = false;
		int ret = send(sock, data, size, 0);
		if (ret <= 0) {
			errflag = true;
		}
		OnSend(data, ret, errflag, errno);
		return errflag ? false : true;
	}

	void RecvThread() {
		isRecvRun = true;
		char buf[BUFFER_SIZE];
		bool errflag;
		while (isRecvRun) {
			errflag = false;
			ZeroMemory(buf, BUFFER_SIZE);
			int recvBytes = recv(sock, buf, BUFFER_SIZE, 0);
			if (recvBytes <= 0 || errno == ECONNRESET || errno == ENOTCONN) {
				errflag = true;
				isRecvRun = false;
			}
			OnReceive(buf, recvBytes, errflag, errno);
		}
	}

	virtual void OnSend(char* data, UINT16 size, bool errflag, UINT32 err) {}
	virtual void OnReceive(char* data, UINT16 size, bool errflag, UINT32 err) {}

private:
	bool CreateSocket() {
		sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sock == INVALID_SOCKET) {
			cout << "[ERROR]socket() error: " << GetLastError() << endl;
			return false;
		}
		return true;
	}

	bool Connect(const UINT16 SERVER_PORT, const char* SERVER_IP) {
		SOCKADDR_IN addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(SERVER_PORT);
		inet_pton(AF_INET, SERVER_IP, &addr.sin_addr.s_addr);
		auto ret = connect(sock, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN));
		if (ret != 0) {
			cout << "[ERROR]connect() error: " << GetLastError() << endl;
			return false;
		}
		return true;
	}

};