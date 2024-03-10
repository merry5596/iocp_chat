#pragma once

#include "Define.h"
#include <vector>

enum class PACKET_ID : UINT16 {
	ECHO_REQUEST = 1,
};

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

struct EchoPacket : PACKET_HEADER {
	//std::vector<uint8_t> msg;
	char msg[BUFFER_SIZE - HEADER_SIZE];
};
#pragma pack(pop)