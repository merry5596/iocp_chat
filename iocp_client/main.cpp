#include "ClientNetwork.h"

const UINT16 SERVER_PORT = 11021;
const char* SERVER_IP = "127.0.0.1";

bool isRecvRun;

int main(void) {
	cout << "접속 하시겠습니까? (y/n)" << endl;
	char a;
	cin >> a;
	if (a == 'n') {
		return 0;
	}
	cin.ignore();

	ClientNetwork network;
	network.Init(SERVER_PORT, SERVER_IP);
	network.Start();	//Recv 시작

	string msg;
	cout << "메시지를 입력하세요(종료는 exit): ";
	while (1) {
		getline(cin, msg);
		if (msg == "exit") {
			isRecvRun = false;
			break;
		}
		network.Send(msg);
		Sleep(3);
	}
	network.End();
	cout << "프로그램 종료." << endl;
	return 0;
}
