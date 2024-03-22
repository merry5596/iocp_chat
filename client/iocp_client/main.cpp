#include "ChatManager.h"

const UINT16 SERVER_PORT = 11021;
const char* SERVER_IP = "127.0.0.1";

int main(void) {
	ChatManager chatManager;

	//접속
	string answer;
	do {
		cout << "채팅 프로그램에 참여하시겠습니까? (y/n)" << endl;
		cin >> answer;
		cin.ignore();
	} while (answer != "n" && answer != "y");

	if (answer == "n") {
		return 0;
	}
	bool ret = chatManager.Init(SERVER_PORT, SERVER_IP);

	//접속 재시도
	while (ret == false) {
		cout << "서버에 접속할 수 없습니다. 재시도하시겠습니까?(y/n)" << endl;
		do {
			cin >> answer;
			cin.ignore();
		} while (answer != "n" && answer != "y");

		if (answer == "n") {
			return 0;
		}
		ret = chatManager.Init(SERVER_PORT, SERVER_IP);
	}
	cout << "채팅 프로그램을 시작합니다~" << endl;
	chatManager.Start();

	//닉네임 설정(로그인)
	string name;
	do {
		cout << "닉네임: ";
		cin >> name;
		cin.ignore();
		ret = chatManager.Login(name.c_str());
	} while (ret == false);

	//방 입장
	while (true)
	{
		string answer;
		INT16 roomNum;
		bool isValid = false;
		cout << "입장할 방 번호(랜덤 입장은 0, 서버 종료는 -1): " << endl;
		do {
			cin >> answer;
			cin.ignore();

			try {
				roomNum = std::stoi(answer); // 문자열을 정수로 변환
				isValid = true;
			}
			catch (...) {
				cout << "유효한 번호를 입력하세요." << endl;
				//isValid = false;
			}
		} while (!isValid);

		if (roomNum == -1) {
			break;
		}

		ret = chatManager.EnterRoom(roomNum);
		while (ret == false) {
			continue;
		}

		//채팅 입력
		string msg;
		cout << "채팅 시작(퇴장은 /leave)" << endl;
		getline(cin, msg);
		while (msg != "/leave") {
			//bool ret = chatManager.EchoMsg(msg);
			bool ret = chatManager.ChatMsg(msg.c_str());
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
