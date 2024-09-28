import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import FluentUI 1.0

// FluScrollablePage {
//     title: qsTr("Clip")
//
//     autoSuggestBox: FluAutoSuggestBox {
//         iconSource: FluentIcons.Search
//         // items: MessageItemPage.getSearchData()
//         placeholderText: qsTr("Search")
//
//         onItemClicked: data => {
//             // MessageItemPage.startPageByItem(data);
//         }
//     }
//     FluFrame {
//         Layout.fillWidth: true
//         Layout.preferredHeight: 380
//         padding: 10
//
//         Column {
//             spacing: 15
//
//             anchors {
//                 left: parent.left
//                 verticalCenter: parent.verticalCenter
//             }
//             FluText {
//                 Layout.topMargin: 20
//                 text: qsTr("Use with images (this component will have no effect under software rendering)")
//             }
//             RowLayout {
//                 spacing: 14
//
//                 FluClip {
//                     height: 50
//                     radius: [25, 0, 25, 25]
//                     width: 50
//
//                     Image {
//                         anchors.fill: parent
//                         asynchronous: true
//                         source: "qrc:/contact-page/rc-page/img/avatar.jpg"
//                         sourceSize: Qt.size(width, height)
//                     }
//                 }
//                 FluClip {
//                     height: 50
//                     radius: [10, 10, 10, 10]
//                     width: 50
//
//                     Image {
//                         anchors.fill: parent
//                         asynchronous: true
//                         source: "qrc:/contact-page/rc-page/img/avatar.jpg"
//                         sourceSize: Qt.size(width, height)
//                     }
//                 }
//                 FluClip {
//                     height: 50
//                     radius: [25, 25, 25, 25]
//                     width: 50
//
//                     Image {
//                         anchors.fill: parent
//                         asynchronous: true
//                         source: "qrc:/contact-page/rc-page/img/avatar.jpg"
//                         sourceSize: Qt.size(width, height)
//                     }
//                 }
//                 FluClip {
//                     height: 50
//                     radius: [0, 25, 25, 25]
//                     width: 50
//
//                     Image {
//                         anchors.fill: parent
//                         asynchronous: true
//                         source: "qrc:/contact-page/rc-page/img/avatar.jpg"
//                         sourceSize: Qt.size(width, height)
//                     }
//                 }
//             }
//             FluClip {
//                 Layout.topMargin: 20
//                 height: 1200 / 5
//                 radius: [8, 8, 8, 8]
//                 width: 1920 / 5
//
//                 Image {
//                     anchors.fill: parent
//                     asynchronous: true
//                     source: "qrc:/contact-page/rc-page/img/avatar.jpg"
//                     sourceSize: Qt.size(2 * width, 2 * height)
//                 }
//             }
//         }
//     }
// }

FluScrollablePage {
    id: contact_page

    Column {
        FluAutoSuggestBox {
            iconSource: FluentIcons.Search
            // items: MessageItemPage.getSearchData()
            placeholderText: qsTr("Search")
            width: Screen.width * 0.20

            onItemClicked: data => {}
        }
        FluIconButton {
            iconSource: FluentIcons.People
            text: "好友管理器"

            onClicked: {}
        }
        FluTextButton {
            color:
            text: "好友通知"

            onClicked: {}
        }
        FluTextButton {
            text: "群通知"

            onClicked: {}
        }
        FluFrame {
            padding: 10
            width: Screen.width * 0.20

            FluPivot {
                currentIndex: 1

                FluPivotItem {
                    title: qsTr("好友")

                    contentItem: FluClip {
                    }
                }
                FluPivotItem {
                    title: qsTr("群聊")

                    contentItem: FluClip {
                    }
                }
            }
        }
    }
}
