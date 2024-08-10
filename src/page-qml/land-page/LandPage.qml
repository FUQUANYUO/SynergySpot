import QtQuick 2.15
import QtQuick.Layouts
import QtQuick.Window 2.15
import QtQuick.Controls
import FluentUI 1.0

//登录界面
FluWindow{

    //窗口基本属性
    width: 386
    height: 509
    minimumWidth: 386
    minimumHeight: 509
    maximumWidth: 386
    maximumHeight: 509
    fitsAppBarWindows: true

    // //后端对象
    // property var loginBackEnd : LoginBackEnd{}

    //账号相关属性
    property string ssAccount: ""                   // 账号
    property string ssPassword: ""                  // 密码
    property string ssAvatar: "qrc:/land-page/rc-page/img/avatar.jpg"     // 默认头像路径

    //背景
    AnimatedImage{
        id: loginBackground
        width: parent.width
        height: parent.height
        source: "qrc:/land-page/rc-page/img/backgroundImage.gif"
    }
    FluAcrylic{
        target:loginBackground
        width: parent.width
        height: parent.height
        anchors.centerIn: parent
    }


    //菜单栏
    Row
    {
        anchors.right: parent.right
        //设置
        FluIconButton{
            anchors.right: parent.right
            anchors.rightMargin: 35
            iconSource:FluentIcons.Settings
            onClicked: {
                // -前端：转跳设置界面-
            }
        }
        //关闭
        FluIconButton{
            anchors.right: parent.right
            iconSource:FluentIcons.Cancel
            onClicked: {
                exit(0)
            }
        }
    }


    //头像，账号，密码，协议，登录
    Column{
        anchors.centerIn: parent
        spacing: 20
        //头像
        FluClip{
            anchors.horizontalCenter: parent.horizontalCenter
            radius: [48,48,48,48]
            width: 96
            height: 96
            Image{
                width: 96
                height: 96
                source: ssAvatar
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
        //账号
        FluClip{
            anchors.horizontalCenter: parent.horizontalCenter
            radius: [10,10,10,10]
            width: 300
            height: 50
            FluTextBox{
                id: loginAccount
                font.pixelSize: 15
                width: 300
                height: 50
                placeholderText: qsTr("输入SS账号")
                onTextChanged: {
                    ssAccount = text
                    // ssAvatar = -后端：查找账号头像函数(QString ssAccount)- 返回值QString（若查找到，返回账号头像路径；若未查找到，返回默认头像路径）
                    ssAvatar = qsTr("qrc:/land-page/rc-page/img/avatar.jpg")//替换
                }
            }
        }
        //密码
        FluClip{
            anchors.horizontalCenter: parent.horizontalCenter
            radius: [10,10,10,10]
            width: 300
            height: 50
            FluPasswordBox{
                id: loginPassword
                font.pixelSize: 15
                width: 300
                height: 50
                placeholderText: qsTr("输入SS号密码")
                onTextChanged: {
                    ssPassword = text
                }
            }
        }
        //协议
        Row{
            FluRadioButton{
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 3
                onCheckedChanged: {
                    loginButton.disabled = !loginButton.disabled
                }
            }
            FluCopyableText{
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 6
                text: qsTr("我已阅读并同意")
            }
            FluTextButton{
                text:"服务协议"
                onClicked: {
                    // -前端：转跳“服务协议”界面-
                }
            }
            FluCopyableText{
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 6
                text: qsTr("和")
            }
            FluTextButton{
                text:"隐私保护指引"
                onClicked: {
                    // -前端：转跳“隐私保护指引”界面-
                }
            }
        }
        //登录
        FluClip{
            anchors.horizontalCenter: parent.horizontalCenter
            radius: [10,10,10,10]
            width: 300
            height: 45
            FluFilledButton{
                id: loginButton
                disabled: true
                disableColor: "#A6D5FF"
                text:"登录"
                width: 300
                height: 45
                onClicked: {
                    if(ssAccount != "")
                    {
                        if(ssPassword != "")
                        {
                            // if(-后端：校验账号密码函数(string ssAccount, string ssPassword)-)
                            if(true)//替换
                            {
                                // -前端：转跳主界面-
                                showInfo(qsTr("您已登入"))//替换
                            }
                        }
                        else
                            showError(qsTr("SS密码不能为空"))
                    }
                    else
                        showError(qsTr("SS账号不能为空"))
                }
            }
        }
    }

    //注册 & 找回密码
    Row{
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 40
        anchors.left: parent.left
        anchors.leftMargin: 123
        FluTextButton{
            text:"注册用户"
            onClicked: {
                // -前端：转跳“注册用户”界面-
                FluRouter.navigate("/register");//替换
                close();//替换
            }
        }
        Text{
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 8
            text: qsTr(" | ")
        }
        FluTextButton{
            text:"找回密码"
            onClicked: {
                // -前端：转跳“找回密码”界面-
                FluRouter.navigate("/recover-password");//替换
                close();//替换
            }
        }
    }
}
