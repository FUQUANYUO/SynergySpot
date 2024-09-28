import QtQuick
import FluentUI

Rectangle {
    property string bgColor
    property string avatar
    property bool online
    property int size
    width: size
    height: size
    radius: size / 2
    color: bgColor ? bgColor : "transparent"

    FluText {
        anchors.centerIn: parent
        text: avatar ? [...avatar][0] : "" //截取可能含有emoji的字符串的第一个字符
        color: "white"
        font.pixelSize: size * 0.5
    }

    FluBadge {
        count: online
        isDot: true
        color: FluTheme.primaryColor.lighter
        anchors {
            right: parent.right
            bottom: parent.bottom
        }
    }
}