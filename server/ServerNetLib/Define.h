#pragma once
#pragma comment(lib, "ws2_32")
#pragma comment (lib, "mswsock.lib")

#include <winsock2.h>
#include <ws2tcpip.h>	//ipv6
#include <mswsock.h>	//AcceptEx

#define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace ServerNetLib {
	enum class IOOperation {
		ACCEPT,
		RECV,
		SEND,
	};

	struct WSAOverlappedEx {
		WSAOVERLAPPED wsaOverlapped;
		IOOperation operation;
		UINT32 clientIndex;
		WSABUF wsaBuf;
	};
}
