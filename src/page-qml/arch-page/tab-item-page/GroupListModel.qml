// GroupListModel.qml
import QtQuick 2.15

ListModel {
    id: groupListModel

    property var currentItem: null

    function setCurrentItem(item) {
        currentItem = item;
    }
    function setItems(newItems) {
        groupListModel.clear();
        for (var i = 0; i < newItems.length; i++) {
            groupListModel.append(Object.assign({}, newItems[i], {
                onTapListener: newItems[i].onTapListener
            }));
        }
    }

    Component.onCompleted: {
        // 初始化一些数据
        setItems([
            {
                id: 1,
                type: "group",
                name: "Group 1",
                avatar: "2",
                color: "1",
                remark: "1",
                owner: null,
                last: null,
                read: 0,
                unreadNum: 10,
                onTapListener: function () {
                    console.log("Group 1 tapped");
                }
            }
            // ... 更多的初始项 ...
            ,
        ]);
    }
}