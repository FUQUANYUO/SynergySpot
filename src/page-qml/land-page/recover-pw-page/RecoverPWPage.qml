import QtQuick 2.15
import QtQuick.Layouts
import QtQuick.Window 2.15
import QtQuick.Controls
import FluentUI 1.0
// import LoginBackEnd

//找回密码界面
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
    property string ssAccount: ""           //账号
    property string mailbox: ""             //邮箱
    property string verificationCode: ""    //验证码
    property string ssPassword: ""          //新密码
    property string confirmSsPassword: ""   //确认新密码
    property string trueVerificationCode: ""//服务端发送的验证码
    property bool isOver: false             //修改是否结束

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

    //标题，邮箱，验证码，新密码，确认新密码，修改密码
    Column{
        anchors.centerIn: parent
        spacing: 8
        //标题
        Text {
            text: qsTr("设置新密码")
            font.bold: true
            font.pointSize: 24
        }
        Text {
            text: qsTr("安全提示：新密码请勿与旧密码过于相似")
            font.pointSize: 10
        }
        //账号
        FluClip{
            anchors.horizontalCenter: parent.horizontalCenter
            radius: [10,10,10,10]
            width: 375
            height: 30
            FluTextBox{
                id: loginAccount
                font.pixelSize: 15
                width: 375
                height: 30
                placeholderText: qsTr("输入SS账号")
                onTextChanged: {
                    ssAccount = text
                }
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
                onTextChanged:
                {
                    mailbox = text;
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
        //新密码
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
                placeholderText: qsTr("新密码")
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
        //确认新密码
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
                    placeholderText: qsTr("确认新密码")
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
        //修改密码
        FluClip{
            anchors.horizontalCenter: parent.horizontalCenter
            radius: [10,10,10,10]
            width: 300
            height: 45
            FluFilledButton{
                id: registerButton
                disabled: false
                disableColor: "#A6D5FF"
                text:"修改密码"
                width: 300
                height: 45
                onClicked: {
                    if(!isOver)
                    {
                        if(ssAccount != "")
                        {
                            if(mailbox != "")
                            {
                                if(trueVerificationCode != "")
                                {
                                    if(verificationCode != "")
                                    {

                                        if(verificationCode  == trueVerificationCode)
                                        {
                                            if(ssPassword != "")
                                            {
                                                if(ssPassword == confirmSsPassword)
                                                {
                                                    // -后端：修改密码函数（QString mailbox-
                                                    // -前端：转跳“修改成功”界面-
                                                    FluRouter.navigate("/recoverSuccess");//替换
                                                    close();//替换
                                                    isOver = true;
                                                }
                                                else
                                                {
                                                    showError(qsTr("密码不一致"))
                                                    passwordWarning.visible = true;
                                                }
                                            }
                                            else
                                                showError(qsTr("新密码不能为空"))
                                        }
                                        else
                                            showError(qsTr("验证码错误"))
                                    }
                                    else
                                        showError(qsTr("验证码不能为空"))
                                }
                                else
                                    showError(qsTr("验证码未发送"))
                            }
                            else
                                showError(qsTr("邮箱不能为空"))
                        }
                        else
                            showError(qsTr("账号不能为空"))
                    }
                    else
                        showError(qsTr("请关闭此页面登录您的账号"))
                }
            }
        }
    }
}