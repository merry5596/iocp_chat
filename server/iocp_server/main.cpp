#include "ChatServer.h"

#include <wchar.h>
#include <iostream>
using namespace std;

int main(void) {
	ChatServer server;
	server.Init();
	server.Start();

	printf("Chat Server Start! ");
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