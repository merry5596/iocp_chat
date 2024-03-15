#include "ChatManager.h"

const UINT16 SERVER_PORT = 11021;
const char* SERVER_IP = "127.0.0.1";

int main(void) {
	ChatManager chatManager;
	bool ret = chatManager.Init(SERVER_PORT, SERVER_IP);
	while (ret == false) {
		cout << "서버에 접속할 수 없습니다. 재시도하시겠습니까?(y/n)" << endl;
		char answer;
		cin >> answer;
		if (answer == 'n') {
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


	string msg;
	cout << "채팅 시작(종료는 exit)" << endl;

	cin.ignore();	//버퍼 비우기
	getline(cin, msg);
	while (msg != "exit") {
		bool ret = chatManager.EchoMsg(msg);
		if (ret) {
			cout << "---------------전송---------------" << endl;
		}
		getline(cin, msg);
	}

	chatManager.End();
	cout << "프로그램 종료." << endl;
	return 0;
}
