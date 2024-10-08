import QtQuick 2.15
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Controls.Basic
import QtQuick.Layouts
import FluentUI 1.0
import ".."
import "qrc:/component"

Item {
    id: container

    property Component autoSuggestBox
    property var chatList
    property double chat_item_height: 66

    Component {
        id: chat_item

        Item {
            clip: true
            height: chat_item_height
            visible: true
            width: layout_list.width

            FluControl {
                id: item_control

                // onClicked:
                // chat_list.currentIndex = _idx
                // store.control.openGroup(model)
                // {}

                anchors {
                    bottom: parent.bottom
                    bottomMargin: 2
                    left: parent.left
                    leftMargin: 6
                    right: parent.right
                    rightMargin: 6
                    top: parent.top
                    topMargin: 2
                }
                anchors.fill: parent
                // acceptedButtons: Qt.RightButton
                // onClicked: function (mouse) {
                //     if (mouse.button === Qt.RightButton) {
                //         if (model.menuDelegate) {
                //             loader_item_menu.sourceComponent = model.menuDelegate
                //             loader_item_menu.item.popup();
                //         }
                //     }
                // }
                z: -100
                MouseArea {
                }
                Rectangle {
                    anchors.fill: parent
                    color: {
                        if (FluTheme.dark) {
                            if (chat_list.currentIndex === _idx) {
                                return Qt.rgba(1, 1, 1, 0.06);
                            }
                            if (item_control.hovered) {
                                return Qt.rgba(1, 1, 1, 0.03);
                            }
                            return Qt.rgba(0, 0, 0, 0);
                        } else {
                            if (chat_list.currentIndex === _idx) {
                                return Qt.rgba(0, 0, 0, 0.06);
                            }
                            if (item_control.hovered) {
                                return Qt.rgba(0, 0, 0, 0.03);
                            }
                            return Qt.rgba(0, 0, 0, 0);
                        }
                    }
                    radius: 4

                    RandomAvatar {
                        id: item_avatar

                        // avatar: model.type === "twin" ? model.owner.avatar : model.avatar
                        // bgColor: model.type === "twin" ? model.owner.color : model.color
                        // online: model.type === "twin" ? model.owner.online : false
                        avatar: "😂"
                        bgColor: "#aff"
                        online: true
                        size: 42

                        anchors {
                            left: parent.left
                            leftMargin: 10
                            verticalCenter: parent.verticalCenter
                        }
                    }
                    FluText {
                        id: item_title

                        color: {
                            if (item_control.pressed) {
                                return FluTheme.dark ? FluColors.Grey80 : FluColors.Grey120;
                            }
                            return FluTheme.dark ? FluColors.White : FluColors.Grey220;
                        }
                        elide: Text.ElideRight
                        font.pixelSize: 16
                        maximumLineCount: 1
                        text: {
                            // if (model.remark) return model.remark;
                            // if (model.type === "twin") return model.owner.remark ? model.owner.remark : model.owner.nickname
                            // return model.name
                            return "AAA";
                        }

                        anchors {
                            left: item_avatar.right
                            leftMargin: 10
                            right: time_text.left
                            rightMargin: 5
                            verticalCenter: parent.verticalCenter
                            verticalCenterOffset: -10
                        }
                    }
                    FluText {
                        id: item_text

                        color: {
                            if (item_control.pressed) {
                                return FluTheme.dark ? FluColors.Grey120 : FluColors.Grey80;
                            }
                            return FluTheme.dark ? FluColors.Grey80 : FluColors.Grey120;
                        }
                        elide: Text.ElideRight
                        font.pixelSize: 12
                        maximumLineCount: 1
                        text: {
                            // if (!model.last) return ""
                            // let text = ""
                            // if (model.type !== "twin") text = model.last.user.remark ? model.last.user.remark : model.last.user.nickname + "："
                            // switch (model.last.type) {
                            //     case "text":
                            //         text += model.last.content
                            //         break
                            //     case "image":
                            //         text += "[图片]"
                            //         break
                            //     case "file":
                            //         text += "[文件]"
                            //         break
                            //     case "p2p_file":
                            //         text += "[P2P文件]"
                            //         break
                            // }
                            return text;
                        }

                        anchors {
                            left: item_avatar.right
                            leftMargin: 10
                            right: unread_badge.left
                            rightMargin: 5
                            verticalCenter: parent.verticalCenter
                            verticalCenterOffset: 10
                        }
                    }
                    FluText {
                        id: time_text

                        color: item_text.color
                        font.pixelSize: 10
                        // text: model.last ? GlobalTool.formatTime(model.last.time) : ""
                        text: "xxxx:xx:xx xx:xx"

                        anchors {
                            right: parent.right
                            rightMargin: 10
                            verticalCenter: parent.verticalCenter
                            verticalCenterOffset: -10
                        }
                    }
                    Rectangle {
                        id: unread_badge

                        border.width: 0
                        color: Qt.rgba(255 / 255, 77 / 255, 79 / 255, 1)
                        height: 20
                        radius: 10
                        visible: true
                        width: {
                            // if (model.unreadNum < 10) {
                            //     return 20;
                            // } else if (model.unreadNum < 100) {
                            //     return 30;
                            // }
                            return 35;
                        }

                        anchors {
                            right: parent.right
                            rightMargin: 10
                            verticalCenter: parent.verticalCenter
                            verticalCenterOffset: 10
                        }
                        Text {
                            color: Qt.rgba(1, 1, 1, 1)
                            font.bold: true
                            font.pixelSize: 10
                            text: {
                                // if (model.unreadNum < 100)
                                //     return model.unreadNum;
                                return "99+";
                            }

                            anchors {
                                horizontalCenter: parent.horizontalCenter
                                verticalCenter: parent.verticalCenter
                                verticalCenterOffset: 1
                            }
                        }
                    }
                }
            }
        }
    }
    Rectangle {
        id: layout_list

        border.color: FluTheme.dark ? Qt.rgba(45 / 255, 45 / 255, 45 / 255, 1) : Qt.rgba(226 / 255, 230 / 255, 234 / 255, 1)
        border.width: 1
        color: FluTheme.dark ? Window.active ? Qt.rgba(38 / 255, 44 / 255, 54 / 255, 1) : Qt.rgba(39 / 255, 39 / 255, 39 / 255, 1) : Qt.rgba(251 / 255, 251 / 255, 253 / 255, 1)
        width: parent.width

        anchors {
            bottom: parent.bottom
            top: parent.top
        }
        Item {
            id: layout_header

            height: 50
            width: layout_list.width

            FluAutoSuggestBox {
                iconSource: FluentIcons.Search
                items: {
                    // var groups = store.groupList.items;
                    // var result = [];
                    // for (var i = 0; i < groups.length; i++) {
                    //     var group = groups[i];
                    //     if (group.type === "twin") {
                    //         var user = group.owner;
                    //         if (user.nickname.indexOf(text) !== -1 || user.remark.indexOf(text) !== -1 || user.username.indexOf(text) !== -1) {
                    //             result.push({
                    //                 title: user.remark ? user.remark : user.nickname,
                    //                 key: group.id
                    //             });
                    //         }
                    //     } else {
                    //         if (group.name.indexOf(text) !== -1 || group.remark.indexOf(text) !== -1) {
                    //             result.push({
                    //                 title: group.remark ? user.remark : group.name,
                    //                 key: group.id
                    //             });
                    //         }
                    //     }
                    // }
                    var result = [
                        {
                            title: "AAA",
                            key: 111
                        }
                    ];
                    return result;
                }
                placeholderText: "搜索"

                onItemClicked: data => {}

                anchors {
                    left: parent.left
                    leftMargin: 15
                    right: add_button.left
                    rightMargin: 10
                    verticalCenter: parent.verticalCenter
                }
            }
            FluIconButton {
                id: add_button

                iconColor: FluTheme.dark ? FluTheme.primaryColor.lighter : FluTheme.primaryColor.dark
                iconSource: FluentIcons.AddBold

                onClicked: {
                    add_popup.visible = true;
                }

                anchors {
                    right: parent.right
                    rightMargin: 15
                    verticalCenter: parent.verticalCenter
                }
            }
            Popup {
                id: add_popup

                bottomInset: 5
                clip: true
                leftInset: 5
                modal: true
                rightInset: 5
                topInset: 5
                visible: false
                width: 150
                x: add_button.x - (add_popup.width - add_button.width) / 2
                y: add_button.y + add_button.height

                background: FluFrame {
                    border.width: 0
                    radius: 5
                }
                enter: Transition {
                    NumberAnimation {
                        duration: 233
                        easing.type: Easing.InOutExpo
                        from: 0
                        property: "height"
                        to: 155
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
                        from: 155
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

                Column {
                    id: add_popup_column

                    spacing: 5
                    width: parent.width

                    Repeater {
                        model: [
                            {
                                text: "添加好友",
                                icon: FluentIcons.AddFriend,
                                onClick: () => {
                                    add_user_dialog.visible = true;
                                }
                            },
                            {
                                text: "加入群聊",
                                icon: FluentIcons.ChatBubbles,
                                onClick: () => {
                                    add_group_dialog.visible = true;
                                }
                            },
                            {
                                text: "创建群聊",
                                icon: FluentIcons.VideoChat,
                                onClick: () => {
                                    create_group_dialog.visible = true;
                                }
                            },
                        ]

                        delegate: Item {
                            height: 40
                            width: add_popup_column.width

                            Rectangle {
                                id: add_popup_item

                                property bool hoverd: false

                                color: hoverd ? (FluTheme.dark ? "#11FFFFFF" : "#11000000") : "transparent"
                                height: parent.height
                                radius: 5
                                width: parent.width

                                MouseArea {
                                    anchors.fill: parent
                                    hoverEnabled: true

                                    onClicked: {
                                        add_popup.visible = false;
                                        modelData.onClick();
                                    }
                                    onEntered: {
                                        add_popup_item.hoverd = true;
                                    }
                                    onExited: {
                                        add_popup_item.hoverd = false;
                                    }
                                }
                                FluIcon {
                                    id: add_popup_icon

                                    iconColor: FluTheme.primaryColor.normal
                                    iconSize: 20

                                    // iconSource: modelData.icon

                                    anchors {
                                        left: parent.left
                                        leftMargin: 10
                                        verticalCenter: parent.verticalCenter
                                    }
                                }
                                FluText {
                                    id: add_popup_text

                                    font.pixelSize: 14
                                    text: modelData.text
                                    // text: "add popup text"

                                    anchors {
                                        left: add_popup_icon.right
                                        leftMargin: 10
                                        verticalCenter: parent.verticalCenter
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        Item {
            id: highlight_clip

            anchors.fill: chat_list
            clip: true

            Rectangle {
                id: highlight_rectangle

                property bool enableAnimation: true

                color: FluTheme.primaryColor.normal
                height: chat_item_height
                radius: width / 2
                // visible: store.groupList.length !== 0
                visible: true
                width: 4

                Behavior on height {
                    enabled: highlight_rectangle.enableAnimation & FluTheme.enableAnimation

                    NumberAnimation {
                        duration: 666
                        easing.amplitude: 1
                        easing.period: 0.75
                        easing.type: Easing.OutElastic
                    }
                }
                Behavior on y {
                    enabled: highlight_rectangle.enableAnimation & FluTheme.enableAnimation

                    NumberAnimation {
                        duration: 666
                        easing.amplitude: 1
                        easing.period: 0.75
                        easing.type: Easing.OutElastic
                    }
                }

                anchors {
                    left: parent.left
                    leftMargin: 6
                }
            }
        }
        ListView {
            id: chat_list

            property double lastContentY: 0
            property var lastTopItem

            boundsBehavior: Flickable.DragOverBounds
            clip: true
            currentIndex: -1

            model: chatList.items

            ScrollBar.vertical: FluScrollBar {
            }
            delegate: Loader {
                property var _idx: index
                property var model: modelData
                property int type: 0

                sourceComponent: chat_item
            }

            onContentYChanged: {
                var imm = (lastContentY - chat_list.contentY != 0.0); // 高亮是否关闭动画 用于滚动跟随
                if (chat_list.lastTopItem === chatList.items[0]) {
                    lastContentY = chat_list.contentY;
                } else
                    chat_list.lastTopItem = chatList.items[0];
                if (chat_list.currentIndex !== -1 && chat_list.currentItem) {
                    highlight_clip.clip = true;
                    if (imm)
                        highlight_rectangle.enableAnimation = false;
                    highlight_rectangle.height = chat_item_height * 0.5;
                    highlight_rectangle.y = chat_list.currentItem.y - chat_list.contentY + (chat_item_height - chat_item_height * 0.5) / 2;
                    if (imm)
                        highlight_rectangle.enableAnimation = true;
                }
            }
            onCurrentIndexChanged: {
                if (chat_list.currentIndex !== -1) {
                    highlight_clip.clip = true;
                    highlight_rectangle.height = chat_item_height * 0.5;
                    highlight_rectangle.y = chat_list.currentItem.y - chat_list.contentY + (chat_item_height - chat_item_height * 0.5) / 2;
                }
            }

            anchors {
                bottom: layout_footer.top
                left: parent.left
                right: parent.right
                top: layout_header.bottom
            }
            Connections {
                function onItemsChanged() {
                    updateList();
                }
                function updateList() {
                    for (var i = 0; i < chatList.items.length; i++) {
                        if (chatList.items[i] === chatList.currentItem) {
                            chat_list.currentIndex = i;
                        }
                    }

                    // 防止列表更新时滚轮自动移动到选中项
                    chat_list.contentY = chat_list.lastContentY;
                }

                // 监听数据源变化
                target: chatList
            }
        }
        Popup {
            id: add_user_dialog

            anchors.centerIn: Overlay.overlay
            height: 250
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
                    id: add_user_dialog_column

                    anchors.centerIn: parent
                    spacing: 10
                    width: 200

                    RandomAvatar {
                        id: add_user_avatar

                        anchors.horizontalCenter: parent.horizontalCenter
                        avatar: "🤗"
                        bgColor: "#aef"
                        online: true
                        size: 50
                    }
                    FluTextBox {
                        id: add_user_textbox

                        placeholderText: "用户名"

                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                    }
                    FluButton {
                        text: "添加好友"
                        width: parent.width

                        onClicked: {
                            if (add_user_textbox.text) {
                                // store.control.requestUser(add_user_textbox.text);
                                add_user_dialog.visible = false;
                            } else {
                                showError("用户名不能为空");
                            }
                        }
                    }
                }
            }
        }
        Popup {
            id: add_group_dialog

            anchors.centerIn: Overlay.overlay
            height: 250
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
                    id: add_group_dialog_column

                    anchors.centerIn: parent
                    spacing: 10
                    width: 200

                    RandomAvatar {
                        id: add_group_avatar

                        anchors.horizontalCenter: parent.horizontalCenter
                        avatar: "🥳"
                        bgColor: "#aef"
                        online: true
                        size: 50
                    }
                    FluTextBox {
                        id: add_group_textbox

                        placeholderText: "群号"

                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                    }
                    FluButton {
                        text: "加入群组"
                        width: parent.width

                        onClicked: {
                            if (add_group_textbox.text && !isNaN(add_group_textbox.text)) {
                                // store.control.requestGroup(add_group_textbox.text);
                                add_group_dialog.visible = false;
                            } else {
                                showError("群号不对哦");
                            }
                        }
                    }
                }
            }
        }
        Popup {
            id: create_group_dialog

            anchors.centerIn: Overlay.overlay
            height: 350
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
                    id: create_group_dialog_column

                    anchors.centerIn: parent
                    spacing: 10
                    width: 200

                    RandomAvatar {
                        anchors.horizontalCenter: parent.horizontalCenter
                        avatar: create_group_avatar.text
                        bgColor: create_group_color.colorValue
                        online: true
                        size: 50
                    }
                    FluTextBox {
                        id: create_group_name

                        placeholderText: "群名"
                        width: parent.width
                    }
                    FluTextBox {
                        id: create_group_avatar

                        placeholderText: "头像字（可为Emoji）"
                        width: parent.width
                    }
                    FluColorPicker {
                        id: create_group_color

                        width: parent.width

                        Component.onCompleted: {
                            create_group_color.colorValue = FluTheme.primaryColor.normal;
                        }

                        FluText {
                            anchors.centerIn: parent
                            color: "white"
                            text: "头像色"
                        }
                    }
                    FluButton {
                        text: "创建群组"
                        width: parent.width

                        onClicked: {
                            if (create_group_name.text) {
                                // store.control.createGroup(create_group_name.text, create_group_avatar.text, create_group_color.colorValue);
                                create_group_dialog.visible = false;
                            } else {
                                showError("群名不能为空");
                            }
                        }
                    }
                }
            }
        }
        Component.onCompleted: {
            chatList.

        }
    }
}
