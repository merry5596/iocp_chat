#include "ChatManager.h"

const UINT16 SERVER_PORT = 11021;
const char* SERVER_IP = "127.0.0.1";

int main(void) {
	ChatManager chatManager;
	bool ret = chatManager.Init(SERVER_PORT, SERVER_IP);
	while (ret == false) {
		cout << "서버에 접속할 수 없습니다. 재시도하시겠습니까?(y/n)" << endl;
		char a;
		cin >> a;
		if (a == 'n') {
			return 0;
		}
		cin.ignore();
		ret = chatManager.Init(SERVER_PORT, SERVER_IP);
	}

	chatManager.Start();

	char name[NAME_LEN];
	do {
		cout << "닉네임: ";
		cin >> name;
		ret = chatManager.Login(name);
	} while (ret == false);

	cin.ignore();

	string msg;
	cout << "채팅 시작(종료는 exit)" << endl;
	while (true) {
		getline(cin, msg);
		if (msg == "exit") {
			break;
		}
		bool ret = chatManager.EchoMsg(msg);
		if (ret) {
			cout << "---------------전송됨---------------" << endl;
		}
		Sleep(3);
	}

	chatManager.End();
	cout << "프로그램 종료." << endl;
	return 0;
}
