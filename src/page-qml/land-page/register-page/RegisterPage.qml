import QtQuick 2.15
import QtQuick.Layouts
import QtQuick.Window 2.15
import QtQuick.Controls
import FluentUI 1.0
// import LoginBackEnd

//注册界面
FluWindow{

    //窗口基本属性
    width: 710
    height: 504
    minimumWidth: 710
    minimumHeight: 504
    maximumWidth: 710
    maximumHeight: 504
    fitsAppBarWindows: true

    // 后端对象
    // property var loginBackEnd : LoginBackEnd{}


    //账号相关属性
    property string ssName: ""              //昵称
    property string ssPassword: ""          //密码
    property string confirmSsPassword: ""   //确认密码
    property string mailbox: ""             //邮箱
    property string verificationCode: ""    //验证码
    property string trueVerificationCode: ""//服务端发送的验证码


    //背景
    AnimatedImage{
        id: registerBackground
        width: parent.width
        height: parent.height
        source: "qrc:/land-page/rc-page/img/backgroundImage.gif"
    }
    FluAcrylic{
        target:registerBackground
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
                // -前端：转跳登陆界面-
                FluRouter.navigate("/");//替换
                close();//替换
            }
        }
    }


    //标题，昵称，密码，确认密码，邮箱，验证码，协议，立即注册
    Column{
        anchors.centerIn: parent
        spacing: 8
        //标题
        Text {
            text: qsTr("欢迎注册SynergySpot")
            font.pointSize: 24
            font.bold: true
        }
        Text {
            text: qsTr("这是slogan")
            font.pointSize: 10
        }
        //昵称
        FluClip{
            anchors.horizontalCenter: parent.horizontalCenter
            radius: [10,10,10,10]
            width: 375
            height: 30
            FluTextBox{
                id: registrName
                font.pixelSize: 15
                width: 375
                height: 30
                placeholderText: qsTr("昵称")
                onTextChanged: {
                    ssName = text
                }
            }
        }
        //密码
        FluClip{
            anchors.horizontalCenter: parent.horizontalCenter
            radius: [10,10,10,10]
            width: 375
            height: 30
            FluPasswordBox{
                id: registerPassword
                font.pixelSize: 15
                width: 375
                height: 30
                placeholderText: qsTr("密码")
                onTextChanged: {
                    ssPassword = text
                }
                onFocusChanged:{
                    if(confirmSsPassword != "" && ssPassword != confirmSsPassword)
                    {
                        passwordWarning.visible = true;
                        showError("密码不一致")
                    }
                    else
                        passwordWarning.visible= false;
                }
            }
        }
        //确认密码
        Row{
            FluClip{
                radius: [10,10,10,10]
                width: 375
                height: 30
                FluPasswordBox{
                    id: registerConfirmPassword
                    font.pixelSize: 15
                    width: 375
                    height: 30
                    placeholderText: qsTr("确认密码")
                    onTextChanged: {
                        confirmSsPassword = text
                    }
                    onFocusChanged:{
                        if(confirmSsPassword != "" && ssPassword != confirmSsPassword)
                        {
                            passwordWarning.visible = true;
                            showError("密码不一致")
                        }
                        else
                            passwordWarning.visible= false;
                    }
                }
            }
            FluIconButton{
                id: passwordWarning
                visible: false
                implicitWidth: 1
                iconSource: FluentIcons.Warning
                iconColor: "red"
            }
        }
        //邮箱
        FluClip{
            anchors.horizontalCenter: parent.horizontalCenter
            radius: [10,10,10,10]
            width: 375
            height: 30
            FluTextBox{
                id: registerMailbox
                font.pixelSize: 15
                width: 375
                height: 30
                placeholderText: qsTr("邮箱")
                onTextChanged: {
                    mailbox = text
                }
            }
        }
        //验证码 & 发送验证码
        Row{
            spacing: 10
            FluClip{
                radius: [10,10,10,10]
                width: 290
                height: 30
                FluTextBox{
                    id: registerVerificationCode
                    font.pixelSize: 15
                    width: 290
                    height: 30
                    placeholderText: qsTr("邮箱验证码")
                    onTextChanged: {
                        verificationCode = text
                    }
                }
            }
            FluClip{
                radius: [10,10,10,10]
                width: 75
                height: 30
                FluButton{
                    width: 75
                    height: 30
                    font.pixelSize: 12
                    text:"发送验证码"
                    onClicked: {
                        if(mailbox != "")
                        {
                            // trueVerificationCode = -后端：发送验证码函数（QString mailbox）- 返回值 QString （所发送的验证码）
                            trueVerificationCode = "1"//替换
                            showInfo(qsTr("验证码已发送"), 2000, qsTr("测试阶段验证码为1"))
                        }
                        else
                            showError(qsTr("邮箱不得为空"))
                    }
                }
            }
        }
        //协议
        Row{
            FluRadioButton{
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 3
                onCheckedChanged: {
                    registerButton.disabled = !registerButton.disabled
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
        //立即注册
        FluClip{
            anchors.horizontalCenter: parent.horizontalCenter
            radius: [10,10,10,10]
            width: 300
            height: 45
            FluFilledButton{
                id: registerButton
                disabled: true
                disableColor: "#A6D5FF"
                text:"立即注册"
                width: 300
                height: 45
                onClicked: {
                    if(ssName != "")
                    {
                        if(ssPassword != "")
                        {
                            if(ssPassword == confirmSsPassword)
                            {
                                if(mailbox != "")
                                {
                                    if(verificationCode != "")
                                    {
                                        if(verificationCode  == trueVerificationCode)
                                        {
                                            //showInfo(qsTr("你的账号为：") +
                                            //-后端：注册账号函数（QString ssName, QString ssPassword, QString mailBox)- 返回值 QString（账号）
                                            //, 0, qsTr("建议截图保存"))

                                            // showInfo(qsTr("你的账号为：") + loginBackEnd.addAccount(ssName, ssPassword, mailbox), 0, qsTr("建议截图保存"))//替换
                                            showInfo(qsTr("你的账号为：") + qsTr("xxxxxxxxxxx"), 0, qsTr("建议截图保存"))//替换
                                        }
                                        else
                                            showError(qsTr("验证码错误"))
                                    }
                                    else
                                        showError(qsTr("验证码不能为空"))
                                }
                                else
                                    showError(qsTr("邮箱不能为空"))
                            }
                            else
                            {
                                showError(qsTr("密码不一致"))
                                passwordWarning.visible = true;
                            }

                        }
                        else
                            showError(qsTr("密码不能为空"))
                    }
                    else
                        showError(qsTr("昵称不能为空"))
                }
            }
        }
    }
}

