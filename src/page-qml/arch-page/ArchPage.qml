import QtQuick 2.15
import QtQuick.Layouts
import QtQuick.Window 2.15
import Qt5Compat.GraphicalEffects
import FluentUI 1.0
import CircularReveal
import ".."

// FluWindow 是 FluentUI 提供的主窗口组件
FluWindow {
    id: window

    // 设置窗口属性
    fitsAppBarWindows: true  // 设置窗口是否适应 AppBar
    height: 668  // 窗口高度
    launchMode: FluWindowType.SingleTask  // 设置窗口启动模式为单任务
    minimumHeight: 320  // 最小高度
    minimumWidth: 668  // 最小宽度
    title: "FluentUI"  // 窗口标题
    width: 1000  // 窗口宽度

    // 顶部的应用栏 AppBar
    appBar: FluAppBar {
        id: title_bar

        // 关闭按钮点击事件，打开关闭确认对话框
        closeClickListener: () => {
            dialog_close.open();  // 打开关闭确认对话框
        }

        showDark: true  // 显示深色模式切换按钮
        showMaximize: false  // 不显示最大化按钮

        // 深色模式切换按钮点击事件
        darkClickListener: (button) => handleDarkChanged(button)

        // 定位 AppBar
        anchors {
            left: parent.left  // 左侧定位到父项
            right: parent.right  // 右侧定位到父项
            top: parent.top  // 顶部定位到父项
        }
    }

    // 当组件加载完成后，设置导航视图属性
    Component.onCompleted: {
        nav_view.displayMode = FluNavigationViewType.Compact;  // 设置导航视图为紧凑模式
    }

    // 退出确认对话框
    FluContentDialog {
        id: dialog_close

        // 设置按钮类型为：负面、中立、正面
        buttonFlags: FluContentDialogType.NegativeButton | FluContentDialogType.NeutralButton | FluContentDialogType.PositiveButton
        message: qsTr("Are you sure you want to exit the program?")  // 对话框消息
        negativeText: qsTr("Minimize")  // 负面按钮文本
        neutralText: qsTr("Cancel")  // 中立按钮文本
        positiveText: qsTr("Quit")  // 正面按钮文本
        title: qsTr("Quit")  // 对话框标题

        // 负面按钮点击事件：最小化窗口
        onNegativeClicked: {
            system_tray.showMessage(qsTr("Friendly Reminder"), qsTr("FluentUI is hidden from the tray, click on the tray to activate the window again"));
            timer_window_hide_delay.restart();  // 重新启动隐藏窗口延迟计时器
        }

        // 正面按钮点击事件：退出程序
        onPositiveClicked: {
            FluRouter.exit(0);
        }
    }

    // 右键菜单组件
    Component {
        id: nav_item_right_menu

        FluMenu {
            width: 186  // 菜单宽度

            // 菜单项 "Open in Separate Window"
            FluMenuItem {
                font: FluTextStyle.Caption  // 设置字体样式
                text: qsTr("Open in Separate Window")
                // 在这里处理菜单项点击事件（可选）
            }
        }
    }

    // 导航视图组件
    FluNavigationView {
        id: nav_view
        navCompactWidth: 80  // 设置导航视图在紧凑模式下的宽度
        cellHeight: 70  // 设置导航项高度
        width: parent.width  // 宽度与父级窗口一致
        height: parent.height  // 高度与父级窗口一致
        hideNavAppBar : true

        // 页面切换模式：无栈模式（可选）
        // pageMode: FluNavigationViewType.NoStack  // 使用无栈模式，销毁之前的页面以节省内存
        items: TabItemPage  // 导航视图的项目来源于 TabItemPage

        Component.onCompleted: {
            TabItemPage.navigationView = nav_view;  // 将导航视图赋值给 TabItemPage
            TabItemPage.paneItemMenu = nav_item_right_menu;  // 将右键菜单赋值给 TabItemPage
            buttonBack.visible = false;  // 隐藏返回按钮
            setCurrentIndex(1);  // 设置当前选中项的索引为1
        }
    }

    // 圆形揭示动画组件 CircularReveal
    Component {
        id: com_reveal
        CircularReveal {
            id: reveal
            target: window.containerItem()  // 设置动画目标为窗口内容项
            anchors.fill: parent  // 填满父级
            onAnimationFinished: {
                loader_reveal.sourceComponent = undefined  // 动画结束后释放资源
            }
            onImageChanged: {
                changeDark()  // 当图片改变时，切换主题
            }
        }
    }

    // 动画加载器组件 FluLoader，用于动态加载 CircularReveal
    FluLoader {
        id: loader_reveal
        anchors.fill: parent  // 填满父级
    }

    // 计算两点之间的距离
    function distance(x1, y1, x2, y2) {
        return Math.sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2))
    }

    // 处理深色模式切换
    function handleDarkChanged(button) {
        // if (!FluTheme.animationEnabled || window.fitsAppBarWindows === false) {
        //     changeDark()  // 如果动画不可用或不适应 AppBar，直接切换主题
        // } else {
            if (loader_reveal.sourceComponent) {
                return  // 如果动画正在加载，直接返回
            }
        loader_reveal.sourceComponent = com_reveal
        var target = window.containerItem()
        var pos = button.mapToItem(target,0,0)
        var mouseX = pos.x + button.width / 2
        var mouseY = pos.y + button.height / 2
        var radius = Math.max(distance(mouseX,mouseY,0,0),distance(mouseX,mouseY,target.width,0),distance(mouseX,mouseY,0,target.height),distance(mouseX,mouseY,target.width,target.height))
        var reveal = loader_reveal.item
        reveal.start(reveal.width*Screen.devicePixelRatio,reveal.height*Screen.devicePixelRatio,Qt.point(mouseX,mouseY),radius)

    }

    // 切换深色模式
    function changeDark() {
        if (FluTheme.dark) {
            FluTheme.darkMode = FluThemeType.Light  // 切换到浅色模式
        } else {
            FluTheme.darkMode = FluThemeType.Dark  // 切换到深色模式
        }
    }
}
