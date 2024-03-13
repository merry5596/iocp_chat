#include "ChatManager.h"

const UINT16 SERVER_PORT = 11021;
const char* SERVER_IP = "127.0.0.1";

int main(void) {
	cout << "접속 하시겠습니까? (y/n)" << endl;
	char a;
	cin >> a;
	if (a == 'n') {
		return 0;
	}
	cin.ignore();

	ChatManager chatManager;
	chatManager.Init(SERVER_PORT, SERVER_IP);
	chatManager.Start();

	string msg;
	cout << "채팅 시작(종료는 exit)" << endl;
	while (1) {
		getline(cin, msg);
		if (msg == "exit") {
			break;
		}
		chatManager.SendMsg(msg);
		Sleep(3);
	}
	chatManager.End();
	cout << "프로그램 종료." << endl;
	return 0;
}
