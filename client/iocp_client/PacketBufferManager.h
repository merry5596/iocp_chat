#pragma once

#include "ErrorCode.h"
#include "Packet.h"
#include "UserInfo.h"

#include <thread>
#include <mutex>
#include <unordered_map>
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

	LoginResponsePacket* loginResPkt;
	RoomEnterResponsePacket* roomEnterResPkt;
	RoomLeaveResponsePacket* roomLeaveResPkt;

	typedef void (PacketBufferManager::* ProcessFunction)(char*);
	unordered_map<UINT16, ProcessFunction> processFuncDic;
public:
	void Init() {
		writePos = 0;
		readPos = 0;
		loginResPkt = nullptr;
		roomEnterResPkt = nullptr;

		processFuncDic = unordered_map<UINT16, ProcessFunction>();
		processFuncDic[(UINT16)PACKET_ID::LOGIN_RESPONSE] = &PacketBufferManager::ProcessLoginResponse;
		processFuncDic[(UINT16)PACKET_ID::ROOM_ENTER_RESPONSE] = &PacketBufferManager::ProcessRoomEnterResponse;
		processFuncDic[(UINT16)PACKET_ID::ROOM_LEAVE_RESPONSE] = &PacketBufferManager::ProcessRoomLeaveResponse;
		processFuncDic[(UINT16)PACKET_ID::ECHO_RESPONSE] = &PacketBufferManager::ProcessEchoResponse;
		processFuncDic[(UINT16)PACKET_ID::CHAT_RESPONSE] = &PacketBufferManager::ProcessChatResponse;
		processFuncDic[(UINT16)PACKET_ID::CHAT_NOTIFY] = &PacketBufferManager::ProcessChatNotify;

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

	UINT16 GetRoomEnterResponse() {
		while (true) {
			if (roomEnterResPkt != nullptr) {
				auto result = roomEnterResPkt->result;
				roomEnterResPkt = nullptr;
				return result;
			}
			this_thread::sleep_for(chrono::milliseconds(1));
		}
	}

	UINT16 GetRoomLeaveResponse() {
		while (true) {
			if (roomLeaveResPkt != nullptr) {
				auto result = roomLeaveResPkt->result;
				roomLeaveResPkt = nullptr;
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
			if (header->packetID < (UINT16)PACKET_ID::LOGIN_RESPONSE) {
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

		auto iter = processFuncDic.find(header->packetID);
		if (iter != processFuncDic.end())
		{
			(this->*(iter->second))(&packetBuffer[pktStartPos]);
		}
		/*
		if (header->packetID == (UINT16)PACKET_ID::ECHO_RESPONSE) {
			EchoResponsePacket* echoPkt = (EchoResponsePacket*)&packetBuffer[pktStartPos];
			ProcessEchoPacket(echoPkt);
		}
		else if (header->packetID == (UINT16)PACKET_ID::CHAT_NOTIFY) {
			ChatNotifyPacket* chatPkt = (ChatNotifyPacket*)&packetBuffer[pktStartPos];
			ProcessChatPacket(chatPkt);
		}
		else if (header->packetID == (UINT16)PACKET_ID::ROOM_ENTER_RESPONSE) {

		}
		else {
			ResponsePacket* resPkt = (ResponsePacket*)&packetBuffer[pktStartPos];
			ProcessResponsePacket(resPkt);
		}*/

		//printf("읽기완료. writePos: %d, readPos: %d\n", writePos, readPos);
		return true;
	}

	void ProcessLoginResponse(char* pkt) {
		LoginResponsePacket* resPkt = reinterpret_cast<LoginResponsePacket*>(pkt);
		loginResPkt = resPkt;
	}

	void ProcessRoomEnterResponse(char* pkt) {
		RoomEnterResponsePacket* resPkt = reinterpret_cast<RoomEnterResponsePacket*>(pkt);
		roomEnterResPkt = resPkt;
	}

	void ProcessRoomLeaveResponse(char* pkt) {
		RoomLeaveResponsePacket* resPkt = reinterpret_cast<RoomLeaveResponsePacket*>(pkt);
		roomLeaveResPkt = resPkt;
	}

	void ProcessEchoResponse(char* pkt) {
		EchoResponsePacket* resPkt = reinterpret_cast<EchoResponsePacket*>(pkt);
		cout << "Server : " << resPkt->msg << endl;;
	}

	void ProcessChatResponse(char* pkt) {
		ChatResponsePacket* resPkt = reinterpret_cast<ChatResponsePacket*>(pkt);
		if (resPkt->result != ERROR_CODE::NONE) {
			cout << "ProcessChatResponse() Error: " << resPkt->result << endl;
		}
	}

	void ProcessChatNotify(char* pkt) {
		ChatNotifyPacket* ntfPkt = reinterpret_cast<ChatNotifyPacket*>(pkt);
		cout << ntfPkt->sender << " : " << ntfPkt->msg << endl;
	}

};