#include "TcpNetwork.h"

namespace ClientNetLib {

	TcpNetwork::~TcpNetwork() {
		WSACleanup();
	}

	bool TcpNetwork::Init(const UINT16 SERVER_PORT, const char* SERVER_IP) {
		//WinSock 사용
		WSADATA wsaData;
		int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (ret != 0) {
			spdlog::error("[ERROR]WSAStartup() error: {}", WSAGetLastError());
			//cout << "[ERROR]WSAStartup() error: " << WSAGetLastError() << endl;
			return false;
		}

		//소켓 생성
		if (CreateSocket() == false) {
			return false;
		}

		//연결
		return Connect(SERVER_PORT, SERVER_IP);
	}

	void TcpNetwork::Start() {
		//Receive 스레드 시작
		recvThread = thread([&]() { RecvThread(); });
	}

	void TcpNetwork::End() {
		if (isRecvRun) {
			isRecvRun = false;
			closesocket(sock);
			if (recvThread.joinable()) {
				recvThread.join();
			}
		}
	}

	bool TcpNetwork::Refresh(const UINT16 SERVER_PORT, const char* SERVER_IP) {
		End();

		//소켓 생성
		if (CreateSocket() == false) {
			return false;
		}

		//연결
		if (Connect(SERVER_PORT, SERVER_IP) == false) {
			return false;
		}

		Start();
		return true;
	}

	bool TcpNetwork::SendData(char* data, UINT16 size) {
		bool errflag = false;
		int ret = send(sock, data, size, 0);
		if (ret <= 0) {
			errflag = true;
		}
		OnSend(data, ret, errflag, errno);
		return errflag ? false : true;
	}

	void TcpNetwork::RecvThread() {
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

	bool TcpNetwork::CreateSocket() {
		sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sock == INVALID_SOCKET) {
			spdlog::error("[ERROR]socket() error: {}", GetLastError());
			//cout << "[ERROR]socket() error: " << GetLastError() << endl;
			return false;
		}
		return true;
	}

	bool TcpNetwork::Connect(const UINT16 SERVER_PORT, const char* SERVER_IP) {
		SOCKADDR_IN addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(SERVER_PORT);
		inet_pton(AF_INET, SERVER_IP, &addr.sin_addr.s_addr);
		auto ret = connect(sock, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN));
		if (ret != 0) {
			spdlog::error("[ERROR]connect() error: {}", GetLastError());
			//cout << "[ERROR]connect() error: " << GetLastError() << endl;
			return false;
		}
		return true;
	}

}