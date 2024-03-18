#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>	//ipv6
#include <mswsock.h>	//AcceptEx
#pragma comment(lib, "ws2_32")
#pragma comment (lib, "mswsock.lib")

#include <iostream>
using namespace std;

const UINT16 THREADPOOL_SIZE = 4;
const UINT16 BUFFER_SIZE = 256;

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