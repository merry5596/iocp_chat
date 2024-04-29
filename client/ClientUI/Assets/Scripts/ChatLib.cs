using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;

public struct Notify {
	public ushort packetID;
    public string name;
    public string msg;
};

public class ChatLib : MonoBehaviour
{
    [DllImport ("ChatClientLibWrapper")]
    public static extern IntPtr UseChatManager();
    [DllImport ("ChatClientLibWrapper")]
    public static extern void ExitChatManager(IntPtr chatManager);
    [DllImport ("ChatClientLibWrapper")]
    public static extern bool Init(IntPtr chatManager, ushort SERVER_PORT, string SERVER_IP);
    [DllImport ("ChatClientLibWrapper")]
    public static extern void Start(IntPtr chatManager);
    [DllImport ("ChatClientLibWrapper")]
    public static extern void End(IntPtr chatManager);
    [DllImport ("ChatClientLibWrapper")]
    public static extern void OnReceive(IntPtr chatManager, string data, ushort size, bool errflag, uint err);
    [DllImport ("ChatClientLibWrapper")]
    public static extern void OnSend(IntPtr chatManager, string data, ushort size, bool errflag, uint err);
    [DllImport ("ChatClientLibWrapper")]
    public static extern ushort Login(IntPtr chatManager, string name);
    [DllImport ("ChatClientLibWrapper")]
    public static extern ushort EnterRoom(IntPtr chatManager, ushort roomNum);
    [DllImport ("ChatClientLibWrapper")]
    public static extern ushort LeaveRoom(IntPtr chatManager);
    [DllImport ("ChatClientLibWrapper")]
    public static extern bool EchoMsg(IntPtr chatManager, string msg);
    [DllImport ("ChatClientLibWrapper")]
    public static extern bool ChatMsg(IntPtr chatManager, string msg);
    [DllImport ("ChatClientLibWrapper")]
    public static extern Notification GetNotify(IntPtr chatManager);
    [DllImport ("ChatClientLibWrapper")]
    public static extern ushort GetUserState(IntPtr chatManager);
    [DllImport ("ChatClientLibWrapper")]
    public static extern IntPtr GetUserNickName(IntPtr chatManager);
    [DllImport ("ChatClientLibWrapper")]
    public static extern ushort GetUserRoomNum(IntPtr chatManager);
}
