#pragma once
#include "Define.h"
#include "ClientNetwork.h"

class ChatManager : public ClientNetwork {
private:
public:
	void SendMsg(string msg) {
		MessagePacket msgPkt;
		msgPkt.packetID = (UINT16)PACKET_ID::ECHO_REQUEST;
		msgPkt.packetSize = sizeof(MessagePacket);
		CopyMemory(msgPkt.msg, msg.c_str(), sizeof(msg));
		ClientNetwork::Send(msgPkt);
	}
};