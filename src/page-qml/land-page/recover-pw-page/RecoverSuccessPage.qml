import QtQuick 2.15
import QtQuick.Layouts
import QtQuick.Window 2.15
import QtQuick.Controls
import FluentUI 1.0
// import LoginBackEnd

//密码修改成功弹窗
FluWindow{
    id: registerWindow

    // property var loginBackEnd : LoginBackEnd{}

    //窗口基本属性
    width: 300
    height: 160
    minimumWidth: 300
    minimumHeight: 160
    maximumWidth: 300
    maximumHeight: 160
    fitsAppBarWindows: true

    //背景
    // AnimatedImage{
    //     id: registerBackground
    //     width: parent.width
    //     height: parent.height
    //     source: "qrc:/land-page/rc-page/img/backgroundImage.gif"
    // }
    // FluAcrylic{
    //     target:registerBackground
    //     width: parent.width
    //     height: parent.height
    //     anchors.centerIn: parent
    // }

    //菜单栏
    Row
    {
        anchors.right: parent.right
        //关闭
        FluIconButton{
            anchors.right: parent.right
            iconSource:FluentIcons.Cancel
            onClicked: {
                FluRouter.navigate("/");
                close();
            }
        }
    }

    Text {
        anchors.centerIn: parent
        text: qsTr("密码修改成功！")
        font.bold: true
        font.pointSize: 24
    }
}