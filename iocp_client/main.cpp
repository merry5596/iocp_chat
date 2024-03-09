#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>

#pragma comment(lib, "ws2_32")
using namespace std;

const UINT16 SERVER_PORT = 11021;
const char* SERVER_IP = "127.0.0.1";
const UINT16 BUFFER_SIZE = 256;

bool isRecvRun;


void RecvThread(SOCKET s) {
	isRecvRun = true;
	while (isRecvRun) {
		char buf[BUFFER_SIZE];
		int ret = recv(s, buf, sizeof(buf), 0);
		if (ret == -1) {
			cout << "수신 실패." << endl;
			continue;
		}

		cout << "Server response: " << buf << endl;
	}
}

int main(void) {

	cout << "접속하시겠습니까? (y/n)" << endl;
	char a;
	cin >> a;
	if (a == 'n') {
		return 0;
	}
	WSADATA wsaData;
	int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != 0) {
		cout << "[ERROR]WSAStartup() error: " << WSAGetLastError() << endl;
		return 0;
	}

	printf("[SUCCESS][1/3]WSAStartup\n");
	
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET) {
		cout << "[ERROR]socket() error: " << GetLastError() << endl;
		return 0;
	}
	printf("[SUCCESS][2/3]socket creation\n");

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SERVER_PORT);
//	addr.sin_addr.s_addr = inet_pton(AF_INET, SERVER_IP, &ipv4_addr);
	addr.sin_addr.s_addr = inet_addr(SERVER_IP);

	ret = connect(s, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN));
	if (ret != 0) {
		cout << "[ERROR]connect() error: " << GetLastError() << endl;
		return 0;
	}

	printf("[SUCCESS][3/3]connect\n");

	thread recvThread = thread([&]() { RecvThread(s);  });

	string msg;

	while (1) {
		cout << "메시지를 입력하세요(종료는 exit): ";
		cin >> msg;
		if (msg == "exit") {
			isRecvRun = false;
			break;
		}
		int ret = send(s, msg.c_str(), sizeof(msg), 0);
		if (ret == -1) {
			printf("전송에 실패했습니다.\n");
			continue;
		}
		Sleep(3);
	}

	closesocket(s);
	if (recvThread.joinable()) {
		recvThread.join();
	}

	WSACleanup();

	cout << "서버 종료." << endl;
	return 0;
}
