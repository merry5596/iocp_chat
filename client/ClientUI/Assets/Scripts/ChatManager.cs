using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using System.IO;

enum PACKET_ID  {
    ROOM_ENTER_NOTIFY = 43,
    ROOM_LEAVE_NOTIFY = 44,
    CHAT_NOTIFY = 45,
    
    DISCONNECT = 60,
};

enum ERROR_CODE {
    NONE = 0,
    ALREADY_EXIST_NAME = 1,
    USER_STATE_ERROR = 2,
    NO_EMPTY_ROOM = 3,
    INVALID_ROOM_NUM = 4,
    ROOM_FULL = 5
}

public struct Notification {
    public ushort packetID;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = ChatManager.NAME_LEN)]
    public string name;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = ChatManager.CHAT_MSG_LEN)]
    public string msg;
};

public class ChatManager : MonoBehaviour
{
    public const ushort NAME_LEN = 10;
    public const ushort CHAT_MSG_LEN = 242;

    public static ChatManager instance;
    
    public IntPtr cmPtr;
    StreamWriter writer;

    void Awake() {
        Debug.Log("ChatManager::Awake");
        if (instance == null) {
            instance = this;
            if (cmPtr == IntPtr.Zero) {
                cmPtr = ChatLib.UseChatManager();
                Debug.Log("ChatManager::ChatLib.UseChatManager");
            }
            writer = new StreamWriter("Logs/logUnity.txt", true);
            writer.WriteLine("UseChatManager");
            DontDestroyOnLoad(gameObject);
        } else {
            Destroy(gameObject);
        }
    }

    void OnApplicationQuit() {
        Debug.Log("ChatManager::OnApplicationQuit");
        if (cmPtr != IntPtr.Zero) {
            ChatLib.ExitChatManager(cmPtr);
            cmPtr = IntPtr.Zero;
            Debug.Log("ChatManager::ChatLib.ExitChatManager");
        }
        writer.WriteLine("ExitChatManager");
        writer.Close();
    }

    public bool Connect(ushort SERVER_PORT, string SERVER_IP) {
        bool ret = ChatLib.Init(cmPtr, SERVER_PORT, SERVER_IP);
        if (ret) {
            Debug.Log("SUCCESS : ChatManager::ChatLib.Init");
            ChatLib.Start(cmPtr);
            Debug.Log("ChatManager::ChatLib.Start");
        } else {
            Debug.Log("FAILED : ChatManager::ChatLib.Init");
            return false;
        }
        return true;
    }

    public ushort Login(string name) {
        ushort ret = ChatLib.Login(cmPtr, name);
        if (ret == (ushort)ERROR_CODE.NONE) {
            Debug.Log("SUCCESS : ChatManager::ChatLib.Login");
        } else {
            Debug.Log("FAILED : ChatManager::ChatLib.Login : " + ret);
        }
        return ret;
    }

    public ushort EnterRoom(ushort roomNum) {
        ushort ret = ChatLib.EnterRoom(cmPtr, roomNum);
        if (ret == (ushort)ERROR_CODE.NONE) {
            Debug.Log("SUCCESS : ChatManager::ChatLib.EnterRoom(" + GetUserRoomNum() + ")");
        } else {
            Debug.Log("FAILED : ChatManager::ChatLib.EnterRoom(" + GetUserRoomNum() + ") : " + ret);
        }
        return ret;
    }

    public ushort GetUserRoomNum() {
        ushort ret = ChatLib.GetUserRoomNum(cmPtr);
        Debug.Log("ChatManager::ChatLib.GetUserRoomNum : " + ret);
        return ret;
    }

    public ushort LeaveRoom() {
        ushort ret = ChatLib.LeaveRoom(cmPtr);
        if (ret == (ushort)ERROR_CODE.NONE) {
            Debug.Log("SUCCESS : ChatManager::ChatLib.LeaveRoom");
        } else {
            Debug.Log("FAILED : ChatManager::ChatLib.LeaveRoom : " + ret);
        }
        return ret;
    }

    
    public bool ChatMsg(string msg) {
        bool ret = ChatLib.ChatMsg(cmPtr, msg);
        if (ret) {
            Debug.Log("SUCCESS : ChatManager::ChatLib.ChatMsg");
        } else {
            Debug.Log("FAILED : ChatManager::ChatLib.ChatMsg : " + ret);
        }
        return ret;
    }

    public Notification GetNotify() {
        return ChatLib.GetNotify(cmPtr);
    }

    public string GetUserNickName() {
        IntPtr namePtr = ChatLib.GetUserNickName(cmPtr);
        string name = Marshal.PtrToStringAnsi(namePtr);
        Debug.Log(name);
        return name;
    }
}
