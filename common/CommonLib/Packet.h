#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

const UINT16 NAME_LEN = 10;
const UINT16 ECHO_MSG_LEN = 252;
const UINT16 CHAT_MSG_LEN = 242;

enum class PACKET_ID : UINT16 {
	LOGIN_REQUEST = 1,
	ECHO_REQUEST = 2,
	ROOM_ENTER_REQUEST = 3,
	ROOM_LEAVE_REQUEST = 4,
	CHAT_REQUEST = 5,

	LOGIN_RESPONSE = 21,
	ECHO_RESPONSE = 22,
	ROOM_ENTER_RESPONSE = 23,
	ROOM_LEAVE_RESPONSE = 24,
	CHAT_RESPONSE = 25,

	ROOM_ENTER_NOTIFY = 43,
	ROOM_LEAVE_NOTIFY = 44,
	CHAT_NOTIFY = 45,
	

	DISCONNECT = 60,
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

struct DisconnectPacket : PACKET_HEADER {};

struct LoginRequestPacket : PACKET_HEADER {
	char name[NAME_LEN];
};

struct LoginResponsePacket : PACKET_HEADER {
	UINT16 result;
	char name[NAME_LEN];
};

struct RoomEnterRequestPacket : PACKET_HEADER {
	UINT16 roomNum;
};

struct RoomEnterResponsePacket : PACKET_HEADER {
	UINT16 result;
	UINT16 roomNum;
};

struct RoomEnterNotifyPacket : PACKET_HEADER {
	char name[NAME_LEN];
	UINT16 roomNum;
};

struct RoomLeaveRequestPacket : PACKET_HEADER {};

struct RoomLeaveResponsePacket : PACKET_HEADER {
	UINT16 result;
};

struct RoomLeaveNotifyPacket : PACKET_HEADER {
	char name[NAME_LEN];
	UINT16 roomNum;
};

struct EchoRequestPacket : PACKET_HEADER {
	char msg[ECHO_MSG_LEN];
};

struct EchoResponsePacket : PACKET_HEADER {
	char msg[ECHO_MSG_LEN];
};

struct ChatRequestPacket : PACKET_HEADER {
	char msg[CHAT_MSG_LEN];
};

struct ChatResponsePacket : PACKET_HEADER {
	UINT16 result;
};

struct ChatNotifyPacket : PACKET_HEADER {
	char sender[NAME_LEN];
	char msg[CHAT_MSG_LEN];
};

#pragma pack(pop)