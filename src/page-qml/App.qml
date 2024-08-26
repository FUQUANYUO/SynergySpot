import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import FluentUI

FluLauncher {
    id: app

    Component.onCompleted: {
        FluApp.init(app);
        FluApp.windowIcon = "qrc:/land-page/rc-page/img/logo.ico";
        FluRouter.routes = {
            "/": "qrc:/land-page/LandPage.qml",
            "/register": "qrc:/land-page/register-page/RegisterPage.qml",
            "/recover-password": "qrc:/land-page/recover-pw-page/RecoverPWPage.qml",
            "/arch-page": "qrc:/arch-page/ArchPage.qml"
        };
        FluRouter.navigate("/arch-page");
    }
}
