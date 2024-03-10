#pragma once

#include "Define.h"
#include <vector>

enum class PACKET_ID : UINT16 {
	ECHO_REQUEST = 1,
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