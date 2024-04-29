#include "pch.h"
#include "ChatClientLibWrapper.h"

namespace ChatClientLibWrapper {
	bool Init(ChatManager* chatManager, const UINT16 SERVER_PORT, const char* SERVER_IP) {
		return chatManager->Init(SERVER_PORT, SERVER_IP);
	}
	void Start(ChatManager* chatManager) {
		chatManager->Start();
	}
	void End(ChatManager* chatManager) {
		chatManager->End();
	}
	void OnReceive(ChatManager* chatManager, char* data, UINT16 size, bool errflag, UINT32 err) {
		chatManager->OnReceive(data, size, errflag, err);
	}
	void OnSend(ChatManager* chatManager, char* data, UINT16 size, bool errflag, UINT32 err) {
		chatManager->OnSend(data, size, errflag, err);
	}
	UINT16 Login(ChatManager* chatManager, const char* name) {
		return chatManager->Login(name);
	}
	UINT16 EnterRoom(ChatManager* chatManager, UINT16 roomNum) {
		return chatManager->EnterRoom(roomNum);
	}
	UINT16 LeaveRoom(ChatManager* chatManager) {
		return chatManager->LeaveRoom();
	}
	bool EchoMsg(ChatManager* chatManager, const char* msg) {
		return chatManager->EchoMsg(msg);
	}
	bool ChatMsg(ChatManager* chatManager, const char* msg) {
		return chatManager->ChatMsg(msg);
	}
	ChatClientLib::Notify GetNotify(ChatManager* chatManager) {
		return chatManager->GetNotify();
	}
	UINT16 GetUserState(ChatManager* chatManager) {
		return chatManager->GetUserState();
	}
	char* GetUserNickName(ChatManager* chatManager) {
		return chatManager->GetUserNickName();
	}
	UINT16 GetUserRoomNum(ChatManager* chatManager) {
		return chatManager->GetUserRoomNum();
	}
}