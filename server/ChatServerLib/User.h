#pragma once
#include "Define.h"
#include "Packet.h"

#include <mutex>
#include <iostream>
using namespace std;

namespace ChatServerLib {

	const UINT16 PACKET_BUFFER_SIZE = 8096;

	enum class USER_STATE : UINT16 {
		NONE = 0,
		LOGIN = 1,
		ROOM = 2,
	};

	class User {
	private:
		UINT32 clientIndex;
		char name[NAME_LEN];
		UINT16 state;
		UINT16 roomNum;

		char packetBuffer[PACKET_BUFFER_SIZE];
		UINT16 readPos;
		UINT16 writePos;
		mutex pktMtx;

	public:
		User(UINT32 index) : clientIndex(index), writePos(0), readPos(0) {}
		~User() {}

		void Reset() {
			if (state == (UINT16)USER_STATE::ROOM) {
				LeaveRoom();
			}
			if (state == (UINT16)USER_STATE::LOGIN) {
				SetLogout();
			}
		}

		void SetLogin(char* name) {
			strcpy_s(this->name, NAME_LEN, name);
			state = (UINT16)USER_STATE::LOGIN;
		}

		void SetLogout() {
			ZeroMemory(name, NAME_LEN);
			state = (UINT16)USER_STATE::NONE;
			this->roomNum = 0;
			writePos = 0;
			readPos = 0;
		}

		void EnterRoom(UINT16 roomNum) {
			this->roomNum = roomNum;
			this->state = (UINT16)USER_STATE::ROOM;
		}

		UINT16 LeaveRoom() {
			UINT16 leaveRoomNum = roomNum;
			this->roomNum = 0;
			this->state = (UINT16)USER_STATE::LOGIN;
			return leaveRoomNum;
		}

		UINT16 GetRoomNum() const {
			return roomNum;
		}

		char* GetName() {
			return name;
		}

		UINT16 GetState() {
			return state;
		}

		bool SetPacket(char* data, UINT16 size) {
			bool isOverwrite = false;
			UINT16 tryCnt = 0;

			pktMtx.lock();
			if (writePos + size >= PACKET_BUFFER_SIZE) {	//이 쓰기로 버퍼가 넘친다면
				if (readPos != 0) {	//당길 수 있으면 안읽은 데이터부터 버퍼 맨 앞으로 당겨오기
					auto noReadDataSize = writePos - readPos;
					CopyMemory(&packetBuffer[0], &packetBuffer[readPos], noReadDataSize);
					writePos = noReadDataSize;
					readPos = 0;
				}
			}
			if (writePos + size >= PACKET_BUFFER_SIZE) {	//당겼는데도 여전히 넘친다면 조금뒤에 시도
				isOverwrite = true;
				tryCnt = 5;
			}
			pktMtx.unlock();
			
			if (isOverwrite) {
				while (tryCnt > 0) {
					this_thread::sleep_for(chrono::milliseconds(100));
					pktMtx.lock();
					if (writePos + size < PACKET_BUFFER_SIZE) {
						isOverwrite = false;
						break;
					}
					pktMtx.unlock();
					tryCnt--;
				}
			}
			if (isOverwrite) {
				spdlog::warn("{} : overwrite으로 인해 종료 예정", clientIndex);
				return false;
			}

			pktMtx.lock();
			//이어서 쓰기
			CopyMemory(&packetBuffer[writePos], data, size);
			writePos += size;
			pktMtx.unlock();

			return true;
		}

		PacketInfo GetPacket() {
			PacketInfo pktInfo;
			PACKET_HEADER* header;
			UINT16 noReadDataSize;
			{
				lock_guard<mutex> lock(pktMtx);
				noReadDataSize = writePos - readPos;
				if (noReadDataSize < HEADER_SIZE) {	//헤더조차 다 안 온 상태
					return PacketInfo();
				}
				header = (PACKET_HEADER*)&packetBuffer[readPos];
				if (noReadDataSize < header->packetSize) {	//전체 패킷 덜 옴
					//printf("body 덜옴. 와야할 패킷사이즈는 %d 인데, 읽고자 하는 버퍼 사이즈는 %d\n", header->packetSize, noReadDataSize);
					return PacketInfo();
				}

				pktInfo.packetData = &packetBuffer[readPos];
				readPos += header->packetSize;
			}

			pktInfo.clientIndex = clientIndex;
			pktInfo.packetID = header->packetID;
			pktInfo.packetSize = header->packetSize;

			return pktInfo;
		}
	};

}