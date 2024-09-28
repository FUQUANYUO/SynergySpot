pragma Singleton

import QtQuick 2.15
import FluentUI 1.0

FluObject{

    property var navigationView
    property var paneItemMenu

    id:footer_items

    FluPaneItemSeparator{}

    FluPaneItem{
        title:qsTr("About")
        icon:FluentIcons.Contact
        onTapListener:function(){
            FluRouter.navigate("/about")
        }
    }

    FluPaneItem{
        title:qsTr("Settings")
        menuDelegate: paneItemMenu
        icon:FluentIcons.Settings
        url:"qrc:/example/qml/page/T_Settings.qml"
        onTap:{
            navigationView.push(url)
        }
    }

    FluPaneItem{
        title:qsTr("FluentUI Pro")
        menuDelegate: paneItemMenu
        icon: FluentIcons.Airplane
        url:"qrc:/example/qml/page/T_FluentPro.qml"
        onTap:{
            navigationView.push(url)
        }
    }
}
