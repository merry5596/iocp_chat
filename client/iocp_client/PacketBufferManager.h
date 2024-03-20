#pragma once

#include "Packet.h"
#include "UserInfo.h"

#include <thread>
#include <mutex>
#include <iostream>
using namespace std;

const UINT16 PACKET_BUFFER_SIZE = 8096;

//recv packet을 처리하는 버퍼 관련
class PacketBufferManager {
private:
	char packetBuffer[PACKET_BUFFER_SIZE];
	UINT16 writePos;
	UINT16 readPos;
	mutex mtx;

	thread packetThread;
	bool isPacketRun;

	ResponsePacket* loginResPkt;
public:
	void Init() {
		writePos = 0;
		readPos = 0;
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

	void OnDataReceive(char* data, UINT16 size) {
		SetPacket(data, size);	//버퍼에 데이터를 넣는다.
	}

	UINT16 GetLoginResponse() {	//ChatManager::Login() 에서 호출
		while (true) {
			if (loginResPkt != nullptr) {
				auto result = loginResPkt->result;
				loginResPkt = nullptr;
				return result;
			}
			this_thread::sleep_for(chrono::milliseconds(1));
		}
	}

private:
	void PacketThread() {
		isPacketRun = true;
		while (isPacketRun) {
			bool ret = ProcessBuffer();
			if (ret == false) {
				this_thread::sleep_for(chrono::milliseconds(32));
			}
		}
	}

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
		//printf("쓰기완료. writePos: %d, readPos: %d", writePos, readPos);
	}

	bool ProcessBuffer() {
		PACKET_HEADER* header;
		UINT16 pktStartPos;

		lock_guard<mutex> lock(mtx);
		{
			auto noReadDataSize = writePos - readPos;
			if (noReadDataSize < HEADER_SIZE) {	//헤더조차 다 안 온 상태
				//cout << "[ERROR]헤더안옴" << endl;
				//return PacketInfo();
				return false;
			}
			header = (PACKET_HEADER*)&packetBuffer[readPos];
			if (header->packetID < (UINT16)PACKET_ID::ECHO) {
				cout << "[ERROR]ProcessBuffer(): 응답 패킷이 아님.." << endl;
				//return PacketInfo();
				return false;
			}

			if (noReadDataSize < header->packetSize) {	//전체 패킷 덜 옴
				//printf("body 덜옴. 와야할 패킷사이즈는 %d 인데, 읽고자 하는 버퍼 사이즈는 %d\n", header->packetSize, noReadDataSize);
				//return PacketInfo();
				return false;
			}
			pktStartPos = readPos;
			readPos += header->packetSize;
		}
		
		if (header->packetID == (UINT16)PACKET_ID::ECHO) {
			EchoPacket* echoPkt = (EchoPacket*)&packetBuffer[pktStartPos];
			ProcessEchoPacket(echoPkt);
		}
		else if (header->packetID == (UINT16)PACKET_ID::CHAT_NOTIFY) {
			ChatNotifyPacket* chatPkt = (ChatNotifyPacket*)&packetBuffer[pktStartPos];
			ProcessChatPacket(chatPkt);
		}
		else {
			ResponsePacket* resPkt = (ResponsePacket*)&packetBuffer[pktStartPos];
			ProcessResponsePacket(resPkt);
		}

		//printf("읽기완료. writePos: %d, readPos: %d\n", writePos, readPos);
		return true;
	}
	void ProcessChatPacket(ChatNotifyPacket* pkt) {
		cout << pkt->sender << " : " << pkt->msg << endl;
	}

	void ProcessEchoPacket(EchoPacket* pkt) {
		cout << "Server : " << pkt->msg << endl;;
	}

	void ProcessResponsePacket(ResponsePacket* pkt) {
		if (pkt->packetID == (UINT16)PACKET_ID::LOGIN_RESPONSE) {	//로그인 응답
			loginResPkt = pkt;
		}
	}


};