#pragma once
#include "Define.h"
#include "Packet.h"

#include <thread>
#include <iostream>
using namespace std;

class ClientNetwork {
private:
	SOCKET sock;
	thread recvThread;
	bool isRecvRun;
public:
	ClientNetwork() {}
	~ClientNetwork() {
		WSACleanup();
	}

	bool Init(UINT16 SERVER_PORT, const char* SERVER_IP) {
		//WinSock 사용
		WSADATA wsaData;
		int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (ret != 0) {
			cout << "[ERROR]WSAStartup() error: " << WSAGetLastError() << endl;
			return false;
		}

		//socket 생성
		sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sock == INVALID_SOCKET) {
			cout << "[ERROR]socket() error: " << GetLastError() << endl;
			return false;
		}

		//연결
		SOCKADDR_IN addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(SERVER_PORT);
		//	addr.sin_addr.s_addr = inet_pton(AF_INET, SERVER_IP, &ipv4_addr);
		addr.sin_addr.s_addr = inet_addr(SERVER_IP);
		ret = connect(sock, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN));
		if (ret != 0) {
			cout << "[ERROR]connect() error: " << GetLastError() << endl;
			return false;
		}

		printf("[SUCCESS]connection completed.\n");
		return true;
	}

	void Start() {
		//Receive 스레드 시작
		recvThread = thread([&]() { RecvThread(); });
	}

	void Send(string msg) {
		int ret = send(sock, msg.c_str(), sizeof(msg), 0);
		if (ret == -1) {
			printf("[FAILED]전송에 실패했습니다.\n");
		}
	}

	void End() {
		isRecvRun = false;
		closesocket(sock);
		if (recvThread.joinable()) {
			recvThread.join();
		}
	}


	void RecvThread() {
		isRecvRun = true;
		char buf[BUFFER_SIZE];
		while (isRecvRun) {
			ZeroMemory(buf, BUFFER_SIZE);
			int ret = recv(sock, buf, sizeof(buf), 0);
			if (ret == -1) {
				if (isRecvRun == false) {
					break;
				}
				cout << "수신 실패." << endl;
				continue;
			}

			cout << "Server response: " << buf << endl;
		}
	}
};