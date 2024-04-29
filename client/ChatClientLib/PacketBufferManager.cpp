#include "PacketBufferManager.h"

namespace ChatClientLib {

	void PacketBufferManager::Init(NotifyManager* notifyManager, UserInfo* userInfo) {
		writePos = 0;
		readPos = 0;
		this->notifyManager = notifyManager;
		this->userInfo = userInfo;

		processFuncDic = unordered_map<UINT16, ProcessFunction>();
		processFuncDic[(UINT16)PACKET_ID::LOGIN_RESPONSE] = &PacketBufferManager::ProcessLoginResponse;
		processFuncDic[(UINT16)PACKET_ID::ROOM_ENTER_RESPONSE] = &PacketBufferManager::ProcessRoomEnterResponse;
		processFuncDic[(UINT16)PACKET_ID::ROOM_LEAVE_RESPONSE] = &PacketBufferManager::ProcessRoomLeaveResponse;
		processFuncDic[(UINT16)PACKET_ID::ECHO_RESPONSE] = &PacketBufferManager::ProcessEchoResponse;
		processFuncDic[(UINT16)PACKET_ID::CHAT_RESPONSE] = &PacketBufferManager::ProcessChatResponse;
		processFuncDic[(UINT16)PACKET_ID::CHAT_NOTIFY] = &PacketBufferManager::ProcessChatNotify;
		processFuncDic[(UINT16)PACKET_ID::ROOM_ENTER_NOTIFY] = &PacketBufferManager::ProcessRoomEnterNotify;
		processFuncDic[(UINT16)PACKET_ID::ROOM_LEAVE_NOTIFY] = &PacketBufferManager::ProcessRoomLeaveNotify;
	}

	void PacketBufferManager::Start() {
		packetThread = thread([&]() {
			PacketThread();
			});
	}

	void PacketBufferManager::End() {
		if (isPacketRun) {
			isPacketRun = false;
			if (packetThread.joinable()) {
				packetThread.join();
			}
		}
	}

	void PacketBufferManager::OnDataReceive(char* data, UINT16 size) {
		SetPacket(data, size);	//버퍼에 데이터를 넣는다.
	}

	//응답 패킷 처리 완료 대기 함수들 (ChatManager에서 패킷 요청 후 호출)
	UINT16 PacketBufferManager::GetLoginResult() {
		while (true) {
			if (isLoginResCompleted) {
				isLoginResCompleted = false;
				return loginResult;
			}
			this_thread::sleep_for(chrono::milliseconds(1));
		}
	}
	UINT16 PacketBufferManager::GetRoomEnterResult() {
		while (true) {
			if (isRoomEnterResCompleted) {
				isRoomEnterResCompleted = false;
				return roomEnterResult;
			}
			this_thread::sleep_for(chrono::milliseconds(1));
		}
	}
	UINT16 PacketBufferManager::GetRoomLeaveResult() {
		while (true) {
			if (isRoomLeaveResCompleted) {
				isRoomLeaveResCompleted = false;
				return roomLeaveResult;
			}
			this_thread::sleep_for(chrono::milliseconds(1));
		}
	}

	void PacketBufferManager::SetPacket(char* data, UINT16 size) {
		lock_guard<mutex> lock(mtx);
		if (writePos + size >= PACKET_BUFFER_SIZE) {	//이 쓰기로 버퍼가 넘친다면 우선 안읽은 데이터를 버퍼 앞으로 복사하고 이어서 쓰기
			auto noReadDataSize = writePos - readPos;
			CopyMemory(&packetBuffer[0], &packetBuffer[readPos], noReadDataSize);
			writePos = noReadDataSize;
			readPos = 0;
		}
		CopyMemory(&packetBuffer[writePos], data, size);
		writePos += size;
		spdlog::debug("쓰기완료. writePos: {0}, readPos: {1}", writePos, readPos);
		//printf("쓰기완료. writePos: %d, readPos: %d", writePos, readPos);
	}

	void PacketBufferManager::PacketThread() {
		isPacketRun = true;
		while (isPacketRun) {
			bool ret = ProcessBuffer();
			if (ret == false) {
				this_thread::sleep_for(chrono::milliseconds(32));
			}
		}
	}

	bool PacketBufferManager::ProcessBuffer() {
		PACKET_HEADER* header;
		UINT16 pktStartPos;

		lock_guard<mutex> lock(mtx);
		{
			auto noReadDataSize = writePos - readPos;
			if (noReadDataSize < HEADER_SIZE) {	//헤더조차 다 안 온 상태
				spdlog::debug("헤더 안옴");
				//cout << "[ERROR]헤더안옴" << endl;
				return false;
			}
			header = (PACKET_HEADER*)&packetBuffer[readPos];
			if (header->packetID < (UINT16)PACKET_ID::LOGIN_RESPONSE) {
				spdlog::error("응답 패킷이 아님");
				//cout << "[ERROR]ProcessBuffer(): 응답 패킷이 아님" << endl;
				return false;
			}

			if (noReadDataSize < header->packetSize) {	//전체 패킷 덜 옴
				spdlog::debug("바디 덜옴. 와야할 패킷사이즈는 {0} 인데, 읽고자 하는 버퍼 사이즈는 {1}\n", header->packetSize, noReadDataSize);
				//printf("body 덜옴. 와야할 패킷사이즈는 %d 인데, 읽고자 하는 버퍼 사이즈는 %d\n", header->packetSize, noReadDataSize);
				return false;
			}
			pktStartPos = readPos;
			readPos += header->packetSize;
		}

		//알맞는 처리함수 호출
		auto iter = processFuncDic.find(header->packetID);
		if (iter != processFuncDic.end())
		{
			(this->*(iter->second))(&packetBuffer[pktStartPos]);
		}
		spdlog::debug("읽기완료. writePos: {0}, readPos: {1}", writePos, readPos);
		//printf("읽기완료. writePos: %d, readPos: %d\n", writePos, readPos);
		return true;
	}

	//패킷종류별 처리
	void PacketBufferManager::ProcessLoginResponse(char* pkt) {
		LoginResponsePacket* resPkt = reinterpret_cast<LoginResponsePacket*>(pkt);
		if (resPkt->result == ERROR_CODE::ALREADY_EXIST_NAME) {
			spdlog::info("이미 존재하는 닉네임");
			//cout << "이미 존재하는 닉네임입니다." << endl;
		}
		if (resPkt->result == ERROR_CODE::NONE) {
			userInfo->Login(resPkt->name);
			spdlog::info("{}님 로그인 성공", resPkt->name);
			//cout << resPkt->name << "님 로그인 성공" << endl;
		}

		loginResult = resPkt->result;
		isLoginResCompleted = true;
	}

	void PacketBufferManager::ProcessRoomEnterResponse(char* pkt) {
		RoomEnterResponsePacket* resPkt = reinterpret_cast<RoomEnterResponsePacket*>(pkt);
		if (resPkt->result == ERROR_CODE::USER_STATE_ERROR) {
			spdlog::info("사용자 상태 이상. 입장 불가");
			//cout << "입장할 수 없는 상태입니다." << endl;
		}
		if (resPkt->result == ERROR_CODE::INVALID_ROOM_NUM) {
			spdlog::info("없는 방 번호");
			//cout << "없는 방 번호입니다." << endl;
		}
		if (resPkt->result == ERROR_CODE::ROOM_FULL) {
			spdlog::info("인원 모두 찼음");
			//cout << "해당 방은 인원이 모두 찼습니다." << endl;
		}
		if (resPkt->result == ERROR_CODE::NONE) {
			userInfo->EnterRoom(resPkt->roomNum);
			spdlog::info("{}번 방 입장", resPkt->roomNum);
			//cout << resPkt->roomNum << "번 방에 입장합니다." << endl;
		}

		roomEnterResult = resPkt->result;
		isRoomEnterResCompleted = true;
	}

	void PacketBufferManager::ProcessRoomLeaveResponse(char* pkt) {
		RoomLeaveResponsePacket* resPkt = reinterpret_cast<RoomLeaveResponsePacket*>(pkt);
		if (resPkt->result == ERROR_CODE::USER_STATE_ERROR) {
			spdlog::info("사용자 상태 이상. 퇴장 불가");
			//cout << "퇴장할 수 없는 상태입니다." << endl;
		}
		if (resPkt->result == ERROR_CODE::NONE) {
			userInfo->LeaveRoom();
			spdlog::info("방 퇴장");
			//cout << "방을 퇴장합니다." << endl;
		}
		roomLeaveResult = resPkt->result;
		isRoomLeaveResCompleted = true;
	}

	void PacketBufferManager::ProcessEchoResponse(char* pkt) {
		EchoResponsePacket* resPkt = reinterpret_cast<EchoResponsePacket*>(pkt);
		spdlog::info("Server : {}", resPkt->msg);
	}

	void PacketBufferManager::ProcessChatResponse(char* pkt) {
		ChatResponsePacket* resPkt = reinterpret_cast<ChatResponsePacket*>(pkt);
		if (resPkt->result != ERROR_CODE::NONE) {
			spdlog::error("[ERROR]ProcessChatResponse() Error: {}", resPkt->result);
		}
	}

	void PacketBufferManager::ProcessChatNotify(char* pkt) {
		ChatNotifyPacket* ntfPkt = reinterpret_cast<ChatNotifyPacket*>(pkt);
		spdlog::info("{0} : {1}", ntfPkt->name, ntfPkt->msg);
		notifyManager->AddChatNotify(ntfPkt->name, ntfPkt->msg);
	}

	void PacketBufferManager::ProcessRoomEnterNotify(char* pkt) {
		RoomEnterNotifyPacket* ntfPkt = reinterpret_cast<RoomEnterNotifyPacket*>(pkt);
		spdlog::info("{}님 방 입장", ntfPkt->name);
		//cout << ntfPkt->name << "님이 방에 입장하셨습니다." << endl;
		notifyManager->AddRoomEnterNotify(ntfPkt->name);
	}

	void PacketBufferManager::ProcessRoomLeaveNotify(char* pkt) {
		RoomLeaveNotifyPacket* ntfPkt = reinterpret_cast<RoomLeaveNotifyPacket*>(pkt);
		spdlog::info("{}님 방 퇴장", ntfPkt->name);
		//cout << ntfPkt->name << "님이 방에서 퇴장하셨습니다." << endl;
		notifyManager->AddRoomLeaveNotify(ntfPkt->name);
	}

}