import QtQuick 2.15
import QtQuick.Layouts
import QtQuick.Window 2.15
import Qt5Compat.GraphicalEffects
import FluentUI 1.0
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
                // onClicked: {
                //     FluRouter.navigate("/pageWindow",{title:modelData.title,url:modelData.url})
                // }
                text: qsTr("Open in Separate Window")
            }
        }
    }
    FluNavigationView {
        id: nav_view
        navCompactWidth: Screen.width * 0.04625
        width: parent.width
        height: parent.height

        //imageLogo.Layout.topMargin: window.appBar.height

        // //Stack模式，每次切换都会将页面压入栈中，随着栈的页面增多，消耗的内存也越多，内存消耗多就会卡顿，这时候就需要按返回将页面pop掉，释放内存。该模式可以配合FluPage中的launchMode属性，设置页面的启动模式
        // //                pageMode: FluNavigationViewType.Stack
        // //NoStack模式，每次切换都会销毁之前的页面然后创建一个新的页面，只需消耗少量内存
        // pageMode: FluNavigationViewType.NoStack
        items: TabItemPage

        Component.onCompleted: {
            TabItemPage.navigationView = nav_view;
            TabItemPage.paneItemMenu = nav_item_right_menu;
            buttonBack.visible = false;
            setCurrentIndex(1);
        }
    }
    // Component{
    //         id: com_reveal
    //         CircularReveal{
    //             id: reveal
    //             target: window.containerItem()
    //             anchors.fill: parent
    //             darkToLight: FluTheme.dark
    //             onAnimationFinished:{
    //                 //动画结束后释放资源
    //                 loader_reveal.sourceComponent = undefined
    //             }
    //             onImageChanged: {
    //                 changeDark()
    //             }
    //         }
    //     }
    // FluLoader{
    //     id:loader_reveal
    //     anchors.fill: parent
    // }
    // function handleDarkChanged(button){
    //     if(FluTools.isMacos() || !FluTheme.animationEnabled){
    //         changeDark()
    //     }else{
    //         loader_reveal.sourceComponent = com_reveal
    //         var target = window.containerItem()
    //         var pos = button.mapToItem(target,0,0)
    //         var mouseX = pos.x + button.width / 2
    //         var mouseY = pos.y + button.height / 2
    //         var radius = Math.max(distance(mouseX,mouseY,0,0),distance(mouseX,mouseY,target.width,0),distance(mouseX,mouseY,0,target.height),distance(mouseX,mouseY,target.width,target.height))
    //         var reveal = loader_reveal.item
    //         reveal.start(reveal.width*Screen.devicePixelRatio,reveal.height*Screen.devicePixelRatio,Qt.point(mouseX,mouseY),radius)
    //     }
    // }
}
