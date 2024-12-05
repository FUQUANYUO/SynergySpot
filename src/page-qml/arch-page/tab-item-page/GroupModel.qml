// GroupModel.qml
import QtQuick 2.15

QtObject {
    property string avatar: ""
    property string color: ""
    property int id: 0
    property var last: null // 假设 MessageModel 是一个 QML 对象
    property string name: ""
    property var owner: null // 假设 UserModel 是一个 QML 对象
    property int read: 0
    property string remark: ""
    property string type: ""
    property int unreadNum: 0

    // 定义 onTapListener 函数
    function onTapListener() {
        // 你的逻辑代码
        console.log("onTapListener called for item with id:", id);
    }
}