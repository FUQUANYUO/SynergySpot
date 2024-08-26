import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import FluentUI 1.0

FluScrollablePage {
    id: root

    property var colorData: [FluColors.Yellow, FluColors.Orange, FluColors.Red, FluColors.Magenta, FluColors.Purple, FluColors.Blue, FluColors.Teal, FluColors.Green]

    ColumnLayout {
        id: column_root

        function getRandomMessage() {
            var messages = ["Hello, how are you?", "What are you doing?", "Let's meet up later!", "I'm working on a project.", "This is a random message.", "Do you have any plans for today?", "I found something interesting!", "Good morning!", "See you soon!", "How's it going?"];

            // 获取一个随机索引
            var randomIndex = Math.floor(Math.random() * messages.length);
            return messages[randomIndex];
        }

        anchors.fill: parent
        spacing: 10

        FluFrame {
            Layout.fillWidth: true
            Layout.rightMargin: 5
            padding: 10

            RowLayout {
                anchors.fill: parent

                FluTextButton {
                    text: qsTr("Chat")
                }
                FluRectangle {
                    Layout.fillWidth: parent
                    color: "red"
                }
                FluIconButton {
                    iconSource: FluentIcons.Phone
                }
                FluIconButton {
                    iconSource: FluentIcons.VideoChat
                }
                FluIconButton {
                    iconSource: FluentIcons.NearbySharing
                }
                FluIconButton {
                    iconSource: FluentIcons.ViewAll
                }
                FluIconButton {
                    iconSource: FluentIcons.AddTo
                }
                FluIconButton {
                    iconSource: FluentIcons.GlobalNavButton
                }
            }
        }
        FluFrame {
            Layout.fillHeight: true
            Layout.fillWidth: true
            //Layout.alignment: Qt.AlignTop
            padding: 10

            ColumnLayout {
                spacing: 10

                // 显示 10 条随机消息
                Repeater {
                    model: 10

                    FluText {
                        Layout.fillWidth: parent
                        color: "black"
                        font.pixelSize: 16
                        text: column_root.getRandomMessage()
                    }
                }
            }
        }
    }
}
