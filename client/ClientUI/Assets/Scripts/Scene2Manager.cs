using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;


public class Scene2Manager : MonoBehaviour
{
    public Text textRoomNum;

    public ScrollRect scrollView;
    public Text chatObj;
    public InputField inputMsg;

    ChatManager chatManager;

    void Start()
    {
        Debug.Log("Scene2Manager::Start");
        chatManager = ChatManager.instance;

        textRoomNum.text = chatManager.GetUserRoomNum().ToString();
        inputMsg.onEndEdit.AddListener(OnPressEnterToSend);
        
        inputMsg.ActivateInputField();
    }

    void Update()
    {
        Notification ntf = chatManager.GetNotify();
        
        if (ntf.packetID != 0) {
            if (ntf.packetID == (ushort)PACKET_ID.CHAT_NOTIFY) {
                Debug.Log(ntf.name + " : " + ntf.msg);
                Text chat = Instantiate(chatObj, scrollView.content);
                chat.text = ntf.name + " : " + ntf.msg;
            }
            else if (ntf.packetID == (ushort)PACKET_ID.ROOM_ENTER_NOTIFY) {
                Debug.Log(ntf.name + "님이 방에 입장");
                Text chat = Instantiate(chatObj, scrollView.content);
                chat.text = "<color=#ffffff>" + ntf.name + "님이 방에 입장합니다.</color>";
            }
            else if (ntf.packetID == (ushort)PACKET_ID.ROOM_LEAVE_NOTIFY) {
                Debug.Log(ntf.name + "님이 방에서 퇴장");
                Text chat = Instantiate(chatObj, scrollView.content);
                chat.text = "<color=#ffffff>" + ntf.name + "님이 방에서 퇴장합니다.</color>";
            }
            else if (ntf.packetID == (ushort)PACKET_ID.DISCONNECT) {
                Debug.Log("서버와의 통신이 종료되었습니다.");
                Text chat = Instantiate(chatObj, scrollView.content);
                chat.text = "<color=#ffffff>서버와의 통신이 종료되었습니다.</color>";
            }

            LayoutRebuilder.ForceRebuildLayoutImmediate(scrollView.content);
            scrollView.verticalScrollbar.value = 0;
        }
    }

    public void OnClickBtnRoomLeave() {
        ushort ret = chatManager.LeaveRoom();
        if (ret == (ushort)ERROR_CODE.NONE) {
            SceneManager.LoadScene(1);
        } else {
            Debug.Log("퇴장 실패");
        }
    }

    void SendMsg(string msg) {
        if (msg != "") {
            bool ret = chatManager.ChatMsg(msg);
            if (ret) {
                Text chat = Instantiate(chatObj, scrollView.content);
                chat.text = "<color=#C9FF30>" + chatManager.GetUserNickName() + " : " + msg + "</color>";
                inputMsg.text = "";
                LayoutRebuilder.ForceRebuildLayoutImmediate(scrollView.content);
                scrollView.verticalScrollbar.value = 0;
            } else {
                Debug.Log("전송 실패");
            }
        } else {
            Debug.Log("보낼 게 없어요!");
        }
    }

    public void OnClickBtnSend() {
        string msg = inputMsg.text;
        SendMsg(msg);
    }
    
    public void OnPressEnterToSend(string msg) {
        if (Input.GetKeyDown(KeyCode.Return))
        {
            SendMsg(msg);
            inputMsg.ActivateInputField();
        }
    }
}
