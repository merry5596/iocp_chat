#include "ChatManager.h"

const UINT16 SERVER_PORT = 11021;
const char* SERVER_IP = "127.0.0.1";

int main(void) {
	ChatManager chatManager;

	cout << "채팅 프로그램에 참여하시겠습니까? (y/n)" << endl;
	char answer;
	cin >> answer;
	cin.ignore();
	if (answer == 'n') {
		return 0;
	}

	bool ret = chatManager.Init(SERVER_PORT, SERVER_IP);
	while (ret == false) {
		cout << "서버에 접속할 수 없습니다. 재시도하시겠습니까?(y/n)" << endl;
		cin >> answer;
		cin.ignore();
		if (answer == 'n') {
			return 0;
		}
		ret = chatManager.Init(SERVER_PORT, SERVER_IP);
	}

	cout << "채팅 프로그램을 시작합니다~" << endl;
	chatManager.Start();

	char name[NAME_LEN];
	do {
		cout << "닉네임: ";
		cin >> name;
		cin.ignore();
		ret = chatManager.Login(name);
	} while (ret == false);

	while (true)
	{
		INT16 roomNum;
		cout << "입장할 방 번호(랜덤 입장은 0, 서버 종료는 -1): " << endl;
		cin >> roomNum;
		cin.ignore();
		if (roomNum == -1) {
			break;
		}

		ret = chatManager.EnterRoom(roomNum);
		while (ret == false) {
			continue;
		}

		string msg;
		cout << "채팅 시작(퇴장은 /leave)" << endl;
		getline(cin, msg);
		while (msg != "/leave") {
			//bool ret = chatManager.EchoMsg(msg);
			bool ret = chatManager.ChatMsg(msg);
			if (ret) {
				cout << "---------------전송됨---------------" << endl;
			}
			getline(cin, msg);
		}

		chatManager.LeaveRoom();
	}

	chatManager.End();
	cout << "프로그램 종료." << endl;
	return 0;
}
