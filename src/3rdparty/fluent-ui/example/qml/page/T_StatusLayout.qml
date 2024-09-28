import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import FluentUI 1.0
import "../component"

FluScrollablePage{

    title: qsTr("StatusLayout")

    FluFrame{
        id:layout_actions
        Layout.fillWidth: true
        Layout.preferredHeight: 50
        padding: 10
        RowLayout{
            spacing: 14
            FluDropDownButton{
                id:btn_status_mode
                Layout.preferredWidth: 140
                text:"Loading"
                FluMenuItem{
                    text:"Loading"
                    onClicked: {
                        btn_status_mode.text = text
                        status_view.statusMode = FluStatusLayoutType.Loading
                    }
                }
                FluMenuItem{
                    text:"Empty"
                    onClicked: {
                        btn_status_mode.text = text
                        status_view.statusMode = FluStatusLayoutType.Empty
                    }
                }
                FluMenuItem{
                    text:"Error"
                    onClicked: {
                        btn_status_mode.text = text
                        status_view.statusMode = FluStatusLayoutType.Error
                    }
                }
                FluMenuItem{
                    text:"Success"
                    onClicked: {
                        btn_status_mode.text = text
                        status_view.statusMode = FluStatusLayoutType.Success
                    }
                }
            }
        }
    }

    FluFrame{
        Layout.fillWidth: true
        Layout.topMargin: 10
        Layout.preferredHeight: 380
        padding: 10
        FluStatusLayout{
            id:status_view
            anchors.fill: parent
            onErrorClicked:{
                status_view.statusMode = FluStatusLayoutType.Loading
            }
            Rectangle {
                anchors.fill: parent
                color:FluTheme.primaryColor
            }
        }
    }
    CodeExpander{
        Layout.fillWidth: true
        Layout.topMargin: -6
        code:'FluStatusLayout{
    anchors.fill: parent
    statusMode: FluStatusLayoutType.Loading
    Rectangle{
        anchors.fill: parent
        color:FluTheme.primaryColor
    }
}'
    }

}
