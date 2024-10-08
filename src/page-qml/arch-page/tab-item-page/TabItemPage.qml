pragma Singleton

import QtQuick 2.15
import FluentUI 1.0

FluObject {
    property var navigationView
    property var paneItemMenu

    function rename(item, newName) {
        if (newName && newName.trim().length > 0) {
            item.title = newName;
        }
    }

    // 头像
    FluPaneItem {
        id: avatar

        url: "qrc:/land-page/LandPage.qml"

        iconDelegate: FluClip {
            height: width
            radius: [20, 20, 20, 20]
            width: Screen.width * 0.0296875

            Image {
                anchors.fill: parent
                asynchronous: true
                source: "qrc:/arch-page/rc-page/img/avatar.jpg"
                sourceSize: Qt.size(width, height)
            }
            MouseArea {
                anchors.fill: parent

                onClicked: {
                    console.log("Image clicked");
                    navigationView.push(avatar.url);
                }
            }
        }
    }

    // 消息内容
    FluPaneItem {
        id: item_home

        count: 0
        icon: FluentIcons.Message
        title: qsTr("Message")
        url: "qrc:/message-container-page/MessageContainerPage.qml"

        //menuDelegate: paneItemMenu
        infoBadge: FluBadge {
            count: item_home.count
        }

        onTap: {
            if (navigationView.getCurrentUrl()) {
                item_home.count = 0;
            }
            navigationView.push(url);
        }
    }

    // 联系人
    FluPaneItem {
        id: item_friends

        count: 0
        icon: FluentIcons.People
        menuDelegate: paneItemMenu
        title: qsTr("Contact")
        url: "qrc:/contact-page/ContactPage.qml"

        infoBadge: FluBadge {
            count: item_friends.count
        }

        onTap: {
            item_friends.count = 0;
            navigationView.push(url);
        }
    }
}
