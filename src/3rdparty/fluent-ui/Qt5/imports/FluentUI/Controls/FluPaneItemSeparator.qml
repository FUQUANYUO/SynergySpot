import QtQuick 2.15
import QtQuick.Controls 2.15
import FluentUI 1.0

QtObject {
    property string key
    property int _idx
    property bool visible: true
    property var parent
    property real spacing
    property int size:1
    Component.onCompleted: {
        key = FluTools.uuid()
    }
}
