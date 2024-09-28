import QtQuick 2.15
import QtQuick.Layouts
import QtQuick.Window 2.15
import Qt5Compat.GraphicalEffects
import FluentUI 1.0
import "qrc:/message-container-page/chat-page"
import ".."

FluWindow {
    id: window

    fitsAppBarWindows: false
    height: 668
    launchMode: FluWindowType.SingleTask
    minimumHeight: 320
    minimumWidth: 668
    title: "FluentUI"
    width: 1000

    appBar: FluAppBar {
        id: title_bar

        //darkClickListener:(button)=>handleDarkChanged(button)
        closeClickListener: () => {
            dialog_close.open();
        }
        showDark: true
        showMaximize: false

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
    }

    Component.onCompleted: {
        nav_view.displayMode = FluNavigationViewType.Compact;
        nav_view.hideNavAppBar = true;
    }

    FluContentDialog {
        id: dialog_close

        buttonFlags: FluContentDialogType.NegativeButton | FluContentDialogType.NeutralButton | FluContentDialogType.PositiveButton
        message: qsTr("Are you sure you want to exit the program?")
        negativeText: qsTr("Minimize")
        neutralText: qsTr("Cancel")
        positiveText: qsTr("Quit")
        title: qsTr("Quit")

        onNegativeClicked: {
            system_tray.showMessage(qsTr("Friendly Reminder"), qsTr("FluentUI is hidden from the tray, click on the tray to activate the window again"));
            timer_window_hide_delay.restart();
        }
        onPositiveClicked: {
            FluRouter.exit(0);
        }
    }
    Component {
        id: nav_item_right_menu

        FluMenu {
            width: 186

            FluMenuItem {
                font: FluTextStyle.Caption
                text: qsTr("Open in Separate Window")
            }
        }
    }
    FluNavigationView {
        id: nav_view
        navCompactWidth: Screen.width * 0.04625
        width: parent.width
        height: parent.height

        items: TabItemPage

        Component.onCompleted: {
            TabItemPage.navigationView = nav_view;
            TabItemPage.paneItemMenu = nav_item_right_menu;
            buttonBack.visible = false;
            setCurrentIndex(1);
        }
    }
}
