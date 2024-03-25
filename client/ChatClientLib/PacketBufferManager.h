#pragma once

#include "ErrorCode.h"
#include "Packet.h"
#include "UserInfo.h"
#include "NotifyManager.h"

#include <thread>
#include <mutex>
#include <unordered_map>
#include <iostream>
using namespace std;

namespace ChatClientLib {

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

		unique_ptr<UserInfo> userInfo;

		//응답패킷처리 완료시 플래그
		bool isLoginResCompleted;
		UINT16 loginResult;

		bool isRoomEnterResCompleted;
		UINT16 roomEnterResult;

		bool isRoomLeaveResCompleted;
		UINT16 roomLeaveResult;

		NotifyManager* notifyManager;

		//패킷처리 함수
		typedef void (PacketBufferManager::* ProcessFunction)(char*);
		unordered_map<UINT16, ProcessFunction> processFuncDic;
	public:
		void Init(NotifyManager* notifyManager);
		void Start();
		void End();

		void OnDataReceive(char* data, UINT16 size);

		//응답 패킷 처리 완료 대기 함수들 (ChatManager에서 패킷 요청 후 호출)
		UINT16 GetLoginResult();
		UINT16 GetRoomEnterResult();
		UINT16 GetRoomLeaveResult();

	private:
		void SetPacket(char* data, UINT16 size);

		void PacketThread();
		bool ProcessBuffer();
		//패킷종류별 처리
		void ProcessLoginResponse(char* pkt);
		void ProcessRoomEnterResponse(char* pkt);
		void ProcessRoomLeaveResponse(char* pkt);
		void ProcessEchoResponse(char* pkt);
		void ProcessChatResponse(char* pkt);
		void ProcessChatNotify(char* pkt);
		void ProcessRoomEnterNotify(char* pkt);
		void ProcessRoomLeaveNotify(char* pkt);
	};

}