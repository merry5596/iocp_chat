#include "ChatManager.h"

const UINT16 SERVER_PORT = 11021;
const char* SERVER_IP = "127.0.0.1";

int main(void) {
	ChatManager chatManager;

	cout << "채팅 프로그램에 참여하시겠습니까? (y/n)" << endl;
	char answer;
	cin >> answer;
	if (answer == 'n') {
		return 0;
	}
	cin.ignore();

	bool ret = chatManager.Init(SERVER_PORT, SERVER_IP);
	while (ret == false) {
		cout << "서버에 접속할 수 없습니다. 재시도하시겠습니까?(y/n)" << endl;
		cin >> answer;
		if (answer == 'n') {
			return 0;
		}
		cin.ignore();
		ret = chatManager.Init(SERVER_PORT, SERVER_IP);
	}

	cout << "채팅 프로그램을 시작합니다~" << endl;
	chatManager.Start();

	char name[NAME_LEN];
	do {
		cout << "닉네임: ";
		cin >> name;
		ret = chatManager.Login(name);
	} while (ret == false);

	UINT16 roomNum;
	cout << "입장할 방 번호(랜덤 입장은 0): " << endl;
	cin.ignore();
	cin >> roomNum;
	ret = chatManager.EnterRoom(roomNum);
	while (ret == false) {
		cin.ignore();
		cin >> roomNum;
		ret = chatManager.EnterRoom(roomNum);
	}

	string msg;
	cout << "채팅 시작(종료는 /exit)" << endl;
	cin.ignore();	//버퍼 비우기
	getline(cin, msg);
	while (msg != "/exit") {
		//bool ret = chatManager.EchoMsg(msg);
		bool ret = chatManager.ChatMsg(msg);
		if (ret) {
			cout << "---------------전송됨---------------" << endl;
		}
		getline(cin, msg);
	}

	chatManager.End();
	cout << "프로그램 종료." << endl;
	return 0;
}
