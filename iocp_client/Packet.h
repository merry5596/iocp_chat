#pragma once

#include "Define.h"
#include <vector>

const UINT16 NAME_LEN = 10;

enum class PACKET_ID : UINT16 {
	ECHO_REQUEST = 1,

	LOGIN_REQUEST = 11,
	CHAT_REQUEST = 12,

	LOGIN_RESPONSE = 22,
	CHAT_RESPONSE = 23,
};

#pragma pack(push, 1)
struct PACKET_HEADER {
	UINT16 packetID;
	UINT16 packetSize;
};
const UINT16 HEADER_SIZE = sizeof(PACKET_HEADER);

struct ResponsePacket : PACKET_HEADER {
	UINT16 result;
};

struct LoginRequestPacket : PACKET_HEADER {
	char name[NAME_LEN];
};

struct EchoPacket : PACKET_HEADER {
	char msg[BUFFER_SIZE - HEADER_SIZE];
};
#pragma pack(pop)