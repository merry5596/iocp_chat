#pragma once

struct PACKET_HEADER {
	int packetID;
};

struct EchoReqPacket : PACKET_HEADER {
	char* msg;
	int len;
};