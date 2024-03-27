#include "ChatServer.h"

#include <wchar.h>
#include <iostream>
using namespace std;

int main(void) {
	auto file_logger = spdlog::basic_logger_mt("file_logger", "logs/logfile.txt");
	spdlog::set_default_logger(file_logger);
	spdlog::set_level(spdlog::level::debug);

	ChatServerLib::ChatServer server;
	server.Init();
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