import QtQuick 2.15
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Dialogs
import Qt5Compat.GraphicalEffects
import FluentUI 1.0
import "qrc:/component"

FluPage {
    anchors.fill: parent

    Component.onCompleted: {
        // if (!store.getConfig("chat_tour")) {
        //     store.setConfig("chat_tour", "done")
        //     tour_delay.start()
        // }
        tour_delay.start();
    }

    // 文本消息
    Component {
        id: message_text

        Item {
            id: message_text_item

            clip: true
            height: {
                return Math.max(message_text_avatar.height, message_text_rectangle.height + message_text_name.height) + message_text_time.height + 10;
            }
            width: message_view.width - 10 //避开滚动条

            RandomAvatar {
                id: message_text_avatar

                avatar: "🤗"
                // bgColor: model.user.color
                // avatar: model.user.avatar
                // online: model.user.online
                bgColor: "#aef"
                online: true
                size: 35

                anchors {
                    left: isSender ? undefined : parent.left
                    right: isSender ? parent.right : undefined
                    top: parent.top
                }
            }
            FluText {
                id: message_text_name

                color: FluTheme.dark ? FluColors.Grey100 : FluColors.Grey100
                elide: Text.ElideRight
                font.pixelSize: 12
                horizontalAlignment: isSender ? Text.AlignRight : Text.AlignLeft
                maximumLineCount: 1
                // text: model.user.remark ? model.user.remark : model.user.nickname
                text: "TESTName1"

                anchors {
                    left: isSender ? message_text_item.left : message_text_avatar.right
                    leftMargin: 10
                    right: isSender ? message_text_avatar.left : message_text_item.right
                    rightMargin: 10
                    top: parent.top
                }
            }
            Rectangle {
                id: message_text_rectangle

                color: {
                    if (isSender)
                        return FluTheme.dark ? FluTheme.primaryColor.lighter : FluTheme.primaryColor.dark;
                    return FluTheme.dark ? Window.active ? Qt.rgba(38 / 255, 44 / 255, 54 / 255, 1) : Qt.rgba(39 / 255, 39 / 255, 39 / 255, 1) : Qt.rgba(251 / 255, 251 / 255, 253 / 255, 1);
                }
                height: message_text_content.contentHeight + 20
                radius: 10
                width: {
                    let max_width = message_text_item.width * 0.75;
                    let need_width = message_text_metrics.width + 20;
                    return need_width > max_width ? max_width : need_width;
                }

                anchors {
                    left: isSender ? undefined : message_text_avatar.right
                    leftMargin: 10
                    right: isSender ? message_text_avatar.left : undefined
                    rightMargin: 10
                    top: message_text_name.bottom
                    topMargin: 5
                }
                FluCopyableText {
                    id: message_text_content

                    color: FluTheme.dark ^ isSender ? FluColors.White : FluColors.Black
                    font.pixelSize: 14
                    // text: model.content
                    text: "Copy text1"
                    wrapMode: Text.Wrap

                    anchors {
                        bottom: parent.bottom
                        left: parent.left
                        margins: 10
                        right: parent.right
                        top: parent.top
                    }
                }
                FluCopyableText {
                    id: message_text_metrics

                    font: message_text_content.font
                    text: message_text_content.text
                    visible: false
                }
            }
            FluText {
                id: message_text_time

                color: FluTheme.dark ? FluColors.Grey120 : FluColors.Grey80
                font.pixelSize: 10
                // text: Qt.formatDateTime(new Date(model.time * 1000), "yyyy-MM-dd hh:mm:ss") + " · #" + model.mid
                text: "yyyy-MM-dd hh:mm:ss"

                anchors {
                    left: isSender ? undefined : message_text_rectangle.left
                    right: isSender ? message_text_rectangle.right : undefined
                    top: message_text_rectangle.bottom
                    topMargin: 5
                }
            }
        }
    }

    // 图片消息
    Component {
        id: message_image

        Item {
            id: message_image_item

            clip: true
            height: {
                return Math.max(message_image_avatar.height, message_image_rectangle.height + message_image_name.height) + message_image_time.height + 10;
            }
            width: message_view.width - 10 //避开滚动条

            onWidthChanged: {
                if (message_image_image.status == Image.Ready) {
                    var max_size = Math.min(message_image_item.width * 0.5, 666);
                    var scale = max_size / Math.max(message_image_image.sourceSize.width, message_image_image.sourceSize.height);
                    message_image_rectangle.width = message_image_image.sourceSize.width * scale;
                    message_image_rectangle.height = message_image_image.sourceSize.height * scale;
                }
            }

            RandomAvatar {
                id: message_image_avatar

                avatar: "🤔"
                // bgColor: model.user.color
                // avatar: model.user.avatar
                // online: model.user.online
                bgColor: "#aaf"
                online: true
                size: 35

                anchors {
                    left: isSender ? undefined : parent.left
                    right: isSender ? parent.right : undefined
                    top: parent.top
                }
            }

            // 消息来源
            FluText {
                id: message_image_name

                color: FluTheme.dark ? FluColors.Grey100 : FluColors.Grey100
                elide: Text.ElideRight
                font.pixelSize: 12
                horizontalAlignment: isSender ? Text.AlignRight : Text.AlignLeft
                maximumLineCount: 1
                // text: model.user.remark ? model.user.remark : model.user.nickname
                text: "TestName2"

                anchors {
                    left: isSender ? message_image_item.left : message_image_avatar.right
                    leftMargin: 10
                    right: isSender ? message_image_avatar.left : message_image_item.right
                    rightMargin: 10
                    top: parent.top
                }
            }

            // 图片消息
            FluRectangle {
                id: message_image_rectangle

                height: 200
                radius: [10, 10, 10, 10]
                width: 200

                anchors {
                    left: isSender ? undefined : message_image_avatar.right
                    leftMargin: 10
                    right: isSender ? message_image_avatar.left : undefined
                    rightMargin: 10
                    top: message_image_name.bottom
                    topMargin: 5
                }
                Image {
                    id: message_image_image

                    anchors.fill: parent
                    asynchronous: true
                    // source: model.content
                    source: "https://cn.bing.com/images/search?view=detailV2&ccid=jdP04yEo&id=7CFFD1044B4096392CD8261CBC67D03F75B0017D&thid=OIP.jdP04yEoxG10mcywseQj7gAAAA&mediaurl=https%3a%2f%2fpic.mksucai.com%2f00%2f35%2f90%2fbc7ed1a8d27fa6ee.webp&exph=256&expw=256&q=%e5%9b%be%e7%89%87&simid=608037253960306903&FORM=IRPRST&ck=99EA2241C44D84CBA72C8B913DB18A7E&selectedIndex=0&itb=0&idpp=overlayview&ajaxhist=0&ajaxserp=0"

                    onStatusChanged: {
                        if (status == Image.Ready) {
                            var max_size = Math.min(message_image_item.width * 0.5, 424);
                            var scale = max_size / Math.max(sourceSize.width, sourceSize.height);
                            message_image_rectangle.width = sourceSize.width * scale;
                            message_image_rectangle.height = sourceSize.height * scale;
                        }
                    }

                    MouseArea {
                        anchors.fill: parent

                        onClicked: {
                            image_viwer_image.source = model.content;
                            image_viwer.visible = true;
                        }
                    }
                }
            }
            FluText {
                id: message_image_time

                color: FluTheme.dark ? FluColors.Grey120 : FluColors.Grey80
                font.pixelSize: 10
                // text: Qt.formatDateTime(new Date(model.time * 1000), "yyyy-MM-dd hh:mm:ss") + " · #" + model.mid
                text: "yyyy-MM-dd hh:mm:ss"

                anchors {
                    left: isSender ? undefined : message_image_rectangle.left
                    right: isSender ? message_image_rectangle.right : undefined
                    top: message_image_rectangle.bottom
                    topMargin: 5
                }
            }
        }
    }

    // 文件消息
    Component {
        id: message_file

        Item {
            id: message_file_item

            clip: true
            height: {
                return Math.max(message_file_avatar.height, message_file_rectangle.height + message_file_name.height) + message_file_time.height + 10;
            }
            width: message_view.width - 10 //避开滚动条

            Component.onCompleted: {
                // var jsonString = model.content
                // var json = JSON.parse(jsonString)
                // message_file_text.text = json.name
                //
                // var size = json.size
                // if (size < 1024) {
                //     message_file_size.text = size + "B"
                // } else if (size < 1024 * 1024) {
                //     message_file_size.text = (size / 1024).toFixed(2) + "KB"
                // } else if (size < 1024 * 1024 * 1024) {
                //     message_file_size.text = (size / 1024 / 1024).toFixed(2) + "MB"
                // } else {
                //     message_file_size.text = (size / 1024 / 1024 / 1024).toFixed(2) + "GB"
                // }
                message_file_text = "file test";
                message_file_size = "{file test size}" + "GB";
            }

            RandomAvatar {
                id: message_file_avatar

                avatar: "😂"
                // bgColor: model.user.color
                // avatar: model.user.avatar
                // online: model.user.online
                bgColor: "#aaa"
                online: true
                size: 35

                anchors {
                    left: isSender ? undefined : parent.left
                    right: isSender ? parent.right : undefined
                    top: parent.top
                }
            }
            FluText {
                id: message_file_name

                color: FluTheme.dark ? FluColors.Grey100 : FluColors.Grey100
                elide: Text.ElideRight
                font.pixelSize: 12
                horizontalAlignment: isSender ? Text.AlignRight : Text.AlignLeft
                maximumLineCount: 1
                // text: model.user.remark ? model.user.remark : model.user.nickname
                text: "TestName3"

                anchors {
                    left: isSender ? message_file_item.left : message_file_avatar.right
                    leftMargin: 10
                    right: isSender ? message_file_avatar.left : message_file_item.right
                    rightMargin: 10
                    top: parent.top
                }
            }
            FluFrame {
                id: message_file_rectangle

                height: 100
                radius: 10
                width: 250

                anchors {
                    left: isSender ? undefined : message_file_avatar.right
                    leftMargin: 10
                    right: isSender ? message_file_avatar.left : undefined
                    rightMargin: 10
                    top: message_file_name.bottom
                    topMargin: 5
                }
                FluIcon {
                    id: message_file_icon

                    iconColor: FluTheme.dark ? FluTheme.primaryColor.lighter : FluTheme.primaryColor.dark
                    iconSize: 42
                    iconSource: FluentIcons.Attach

                    anchors {
                        left: parent.left
                        leftMargin: 15
                        verticalCenter: parent.verticalCenter
                        verticalCenterOffset: -5
                    }
                }
                FluText {
                    id: message_file_size

                    color: FluTheme.dark ? FluColors.Grey100 : FluColors.Grey100
                    font.pixelSize: 10

                    anchors {
                        horizontalCenter: message_file_icon.horizontalCenter
                        top: message_file_icon.bottom
                        topMargin: 5
                    }
                }
                FluText {
                    id: message_file_text

                    color: FluTheme.dark ? FluColors.White : FluColors.Black
                    elide: Text.ElideRight
                    font.pixelSize: 14
                    maximumLineCount: 3
                    width: parent.width
                    wrapMode: Text.WrapAnywhere

                    anchors {
                        left: message_file_icon.right
                        leftMargin: 10
                        right: message_file_button.left
                        verticalCenter: parent.verticalCenter
                    }
                }
                FluTextButton {
                    id: message_file_button

                    text: "下载"
                    width: 60

                    onClicked:
                    // var jsonString = model.content
                    // var json = JSON.parse(jsonString)
                    // file_download_dialog.fileName = json.name
                    // file_download_dialog.fileData = json.data
                    // file_download_dialog.open()
                    {}

                    anchors {
                        bottom: parent.bottom
                        bottomMargin: 10
                        right: parent.right
                        rightMargin: 10
                    }
                }
            }
            FluText {
                id: message_file_time

                color: FluTheme.dark ? FluColors.Grey120 : FluColors.Grey80
                font.pixelSize: 10
                // text: Qt.formatDateTime(new Date(model.time * 1000), "yyyy-MM-dd hh:mm:ss") + " · #" + model.mid
                text: "yyyy-MM-dd hh:mm:ss"

                anchors {
                    left: isSender ? undefined : message_file_rectangle.left
                    right: isSender ? message_file_rectangle.right : undefined
                    top: message_file_rectangle.bottom
                    topMargin: 5
                }
            }
        }
    }

    // 传输消息
    Component {
        id: message_p2p_file

        Item {
            id: message_p2p_file_item

            clip: true
            height: {
                return Math.max(message_p2p_file_avatar.height, message_p2p_file_rectangle.height + message_p2p_file_name.height) + message_p2p_file_time.height + 10;
            }
            width: message_view.width - 10 //避开滚动条

            Component.onCompleted: {
                // var jsonString = model.content
                // var json = JSON.parse(jsonString)
                // message_p2p_file_text.text = json.name
                //
                // var size = json.size
                // if (size < 1024) {
                //     message_p2p_file_size.text = size + "B"
                // } else if (size < 1024 * 1024) {
                //     message_p2p_file_size.text = (size / 1024).toFixed(2) + "KB"
                // } else if (size < 1024 * 1024 * 1024) {
                //     message_p2p_file_size.text = (size / 1024 / 1024).toFixed(2) + "MB"
                // } else {
                //     message_p2p_file_size.text = (size / 1024 / 1024 / 1024).toFixed(2) + "GB"
                // }
                message_p2p_file_text.text = "test";
                var size = 1024;
                if (size < 1024) {
                    message_p2p_file_size.text = size + "B";
                } else if (size < 1024 * 1024) {
                    message_p2p_file_size.text = (size / 1024).toFixed(2) + "KB";
                } else if (size < 1024 * 1024 * 1024) {
                    message_p2p_file_size.text = (size / 1024 / 1024).toFixed(2) + "MB";
                } else {
                    message_p2p_file_size.text = (size / 1024 / 1024 / 1024).toFixed(2) + "GB";
                }
            }

            RandomAvatar {
                id: message_p2p_file_avatar

                avatar: "🤣"
                // bgColor: model.user.color
                // avatar: model.user.avatar
                // online: model.user.online
                bgColor: "#ada"
                online: true
                size: 35

                anchors {
                    left: isSender ? undefined : parent.left
                    right: isSender ? parent.right : undefined
                    top: parent.top
                }
            }
            FluText {
                id: message_p2p_file_name

                color: FluTheme.dark ? FluColors.Grey100 : FluColors.Grey100
                elide: Text.ElideRight
                font.pixelSize: 12
                horizontalAlignment: isSender ? Text.AlignRight : Text.AlignLeft
                maximumLineCount: 1
                // text: model.user.remark ? model.user.remark : model.user.nickname
                text: "file name test p2p"

                anchors {
                    left: isSender ? message_p2p_file_item.left : message_p2p_file_avatar.right
                    leftMargin: 10
                    right: isSender ? message_p2p_file_avatar.left : message_p2p_file_item.right
                    rightMargin: 10
                    top: parent.top
                }
            }
            FluFrame {
                id: message_p2p_file_rectangle

                height: 100
                radius: 10
                width: 250

                anchors {
                    left: isSender ? undefined : message_p2p_file_avatar.right
                    leftMargin: 10
                    right: isSender ? message_p2p_file_avatar.left : undefined
                    rightMargin: 10
                    top: message_p2p_file_name.bottom
                    topMargin: 5
                }
                FluIcon {
                    id: message_p2p_file_icon

                    iconColor: FluTheme.dark ? FluTheme.primaryColor.lighter : FluTheme.primaryColor.dark
                    iconSize: 42
                    iconSource: FluentIcons.Network

                    anchors {
                        left: parent.left
                        leftMargin: 15
                        verticalCenter: parent.verticalCenter
                        verticalCenterOffset: -5
                    }
                }
                FluText {
                    id: message_p2p_file_size

                    color: FluTheme.dark ? FluColors.Grey100 : FluColors.Grey100
                    font.pixelSize: 10

                    anchors {
                        horizontalCenter: message_p2p_file_icon.horizontalCenter
                        top: message_p2p_file_icon.bottom
                        topMargin: 5
                    }
                }
                FluText {
                    id: message_p2p_file_text

                    color: FluTheme.dark ? FluColors.White : FluColors.Black
                    elide: Text.ElideRight
                    font.pixelSize: 14
                    maximumLineCount: 3
                    width: parent.width
                    wrapMode: Text.WrapAnywhere

                    anchors {
                        left: message_p2p_file_icon.right
                        leftMargin: 10
                        right: message_p2p_file_button.left
                        verticalCenter: parent.verticalCenter
                    }
                }
                FluTextButton {
                    id: message_p2p_file_button

                    text: "下载"
                    width: 60

                    onClicked: {
                        // var jsonString = model.content
                        // var json = JSON.parse(jsonString)
                        // p2p_file_download_dialog.fileName = json.name
                        // p2p_file_download_dialog.fileHash = json.hash
                        // p2p_file_download_dialog.uid = model.user.id
                        // p2p_file_download_dialog.open()
                        p2p_file_download_dialog.fileName = "p2p test";
                        p2p_file_download_dialog.fileHash = "abcde";
                        p2p_file_download_dialog.uid = "100001";
                        p2p_file_download_dialog.open();
                    }

                    anchors {
                        bottom: parent.bottom
                        bottomMargin: 10
                        right: parent.right
                        rightMargin: 10
                    }
                }
            }
            FluText {
                id: message_p2p_file_time

                color: FluTheme.dark ? FluColors.Grey120 : FluColors.Grey80
                font.pixelSize: 10
                // text: Qt.formatDateTime(new Date(model.time * 1000), "yyyy-MM-dd hh:mm:ss") + " · #" + model.mid
                text: "yyyy-MM-dd hh:mm:ss"

                anchors {
                    left: isSender ? undefined : message_p2p_file_rectangle.left
                    right: isSender ? message_p2p_file_rectangle.right : undefined
                    top: message_p2p_file_rectangle.bottom
                    topMargin: 5
                }
            }
        }
    }
    Rectangle {
        id: header

        color: FluTheme.dark ? Window.active ? Qt.rgba(38 / 255, 44 / 255, 54 / 255, 1) : Qt.rgba(39 / 255, 39 / 255, 39 / 255, 1) : Qt.rgba(251 / 255, 251 / 255, 253 / 255, 1)
        height: 45
        radius: 10
        width: parent.width
        z: 1

        anchors {
            left: parent.left
            leftMargin: 10
            right: parent.right
            rightMargin: 10
            top: parent.top
            topMargin: 5
        }
        FluText {
            id: header_title

            color: FluTheme.dark ? FluColors.White : FluColors.Black
            elide: Text.ElideRight
            font.pixelSize: 20
            text: {
                // if (!store.currentGroup) return ""
                // let s = ""
                // if (store.currentGroup.type === "twin") {
                //     s += store.currentGroup.remark ? store.currentGroup.remark : store.currentGroup.owner.nickname
                // } else {
                //     s += store.currentGroup.remark ? store.currentGroup.remark : store.currentGroup.name
                //     if (store.currentGroupUsers.length > 0)
                //         s += "（" + store.currentGroupUsers.length + "）"
                // }
                // return s
                return "这是测试（5）";
            }

            anchors {
                left: parent.left
                leftMargin: 20
                right: header_button_group.left
                verticalCenter: parent.verticalCenter
            }
        }
        Row {
            id: header_button_group

            spacing: 5

            anchors {
                right: parent.right
                rightMargin: 20
                verticalCenter: parent.verticalCenter
            }
            FluIconButton {
                id: header_edit_button

                iconColor: FluTheme.dark ? FluTheme.primaryColor.lighter : FluTheme.primaryColor.dark
                iconSource: FluentIcons.Edit
                text: "修改备注"

                onClicked:
                // if (!store.currentGroup) return
                // edit_remark_textbox.text = store.currentGroup.remark
                // edit_remark_dialog.visible = true
                {}
            }
            FluIconButton {
                id: header_info_button

                iconColor: FluTheme.dark ? FluTheme.primaryColor.lighter : FluTheme.primaryColor.dark
                iconSource: FluentIcons.Info
                text: "关于聊天"

                onClicked: {
                    info_popup.visible = true;
                }
            }
        }
        Popup {
            id: info_popup

            clip: true
            height: 200
            modal: true
            visible: false
            width: 200
            x: parent.width - width
            y: parent.height + 10

            background: FluFrame {
                border.width: 0
                radius: 10
            }
            enter: Transition {
                NumberAnimation {
                    duration: 233
                    easing.type: Easing.InOutExpo
                    from: 0
                    property: "height"
                    to: 200
                }
                NumberAnimation {
                    duration: 233
                    from: 0
                    property: "opacity"
                    to: 1
                }
            }
            exit: Transition {
                NumberAnimation {
                    duration: 233
                    easing.type: Easing.InOutExpo
                    from: 200
                    property: "height"
                    to: 0
                }
                NumberAnimation {
                    duration: 233
                    from: 1
                    property: "opacity"
                    to: 0
                }
            }

            ListView {
                id: info_list

                boundsBehavior: Flickable.DragOverBounds
                height: parent.height
                model: store.currentGroupUsers
                orientation: ListView.Vertical
                spacing: 10
                width: parent.width

                ScrollBar.vertical: FluScrollBar {
                }
                delegate: Item {
                    height: 50
                    width: info_list.width

                    Rectangle {
                        id: info_item

                        property bool hoverd: false

                        color: hoverd ? (FluTheme.dark ? "#11FFFFFF" : "#11000000") : "transparent"
                        height: parent.height
                        radius: 10
                        width: parent.width

                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true

                            onEntered: {
                                info_item.hoverd = true;
                            }
                            onExited: {
                                info_item.hoverd = false;
                            }
                        }
                        FluTooltip {
                            delay: 1000
                            // text: modelData.username
                            text: "user name"
                            visible: info_item.hoverd
                        }
                        RandomAvatar {
                            id: info_avatar

                            avatar: "😍"
                            // bgColor: modelData.color
                            // avatar: modelData.avatar
                            // online: modelData.online
                            bgColor: "#ada"
                            online: true
                            size: 30

                            anchors {
                                left: parent.left
                                leftMargin: 5
                                verticalCenter: parent.verticalCenter
                            }
                        }
                        FluText {
                            color: FluTheme.dark ? FluColors.White : FluColors.Black
                            elide: Text.ElideRight
                            font.pixelSize: 14
                            // text: modelData.remark ? modelData.remark : modelData.nickname
                            text: "remark"

                            anchors {
                                left: info_avatar.right
                                leftMargin: 5
                                right: parent.right
                                rightMargin: 5
                                top: info_avatar.top
                            }
                        }
                        FluText {
                            color: FluTheme.dark ? FluColors.Grey100 : FluColors.Grey100
                            elide: Text.ElideRight
                            font.pixelSize: 10
                            // text: modelData.username
                            text: "model username"

                            anchors {
                                bottom: info_avatar.bottom
                                left: info_avatar.right
                                leftMargin: 5
                                right: parent.right
                                rightMargin: 5
                            }
                        }
                    }
                }
                header: Item {
                    height: 50
                    // visible: store.currentGroup && store.currentGroup.type !== "twin"
                    // height: store.currentGroup && store.currentGroup.type !== "twin" ? info_text.height + 20 : 0
                    visible: true
                    width: info_list.width

                    FluText {
                        id: info_text

                        color: FluTheme.dark ? FluColors.Grey100 : FluColors.Grey100
                        // text: store.currentGroup ? "群号：#" + store.currentGroup.id + "\n群主：" + store.currentGroup.owner.username + "\n人数：" + store.currentGroupUsers.length + "\n群名：" + store.currentGroup.name : ""
                        text: "群号：#10023130" + "\n群主：100023"
                        wrapMode: Text.WrapAnywhere

                        anchors {
                            left: parent.left
                            margins: 10
                            right: parent.right
                            top: parent.top
                        }
                    }
                }
            }
        }
    }
    Item {
        id: message_view_mask

        //防止最上面和最下面的空白显示消息
        clip: true

        anchors {
            bottom: input_area.top
            bottomMargin: -10
            left: parent.left
            leftMargin: 20
            right: parent.right
            rightMargin: 10 //这里不是20是因为有个滚动条
            top: header.bottom
            topMargin: -10
        }
        ListView {
            id: message_view

            property var lastGroupWhenBottom: null
            property bool loading: true
            property var oldFirst: 0

            boundsBehavior: Flickable.DragOverBounds
            orientation: ListView.Vertical
            spacing: 20

            ScrollBar.vertical: FluScrollBar {
            }
            delegate: Loader {
                id: message_loader

                // property var model: modelData
                // property bool isSender: modelData.user === store.currentUser
                property bool isSender: true

                sourceComponent: {
                    // if (model.type === "text") {
                    //     return message_text
                    // }
                    // if (model.type === "image") {
                    //     return message_image
                    // }
                    // if (model.type === "file") {
                    //     return message_file
                    // }
                    // if (model.type === "p2p_file") {
                    //     return message_p2p_file
                    // }
                    return message_text;
                }
            }
            footer: Rectangle {
                height: 20
                visible: false
                width: message_view.width
            }

            // model: store.messageList.items

            header: Rectangle {
                color: "transparent"
                height: 60
                width: message_view.width

                FluTextButton {
                    anchors.centerIn: parent
                    // disabled: message_view.loading || !store.messageList.hasMore
                    disabled: false
                    // text: store.messageList.hasMore ? "加载更多" : "没有更多了QwQ"
                    text: "加载更多"

                    onClicked:
                    // message_view.loading = true
                    // store.control.loadMessages()
                    {}
                }
            }

            onModelChanged:
            // if (lastGroupWhenBottom !== store.currentGroup && message_view.contentHeight > message_view.height) {
            //     lastGroupWhenBottom = store.currentGroup
            //     message_view.positionViewAtEnd()
            //     if (message_view.model.length) {
            //         oldFirst = message_view.model[0].id
            //     }
            //     loading = false
            // } else {
            //     if (message_view.model.length && oldFirst !== message_view.model[0].id) {
            //         loading = false
            //         var idx = 0
            //         for (var i = 0; i < message_view.model.length; i++) {
            //             if (message_view.model[i].id === oldFirst) {
            //                 idx = i
            //                 break
            //             }
            //         }
            //         message_view.positionViewAtIndex(Math.max(0, idx - 1), ListView.Beginning)
            //         oldFirst = message_view.model[0].id
            //     } else {
            //         message_view.positionViewAtEnd()
            //     }
            // }
            {}

            anchors {
                bottomMargin: 10
                fill: parent
                topMargin: 10
            }
        }
    }
    FluFrame {
        id: input_area

        property bool editExpand: false

        function sendMessage() {
            if (text_box.text === "")
                return;
            // store.control.sendMessage(store.currentGroup.id, "text", text_box.text)
            text_box.text = "";
            message_view.positionViewAtEnd();
        }

        border.width: 0
        height: editExpand ? parent.height * 0.5 : 150
        padding: 15
        radius: 10

        Behavior on height {
            enabled: FluTheme.enableAnimation

            NumberAnimation {
                duration: 666
                easing.type: Easing.InOutExpo
            }
        }

        anchors {
            bottom: parent.bottom
            bottomMargin: 10
            left: parent.left
            leftMargin: 20
            right: parent.right
            rightMargin: 20
        }
        RowLayout {
            id: button_group

            width: parent.width

            Row {
                spacing: 5

                FluIconButton {
                    iconColor: FluTheme.dark ? FluTheme.primaryColor.lighter : FluTheme.primaryColor.dark
                    iconSource: input_area.editExpand ? FluentIcons.ChevronDown : FluentIcons.ChevronUp
                    text: input_area.editExpand ? "还原输入框" : "扩展输入框"

                    onClicked: {
                        input_area.editExpand = !input_area.editExpand;
                    }
                }

                // 上传图片
                FluIconButton {
                    iconColor: FluTheme.dark ? FluTheme.primaryColor.lighter : FluTheme.primaryColor.dark
                    iconSource: FluentIcons.Photo
                    text: "发送图片"

                    onClicked: {
                        image_upload_dialog.open();
                    }
                }
                FluIconButton {
                    iconColor: FluTheme.dark ? FluTheme.primaryColor.lighter : FluTheme.primaryColor.dark
                    iconSource: FluentIcons.Attach
                    text: "发送文件"

                    onClicked: {
                        file_upload_dialog.open();
                    }
                }
                FluIconButton {
                    iconColor: FluTheme.dark ? FluTheme.primaryColor.lighter : FluTheme.primaryColor.dark
                    iconSource: FluentIcons.Network
                    text: "P2P文件同传"

                    onClicked: {
                        p2p_file_upload_dialog.open();
                    }
                }
                FluIconButton {
                    iconColor: FluTheme.dark ? FluTheme.primaryColor.lighter : FluTheme.primaryColor.dark
                    iconSource: FluentIcons.Cut
                    text: "截屏"

                    onClicked: {
                        screenshot.open();
                    }
                }
            }
            Row {
                Layout.alignment: Qt.AlignRight
                spacing: 5

                FluIconButton {
                    iconColor: FluTheme.dark ? FluTheme.primaryColor.lighter : FluTheme.primaryColor.dark
                    iconSource: FluentIcons.Send

                    onClicked: {
                        input_area.sendMessage();
                    }
                }
            }
        }
        Item {
            id: text_box_container

            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
                top: button_group.bottom
                topMargin: 10
            }
            Flickable {
                anchors.fill: parent
                boundsBehavior: Flickable.StopAtBounds
                clip: true
                contentHeight: text_box.height
                contentWidth: parent.width

                ScrollBar.vertical: FluScrollBar {
                }

                FluMultilineTextBox {
                    id: text_box

                    height: contentHeight + 20 < text_box_container.height ? text_box_container.height : contentHeight + 20
                    padding: 0
                    placeholderText: "Tips：使用Ctrl+Enter换行OvO"
                    width: parent.width

                    background: Rectangle {
                        color: "transparent"
                    }

                    onCommit: {
                        input_area.sendMessage();
                    }
                }
            }
        }
    }

    // 底部边框效果
    Rectangle {
        id: bottom_line_mask

        anchors.fill: input_area
        color: "transparent"
        layer.enabled: true

        layer.effect: OpacityMask {
            maskSource: Rectangle {
                height: bottom_line_mask.height
                radius: 10
                width: bottom_line_mask.width
            }
        }

        Rectangle {
            color: {
                if (FluTheme.dark) {
                    text_box.activeFocus ? FluTheme.primaryColor.lighter : Qt.rgba(166 / 255, 166 / 255, 166 / 255, 1);
                } else {
                    return text_box.activeFocus ? FluTheme.primaryColor.dark : Qt.rgba(183 / 255, 183 / 255, 183 / 255, 1);
                }
            }
            height: text_box.activeFocus ? 5 : 1
            width: text_box.activeFocus ? parent.width : 0

            Behavior on height {
                enabled: FluTheme.enableAnimation

                NumberAnimation {
                    duration: 666
                    easing.type: Easing.OutExpo
                }
            }
            Behavior on width {
                enabled: FluTheme.enableAnimation

                NumberAnimation {
                    duration: 666
                    easing.type: Easing.InOutExpo
                }
            }

            anchors {
                bottom: parent.bottom
                left: parent.left
                leftMargin: text_box.activeFocus ? 0 : parent.width / 2

                Behavior on leftMargin {
                    enabled: FluTheme.enableAnimation

                    NumberAnimation {
                        duration: 666
                        easing.type: Easing.InOutExpo
                    }
                }
            }
        }
    }

    // 图片上传
    FileDialog {
        id: image_upload_dialog

        fileMode: FileDialog.OpenFile
        nameFilters: ["Images (*.png *.jpg *.jpeg)"]

        onAccepted: {
            var file = FluTools.toLocalPath(selectedFile);
            // store.control.sendImage(store.currentGroup.id, file)
        }
    }

    // // 截图
    // FluScreenshot {
    //     id: screenshot
    //     captrueMode: FluScreenshotType.File
    //     saveFolder: FluTools.getApplicationDirPath() + "/screenshot"
    //     onCaptrueCompleted: (captrue) => {
    //         var file = FluTools.toLocalPath(captrue)
    //         // store.control.sendImage(store.currentGroup.id, file)
    //     }
    // }

    // 修改备注
    Popup {
        id: edit_remark_dialog

        anchors.centerIn: Overlay.overlay
        height: 200
        modal: true
        opacity: 0
        visible: false
        width: 300

        background: Rectangle {
            color: "transparent"
        }
        enter: Transition {
            NumberAnimation {
                from: 0.0
                property: "opacity"
                to: 1.0
            }
        }
        exit: Transition {
            NumberAnimation {
                from: 1.0
                property: "opacity"
                to: 0.0
            }
        }

        FluFrame {
            anchors.fill: parent
            radius: 10

            Column {
                id: edit_remark_dialog_column

                anchors.centerIn: parent
                spacing: 10
                width: 200

                FluText {
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: FluTheme.dark ? FluColors.White : FluColors.Black
                    font.pixelSize: 20
                    text: "修改备注"
                }
                FluTextBox {
                    id: edit_remark_textbox

                    placeholderText: "备注"

                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                }
                FluButton {
                    text: "确认修改"
                    width: parent.width

                    onClicked: {
                        // store.control.setGroupRemark(edit_remark_textbox.text)
                        edit_remark_dialog.visible = false;
                        showSuccess("修改成功");
                    }
                }
            }
        }
    }

    // 文件上传
    FileDialog {
        id: file_upload_dialog

        fileMode: FileDialog.OpenFile

        onAccepted: {
            var filePath = FluTools.toLocalPath(selectedFile);
            var fileName = FluTools.getFileNameByUrl(selectedFile);
            // store.control.sendFile(store.currentGroup.id, filePath, fileName)
        }
    }

    // P2P文件上传
    FileDialog {
        id: p2p_file_upload_dialog

        fileMode: FileDialog.OpenFile

        onAccepted: {
            var filePath = FluTools.toLocalPath(selectedFile);
            var fileName = FluTools.getFileNameByUrl(selectedFile);
            // store.control.sendP2PFile(store.currentGroup.id, filePath, fileName)
        }
    }

    // 文件下载
    FolderDialog {
        id: file_download_dialog

        property var fileData
        property var fileName

        onAccepted: {
            if (!fileName || !fileData)
                return;
            var folderPath = FluTools.toLocalPath(selectedFolder);
            var filePath = folderPath + "/" + fileName;
            // store.control.saveBase64File(filePath, fileData)
        }
    }

    // P2P文件下载
    FolderDialog {
        id: p2p_file_download_dialog

        property var fileHash
        property var fileName
        property var uid

        onAccepted: {
            if (!fileName || !fileHash)
                return;
            var folderPath = FluTools.toLocalPath(selectedFolder);
            var filePath = folderPath + "/" + fileName;
            console.log(uid, filePath, fileHash);
            // store.control.saveP2PFile(uid, filePath, fileHash)
        }
    }

    // P2P下载进度
    Popup {
        id: p2p_file_download_progress_popup

        anchors.centerIn: Overlay.overlay
        closePolicy: Popup.NoAutoClose
        height: 200
        modal: true
        opacity: 0
        visible: false
        width: 400

        background: Rectangle {
            color: "transparent"
        }
        enter: Transition {
            NumberAnimation {
                from: 0.0
                property: "opacity"
                to: 1.0
            }
        }
        exit: Transition {
            NumberAnimation {
                from: 1.0
                property: "opacity"
                to: 0.0
            }
        }

        FluFrame {
            anchors.fill: parent
            radius: 10

            Column {
                anchors.centerIn: parent
                spacing: 10
                width: 300

                FluText {
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: FluTheme.dark ? FluColors.White : FluColors.Black
                    font.pixelSize: 20
                    text: "下载进度"
                }
                Rectangle {
                    id: p2p_file_download_progress_bar

                    border.color: FluTheme.primaryColor.normal
                    border.width: 1
                    color: "transparent"
                    height: 6
                    radius: 3
                    width: parent.width

                    Behavior on width {
                        enabled: FluTheme.enableAnimation

                        NumberAnimation {
                            duration: 233
                            easing.type: Easing.InOutExpo
                        }
                    }

                    Rectangle {
                        id: p2p_file_download_progress

                        color: FluTheme.dark ? FluTheme.primaryColor.lighter : FluTheme.primaryColor.dark
                        height: 6
                        radius: 3
                        width: 0
                    }
                }
                FluText {
                    id: p2p_file_download_progress_text

                    anchors.horizontalCenter: parent.horizontalCenter
                    color: FluColors.Grey100
                    font.pixelSize: 12
                }
                FluTextButton {
                    text: "取消下载"
                    width: parent.width

                    onClicked: {
                        // store.control.cancelP2PFile()
                        p2p_file_download_progress_popup.visible = false;
                    }
                }
                Connections {
                    // target: store

                    function onReceiveSizeChanged() {
                    // p2p_file_download_progress_popup.visible = true
                    // var receiveSize = store.receiveSize
                    // var fileSize = store.fileSize
                    // var progress = receiveSize / fileSize
                    // console.log(receiveSize, fileSize, progress)
                    // p2p_file_download_progress.width = p2p_file_download_progress_bar.width * progress
                    // p2p_file_download_progress_text.text = "已下载：" + (receiveSize / 1024 / 1024).toFixed(2) + "MB / " + (fileSize / 1024 / 1024).toFixed(2) + "MB (" + (progress * 100).toFixed(2) + "%)"
                    // if (receiveSize === fileSize) {
                    //     p2p_file_download_progress_popup.visible = false
                    //     showSuccess("下载完成")
                    //     store.recieveSize = 0
                    // }
                    }
                }
            }
        }
    }
    Popup {
        id: image_viwer

        anchors.centerIn: Overlay.overlay
        height: Overlay.overlay.height
        modal: true
        opacity: 0
        visible: false
        width: Overlay.overlay.width

        background: Rectangle {
            color: "transparent"
        }
        enter: Transition {
            NumberAnimation {
                from: 0.0
                property: "opacity"
                to: 1.0
            }
        }
        exit: Transition {
            NumberAnimation {
                from: 1.0
                property: "opacity"
                to: 0.0
            }
        }

        Image {
            id: image_viwer_image

            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
            source: ""
            z: 2333

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    image_viwer_image.source = "";
                    image_viwer.visible = false;
                }
            }
        }
    }
    FluTour {
        id: tour

        steps: [
            {
                title: "聊天名称",
                description: "这里展示了聊天对象或群名，如果有备注将会显示备注。群聊还会有括号显示群人数。",
                target: () => header_title
            },
            {
                title: "编辑备注",
                description: "点击这里可以编辑聊天备注。",
                target: () => header_edit_button
            },
            {
                title: "更多信息",
                description: "点击这里可以查看群聊详情，其中展示了群聊信息、每个成员的昵称、用户名和在线情况等。",
                target: () => header_info_button
            }
        ]
    }
    // Timer {
    //     id: tour_delay
    //
    //     interval: 500
    //     repeat: false
    //
    //     onTriggered: {
    //         tour.open();
    //     }
    // }
}
