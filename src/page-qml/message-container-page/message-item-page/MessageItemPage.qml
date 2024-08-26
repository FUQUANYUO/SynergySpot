pragma Singleton
import QtQuick.Layouts
import QtQuick 2.15
import FluentUI 1.0

FluObject {
    id: group

    property var navigationView
    property var paneItemMenu

    function rename(item, newName) {
        if (newName && newName.trim().length > 0) {
            item.title = newName;
        }
    }

    FluPaneItem {
        id: item_friend

        count: 0
        //menuDelegate:
        url: "qrc:/message-container-page/chat-page/ChatPage.qml"

        iconDelegate: Component {
            Item {
                height: Screen.height * 0.09
                width: Screen.width * 0.20
                x: Screen.width * 0.171750 / 2

                RowLayout {
                    height: parent.height
                    //anchors.fill: parent  // 确保 RowLayout 填充整个 Item
                    width: parent.width   // 直接使用 width 而不是 implicitWidth

                    FluRectangle {
                        width: parent.width * 0.005
                    }
                    FluClip {
                        Layout.alignment: Qt.AlignVCenter  // 水平居中并垂直居中
                        height: width
                        radius: [20, 20, 20, 20]
                        width: parent.height * 0.555555556

                        Image {
                            anchors.fill: parent
                            asynchronous: true
                            source: "qrc:/message-container-page/rc-page/img/avatar.jpg"
                            sourceSize: Qt.size(width, height)
                        }
                    }
                    ColumnLayout {
                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter  // 左对齐并垂直居中
                        spacing: 4  // 控制行间距

                        FluText {
                            Layout.alignment: Qt.AlignLeft  // 左对齐
                            font.pixelSize: 16
                            text: "群名称"  // 显示群名称
                        }
                        FluText {
                            Layout.alignment: Qt.AlignLeft  // 左对齐
                            color: "#888"  // 使用较浅的颜色显示次要信息
                            font.pixelSize: 14
                            text: "最后一条消息内容"  // 显示最后一条消息
                        }
                    }
                }
            }
        }
        infoBadge: FluBadge {
            count: item_friend.count
        }

        onTap: {
            item_friend.count = 0;
            navigationView.push(url);
        }
    }
}
