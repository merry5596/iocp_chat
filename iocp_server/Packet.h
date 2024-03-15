#pragma once

#include "Define.h"
#include <vector>

const UINT16 NAME_LEN = 10;

enum class PACKET_ID : UINT16 {
	DISCONNECT = 10,

	LOGIN_REQUEST = 1,
	CHAT_REQUEST = 2,

	ECHO_REQUEST = 20,

	LOGIN_RESPONSE = 21,
	CHAT_RESPONSE = 22,
};

//for server
struct PacketInfo {
	UINT32 clientIndex;
	UINT16 packetID;
	UINT16 packetSize;
	char* packetData;
};

#pragma pack(push, 1)
struct PACKET_HEADER {
	UINT16 packetID;
	UINT16 packetSize;
};
const UINT16 HEADER_SIZE = sizeof(PACKET_HEADER);

struct SystemPacket : PACKET_HEADER {

};

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