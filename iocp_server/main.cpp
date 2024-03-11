#include "EchoServer.h"

#include <iostream>
using namespace std;

const UINT16 SERVER_PORT = 11021;
const UINT16 CLIENTPOOL_SIZE = 100;

int main(void) {
	EchoServer server;
	server.Init(SERVER_PORT, CLIENTPOOL_SIZE);
	server.Start();
	while (true) {
		std::string key;
		cout << "press \'exit\' to exit" << endl;
		cin >> key;
		if (key == "exit") {
			break;
		}
	}
	server.End();
	return 0;
}