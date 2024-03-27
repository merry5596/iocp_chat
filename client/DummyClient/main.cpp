#include "ChatManager.h"

#include <ctime>
#include <random>

const UINT16 SERVER_PORT = 11021;
const char* SERVER_IP = "127.0.0.1";

bool isDummyRun;

void DummyThread() {
	ChatClientLib::ChatManager chatManager;
	bool ret = chatManager.Init(SERVER_PORT, SERVER_IP);
	if (ret == false) {
		cout << "[ERROR]chatManager Init()" << endl;
		return;
	}

	chatManager.Start();

	//닉네임 설정(로그인)
	string name;
	UINT16 result;
	do {
		// 난수 생성
		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<> dis(1, 400);
		int randomNumber = dis(gen);
		name = to_string(randomNumber);
		this_thread::sleep_for(chrono::seconds(1));
		result = chatManager.Login(name.c_str());
	} while (result != (UINT16)ERROR_CODE::NONE);

	//방 입장
	while (isDummyRun)
	{
		INT16 roomNum;
		// 난수 생성
		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<> dis(1, 40);
		roomNum = dis(gen);
		this_thread::sleep_for(chrono::seconds(1));
		result = chatManager.EnterRoom(roomNum);
		if (result != (UINT16)ERROR_CODE::NONE) {
			continue;
		}

		//채팅 입력
		//난수 생성
		uniform_int_distribution<> dis2(1, 40);
		int chatCnt = dis2(gen);
		string msg;
		for (int i = 0; i < chatCnt; i++) {
			msg = to_string(i);
			this_thread::sleep_for(chrono::seconds(3));
			chatManager.ChatMsg(msg.c_str());
		}
		this_thread::sleep_for(chrono::milliseconds(500));
		chatManager.LeaveRoom();
	}

	chatManager.End();
}

int main(void) {
	vector<thread> dummyThreadPool;
	isDummyRun = true;
	for (int i = 0; i < 100; i++) {
		dummyThreadPool.emplace_back([]() { DummyThread(); });
		this_thread::sleep_for(chrono::milliseconds(200));
	}
	this_thread::sleep_for(chrono::seconds(5));
	for (int i = 0; i < 100; i++) {
		dummyThreadPool.emplace_back([]() { DummyThread(); });
		this_thread::sleep_for(chrono::milliseconds(200));
	}

	int a;
	cin >> a;
	isDummyRun = false;
	for (int i = 0; i < 5; i++) {
		if (dummyThreadPool[i].joinable()) {
			dummyThreadPool[i].join();
		}
	}

	return 0;
}