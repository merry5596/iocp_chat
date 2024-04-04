#include "ChatManager.h"

const UINT16 SERVER_PORT = 11021;
const char* SERVER_IP = "127.0.0.1";
const int num_threads = 1000;
const int roomMax = 500;

int cnt = 0;
bool isAllFinished = false;
mutex mtx;

void DummyThreadEternalChat(UINT16 nameNum, UINT16 roomNum) {
	ChatClientLib::ChatManager chatManager;

	bool ret = chatManager.Init(SERVER_PORT, SERVER_IP);
	if (ret == false) {
		spdlog::error("[ERROR]chatManager Init()");
		return;
	}

	chatManager.Start();

	//닉네임 설정(로그인)
	string name;
	UINT16 result;
	name = to_string(nameNum);
	this_thread::sleep_for(chrono::milliseconds(500));
	result = chatManager.Login(name.c_str());
	if (result != (UINT16)ERROR_CODE::NONE) {
		spdlog::error("Login() error: {}", result);
	}

	this_thread::sleep_for(chrono::milliseconds(500));
	result = chatManager.EnterRoom(roomNum);
	if (result != (UINT16)ERROR_CODE::NONE) {
		spdlog::error("EnterRoom() error: {}", result);
	}

	//채팅 입력
	string msg = "hello";
	while (true) {
		this_thread::sleep_for(chrono::milliseconds(500));
		chatManager.ChatMsg(msg.c_str());
	}
}

void DummyThreadInTime(UINT16 nameNum, UINT16 roomNum) {
	ChatClientLib::ChatManager chatManager;
	bool ret = chatManager.Init(SERVER_PORT, SERVER_IP);
	if (ret == false) {
		spdlog::error("[ERROR]chatManager Init()");
		return;
	}

	chatManager.Start();

	//닉네임 설정(로그인)
	string name;
	UINT16 result;
	name = to_string(nameNum);
	this_thread::sleep_for(chrono::milliseconds(500));
	result = chatManager.Login(name.c_str());
	if (result != (UINT16)ERROR_CODE::NONE) {
		spdlog::error("Login() error: {}", result);
	}

	this_thread::sleep_for(chrono::milliseconds(500));
	result = chatManager.EnterRoom(roomNum);
	if (result != (UINT16)ERROR_CODE::NONE) {
		spdlog::error("EnterRoom() error: {}", result);
	}

	//채팅 입력
	string msg = "hello";
	for (int i = 0; i < 10; i++) {
		this_thread::sleep_for(chrono::milliseconds(500));
		chatManager.ChatMsg(msg.c_str());
	}
	this_thread::sleep_for(chrono::seconds(1));
	chatManager.LeaveRoom();
	this_thread::sleep_for(chrono::milliseconds(500));
	chatManager.End();
	{
		lock_guard<mutex> lock(mtx);
		cnt++;
		if (cnt == num_threads) {
			isAllFinished = true;
		}
	}
}

int main(void) {
	auto file_logger = spdlog::basic_logger_mt("file_logger", "logs/logfile.txt");
	spdlog::set_default_logger(file_logger);
	spdlog::set_level(spdlog::level::info);

	spdlog::info("Dummy threads start!");
	vector<thread> dummyThreadPool;
	for (int i = 1; i <= num_threads; i++) {
		//dummyThreadPool.emplace_back([i]() { DummyThreadInTime(i, (i % roomMax) + 1); });
		dummyThreadPool.emplace_back([i]() { DummyThreadEternalChat(i, (i % roomMax) + 1); });
	}

	while (!isAllFinished);
	spdlog::info("Dummy threads end...");

	for (int i = 0; i < num_threads; i++) {
		if (dummyThreadPool[i].joinable()) {
			dummyThreadPool[i].join();
		}
	}

	return 0;
}