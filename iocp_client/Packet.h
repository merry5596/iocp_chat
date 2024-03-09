#pragma once

#include "Define.h"
#include <vector>

enum class PACKET_ID : UINT16 {
	ECHO_REQUEST = 0,
};

struct PacketInfo {
	UINT32 clientIndex = 0;
	UINT16 packetID = 0;
	UINT16 packetSize = 0;
	char packetData[10];
	//std::vector<uint8_t> packetData;
};

#pragma pack(push, 1)
struct PACKET_HEADER {
	UINT16 packetID;
	UINT16 packetSize;
};
const UINT16 HEADER_SIZE = sizeof(PACKET_HEADER);

struct EchoPacket : PACKET_HEADER {
	char msg[BUFFER_SIZE - HEADER_SIZE];
};
#pragma pack(pop)