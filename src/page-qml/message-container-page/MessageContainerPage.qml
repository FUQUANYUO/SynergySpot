import QtQuick 2.15
import QtQuick.Layouts
import QtQuick.Window 2.15
import Qt5Compat.GraphicalEffects
import FluentUI 1.0
import ".."

FluNavigationView {
    id: message_page

    cellHeight: Screen.height * 0.09
    cellWidth: Screen.width * 0.21
    items: MessageItemPage

    autoSuggestBox: FluAutoSuggestBox {
        iconSource: FluentIcons.Search
        items: MessageItemPage.getSearchData()
        placeholderText: qsTr("Search")

        onItemClicked: data => {
            MessageItemPage.startPageByItem(data);
        }
    }

    Component.onCompleted: {
        MessageItemPage.navigationView = message_page;
        MessageItemPage.paneItemMenu = nav_item_right_menu;
        displayMode = FluNavigationViewType.Open;
        hideNavAppBar = true;
        setCurrentIndex(1);
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
}
