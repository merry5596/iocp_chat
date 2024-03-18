#pragma once

#include <Windows.h>
#include <vector>

const UINT16 NAME_LEN = 10;
const UINT16 ECHO_MSG_LEN = 252;
const UINT16 CHAT_MSG_LEN = 242;

enum class PACKET_ID : UINT16 {
	LOGIN_REQUEST = 1,
	CHAT_REQUEST = 2,

	ECHO = 20,

	LOGIN_RESPONSE = 21,
	CHAT_RESPONSE = 22,

	CHAT_NOTIFY = 30,

	DISCONNECT = 40,
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
	char msg[ECHO_MSG_LEN];
};

struct ChatRequestPacket : PACKET_HEADER {
	char msg[CHAT_MSG_LEN];
};

struct ChatNotifyPacket : PACKET_HEADER {
	char sender[NAME_LEN];
	char msg[CHAT_MSG_LEN];
};

#pragma pack(pop)