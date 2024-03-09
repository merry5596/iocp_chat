#include "EchoServer.h"

#include <iostream>
using namespace std;

const UINT16 SERVER_PORT = 11021;
const UINT16 CLIENTPOOL_SIZE = 100;

int main(void) {
	EchoServer server;
	server.Init(SERVER_PORT);
	server.Start(CLIENTPOOL_SIZE);
	while (true) {
		std::string key;
		cout << "exit를 입력하면 서버 종료됨" << endl;
		cin >> key;
		if (key == "exit") {
			break;
		}
	}
	server.End();
	cout << "서버 종료" << endl;
	return 0;
}