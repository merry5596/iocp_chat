using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;
using System.Runtime.InteropServices;

public class Scene1Manager : MonoBehaviour
{
    public InputField inputRoomNum;
    public Text textFailedEnterRoom;

    ChatManager chatManager;

    void Start() {
        Debug.Log("Scene1Manager::Start");
        chatManager = ChatManager.instance;
        inputRoomNum.onEndEdit.AddListener(OnPressEnterToEnterRoom);
        inputRoomNum.ActivateInputField();
    }

    void Update()
    {
        
    }

    void EnterRoom(string roomNum) {
        ushort roomNumShort;
        if (ushort.TryParse(roomNum, out roomNumShort)) {
            if (roomNumShort == 0) {
                textFailedEnterRoom.text = "존재하지 않는 방 번호입니다.";
            }
            ushort ret = chatManager.EnterRoom(roomNumShort);
            if (ret == (ushort)ERROR_CODE.NONE) {
                SceneManager.LoadScene(2);
            } else {
                inputRoomNum.ActivateInputField();
                if (ret == (ushort)ERROR_CODE.USER_STATE_ERROR) {
                    textFailedEnterRoom.text = "방에 입장할 수 있는 상태가 아닙니다.";
                }
                else if (ret == (ushort)ERROR_CODE.NO_EMPTY_ROOM) {
                    textFailedEnterRoom.text = "빈 방이 없습니다.";
                }
                else if (ret == (ushort)ERROR_CODE.INVALID_ROOM_NUM) {
                    textFailedEnterRoom.text = "존재하지 않는 방 번호입니다.";
                }
                else if (ret == (ushort)ERROR_CODE.ROOM_FULL) {
                    textFailedEnterRoom.text = "이미 인원이 찬 방입니다.";
                }
            }
        } else {
            Debug.Log("틀린 형식입니당");
            textFailedEnterRoom.text = "틀린 형식입니다. 숫자만 입력하세요.";
            inputRoomNum.ActivateInputField();
        }
    }
    
    public void OnClickBtnRoomEnter() {
        string roomNum = inputRoomNum.text;
        EnterRoom(roomNum);
    }

    void OnPressEnterToEnterRoom(string roomNum) {
        if (Input.GetKeyDown(KeyCode.Return))
        {
            EnterRoom(roomNum);
        }
    }
    
    public void OnClickBtnRandomRoom() {
        ushort ret = chatManager.EnterRoom(0);
        if (ret == (ushort)ERROR_CODE.NONE) {
            SceneManager.LoadScene(2);
        }
    }
}
