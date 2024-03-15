#pragma once
#include "Define.h"
#include "Packet.h"
#include <mutex>

const UINT16 PACKET_BUFFER_SIZE = 8096;

class User {
private:
	UINT32 clientIndex;
	char packetBuffer[PACKET_BUFFER_SIZE];
	UINT16 writePos;
	UINT16 readPos;
	mutex mtx;
public:
	User(UINT32 index) : clientIndex(index), writePos(0), readPos(0) {}
	~User() {}
	void SetPacket(char* data, UINT16 size) {
		lock_guard<mutex> lock(mtx);
		if (writePos + size >= PACKET_BUFFER_SIZE) {	//이 쓰기로 버퍼가 넘친다면 우선 안읽은 데이터를 버퍼 앞으로 복사하고 이어서 쓰기
			auto noReadDataSize = writePos - readPos;
			CopyMemory(&packetBuffer[0], &packetBuffer[readPos], noReadDataSize);
			writePos = noReadDataSize;
			readPos = 0;
		}
		CopyMemory(&packetBuffer[writePos], data, size);
		writePos += size;
	}

	PacketInfo GetPacket() {
		PacketInfo pktInfo;
		PACKET_HEADER* header;
		{
			lock_guard<mutex> lock(mtx);
			auto noReadDataSize = writePos - readPos;
			if (noReadDataSize < HEADER_SIZE) {	//헤더조차 다 안 온 상태
				return PacketInfo();
			}
			header = (PACKET_HEADER*)&packetBuffer[readPos];
			if (noReadDataSize < header->packetSize) {	//전체 패킷 덜 옴
				//printf("body 덜옴. 와야할 패킷사이즈는 %d 인데, 읽고자 하는 버퍼 사이즈는 %d\n", header->packetSize, noReadDataSize);
				return PacketInfo();
			}

			pktInfo.packetData = &packetBuffer[readPos];
			readPos += noReadDataSize;
		}
		pktInfo.clientIndex = clientIndex;
		pktInfo.packetID = header->packetID;
		pktInfo.packetSize = header->packetSize;

		return pktInfo;
	}
};