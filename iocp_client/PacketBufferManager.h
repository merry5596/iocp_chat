#pragma once
#include "Define.h"
#include "Packet.h"
#include "UserInfo.h"

#include <thread>

const UINT16 PACKET_BUFFER_SIZE = 8096;

//recv packet을 처리하는 버퍼 관련
class PacketBufferManager {
private:
	char packetBuffer[PACKET_BUFFER_SIZE];
	UINT16 writePos;
	UINT16 readPos;

	thread packetThread;
	bool isPacketRun;

	ResponsePacket* loginResPkt;
public:
	void Init() {
		writePos = 0;
		readPos = 0;
		isPacketRun = true;
		loginResPkt = nullptr;
	}

	void Start() {
		packetThread = thread([&]() {
			PacketThread();
		});
	}
	void End() {
		isPacketRun = false;
		if (packetThread.joinable()) {
			packetThread.join();
		}
	}

	void PacketThread() {
		while (isPacketRun) {
			auto pkt = GetPacket();
			if (pkt != nullptr) {
				ProcessPacket(pkt);
			}
			else {
				this_thread::sleep_for(chrono::milliseconds(32));
			}
		}
	}

	void OnDataReceive(char* data, UINT16 size) {
		//버퍼에 데이터를 넣는다.
		SetPacket(data, size);
	}

	void SetPacket(char* data, UINT16 size) {
		if (writePos + size >= PACKET_BUFFER_SIZE) {	//이 쓰기로 버퍼가 넘친다면 우선 안읽은 데이터를 버퍼 앞으로 복사하고 이어서 쓰기
			auto noReadDataSize = writePos - readPos;
			CopyMemory(&packetBuffer[0], &packetBuffer[readPos], noReadDataSize);
			writePos = noReadDataSize;
			readPos = 0;
		}
		CopyMemory(&packetBuffer[writePos], data, size);
		writePos += size;
		printf("쓰기완료. writePos: %d, readPos: %d", writePos, readPos);
	}

	ResponsePacket* GetPacket() {
		PACKET_HEADER* header;
		auto noReadDataSize = writePos - readPos;
		if (noReadDataSize < HEADER_SIZE) {	//헤더조차 다 안 온 상태
			//cout << "[ERROR]헤더안옴" << endl;
			return nullptr;
		}
		header = (PACKET_HEADER*)&packetBuffer[readPos];
		if (header->packetID < 12) {
			cout << "[ERROR]유효하지 않은 응답 패킷" << endl;
			return nullptr;
		}

		if (noReadDataSize < header->packetSize) {	//전체 패킷 덜 옴
			//printf("body 덜옴. 와야할 패킷사이즈는 %d 인데, 읽고자 하는 버퍼 사이즈는 %d\n", header->packetSize, noReadDataSize);
			return nullptr;
		}

		ResponsePacket* resPkt = (ResponsePacket*)&packetBuffer[readPos];
		readPos += sizeof(ResponsePacket);

		printf("읽기완료. writePos: %d, readPos: %d\n", writePos, readPos);
		return resPkt;
	}

	void ProcessPacket(ResponsePacket* pkt) {
		if (pkt->packetID == 22) {	//로그인 응답
			loginResPkt = pkt;
		}
	}

	UINT16 GetLoginResponse() {
		while (loginResPkt == nullptr) {
			this_thread::sleep_for(chrono::milliseconds(1));
		}
		auto result = loginResPkt->result;
		loginResPkt = nullptr;
		return result;
	}

};