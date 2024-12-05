import QtQuick 2.15
import QtQuick.Controls 2.15

QtObject {
    id: chatListTest

    // 数据源，供 ListView 使用
    property var items: ListModel {
        id: chatListModel
    }

    // 每个聊天项的高度
    property int chat_item_height: 40

    // 数据变化信号
    signal listUpdated()

    Component.onCompleted: {
        // 初始化数据
        chatListModel.append({id: "10001", name: "Chat 1" ,type: "twin"});
        chatListModel.append({id: "10002", name: "Chat 2" ,type: "other"});
        chatListModel.append({id: "10003", name: "Chat 3" ,type: "twin"});
    }

    // 添加聊天项
    function addItem(id,name,type) {
        chatListModel.append({id: id, name: name , type: type});
        listUpdated(); // 通知外部数据变化
    }

    // 移除聊天项
    function removeItem(index) {
        if (index >= 0 && index < chatListModel.count) {
            chatListModel.remove(index);
            listUpdated(); // 通知外部数据变化
        }
    }

    // 更新聊天项
    function updateItem(index, id,name,type) {
        if (index >= 0 && index < chatListModel.count) {
            chatListModel.set(index,{id: id, name: name , type: type});
            listUpdated(); // 通知外部数据变化
        }
    }
}
