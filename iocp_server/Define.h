#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <mswsock.h>

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