using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;
using System.Runtime.InteropServices;
using System.IO;

public class Scene0Manager : MonoBehaviour
{
    public GameObject groupConnect;
    public Text textFailedConnect;

    public GameObject groupLogin;
    public InputField inputName;
    public Text textFailedLogin;

    ChatManager chatManager;

    void Start() {
        Debug.Log("Scene0Manager::Start");
        chatManager = ChatManager.instance;

        inputName.onEndEdit.AddListener(OnPressEnterToLogin);
    }

    public void OnClickBtnConnect() {
        bool ret = chatManager.Connect(11021, "127.0.0.1");
        if (ret) {
            groupConnect.SetActive(false);
            groupLogin.SetActive(true);
            inputName.ActivateInputField();
        } else {
            textFailedConnect.text = "서버에 접속할 수 없습니다. 재시도해주세요.";
        }
    }

    void Login(string name) {
        ushort ret = chatManager.Login(name);
        if (ret == (ushort)ERROR_CODE.NONE) {
            groupLogin.SetActive(false);
            SceneManager.LoadScene(1);
        } else if (ret == (ushort)ERROR_CODE.ALREADY_EXIST_NAME) {
            textFailedLogin.text = "이미 사용중인 닉네임입니다.";
            inputName.ActivateInputField();
        }
    }
    
    public void OnClickBtnLogin() {
        string name = inputName.text;
        if (name == "") {
            textFailedLogin.text = "닉네임을 한 글자 이상 입력해주세요";
            inputName.ActivateInputField();
        } else {
            Login(name);
        }
    }
    
    public void OnPressEnterToLogin(string name) {
        if (Input.GetKeyDown(KeyCode.Return))
        {
            Login(name);
        }
    }

}
