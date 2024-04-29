#pragma once
#include "ChatManager.h"
#include "NotifyManager.h"

using namespace ChatClientLib;

namespace ChatClientLibWrapper {
	shared_ptr<spdlog::logger> file_logger;

	extern "C" __declspec(dllexport) ChatManager* UseChatManager() {
		if (file_logger == nullptr) {
			file_logger = spdlog::basic_logger_mt("file_logger", "logs/logforcs.txt");
			spdlog::set_default_logger(file_logger);
			spdlog::set_level(spdlog::level::info);
		}
		spdlog::info("UseChatManager");
		return new ChatManager;
	}
	extern "C" __declspec(dllexport) void ExitChatManager(ChatManager* chatManager) {
		spdlog::info("ExitChatManager");
		chatManager->End();
		delete chatManager;
	}

	extern "C" __declspec(dllexport) bool Init(ChatManager * chatManager, const UINT16 SERVER_PORT, const char* SERVER_IP);
	extern "C" __declspec(dllexport) void Start(ChatManager * chatManager);
	extern "C" __declspec(dllexport) void End(ChatManager * chatManager);
	extern "C" __declspec(dllexport) void OnReceive(ChatManager * chatManager, char* data, UINT16 size, bool errflag, UINT32 err);
	extern "C" __declspec(dllexport) void OnSend(ChatManager * chatManager, char* data, UINT16 size, bool errflag, UINT32 err);
	extern "C" __declspec(dllexport) UINT16 Login(ChatManager * chatManager, const char* name);
	extern "C" __declspec(dllexport) UINT16 EnterRoom(ChatManager * chatManager, UINT16 roomNum);
	extern "C" __declspec(dllexport) UINT16 LeaveRoom(ChatManager * chatManager);
	extern "C" __declspec(dllexport) bool EchoMsg(ChatManager * chatManager, const char* msg);
	extern "C" __declspec(dllexport) bool ChatMsg(ChatManager * chatManager, const char* msg);
	extern "C" __declspec(dllexport) ChatClientLib::Notify GetNotify(ChatManager * chatManager);
	extern "C" __declspec(dllexport) UINT16 GetUserState(ChatManager * chatManager);
	extern "C" __declspec(dllexport) char* GetUserNickName(ChatManager * chatManager);
	extern "C" __declspec(dllexport) UINT16 GetUserRoomNum(ChatManager * chatManager);
}