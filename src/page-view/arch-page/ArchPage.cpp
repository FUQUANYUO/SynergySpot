//
// Created by FU-QAQ on 2024/9/10.
//

#include "ArchPage.h"
#include "../CommonFunc.hpp"
#include "../about-page/AboutPage.h"
#include "../message-page/MessagePage.h"
#include "../contact-page/ContactPage.h"
#include "../settings-page/SettingsPage.h"
#include "../file-manager-page/FileManagerPage.h"
#include "../user-page/UserPage.h"
#include "../effect-component/SS-mask-widget/SSMaskWidget.h"

#include "ela-widget-tools/ElaContentDialog.h"
#include "ela-widget-tools/ElaStatusBar.h"
#include "ela-widget-tools/ElaText.h"
#include "ela-widget-tools/ElaToolBar.h"
#include "ela-widget-tools/ElaToolButton.h"
#include "ela-widget-tools/ElaSuggestBox.h"
#include "ela-widget-tools/ElaMenu.h"

#include <QHBoxLayout>
#include <QResizeEvent>
#include <mutex>

static std::mutex m;
ArchPage * ArchPage::_obj = nullptr;

ArchPage *ArchPage::getInstance() {
    if(!_obj){
        m.lock();
        if(!_obj){
            _obj = new ArchPage();
        }
        m.unlock();
    }
    return _obj;
}

void ArchPage::destroyInstance() {
    if(_obj){
        m.lock();
        if(_obj){
            _obj->deleteLater();
        }
        m.unlock();
    }
}

void ArchPage::sltShowMaskEffect() {
    _maskWidget->setVisible(true);
    _maskWidget->raise();
    _maskWidget->setFixedSize(this->size());
    _maskWidget->startMaskAnimation(90);
}

void ArchPage::sltHideMaskEffect() {
    _maskWidget->startMaskAnimation(0);
}

ArchPage::ArchPage(QWidget *parent) : ElaWindow(parent) {
    initWindow();

    initEdgeLayout();

    initContent();

    initConnectFunc();

    // intercept close event
    _closeDialog = new ElaContentDialog(this);
    connect(_closeDialog, &ElaContentDialog::rightButtonClicked, this, &ArchPage::closeWindow);
    connect(_closeDialog, &ElaContentDialog::middleButtonClicked, this, [=]() {
        _closeDialog->close();
        showMinimized();
    });
    this->setIsDefaultClosed(false);
    connect(this, &ArchPage::closeButtonClicked, this, [=]() {
        _closeDialog->exec();
    });

    moveToCenter();
}

ArchPage::~ArchPage() {
}

void ArchPage::initWindow() {
    setWindowIcon(QIcon(":/arch-page/rc-page/img/SS-default-icon.jpg"));
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    setMinimumSize(400,500);
    resize(800, 650);

    setUserInfoCardPixmap(QPixmap(":/arch-page/rc-page/img/SS-default-icon-flat.jpg"));
    setUserInfoCardTitle("{test name}");
    setUserInfoCardSubTitle("{ssid}");
    setWindowTitle("Synergy-Spot \t\t version:  " + QString(SS_VERSION));

    _statusBar      =   new ElaStatusBar(this);
    _statusText     =   new ElaText("初始化成功！", this);
    _toolBar        =   new ElaToolBar("Tool Bar", this);
    _addButton      =   new ElaToolButton(this);
    _searchSuggest  =   new ElaSuggestBox(this);
    _maskWidget     =   new SSMaskWidget(this);
}

void ArchPage::initEdgeLayout() {
    // status bar
    _statusText->setTextPixelSize(14);
    _statusBar->addWidget(_statusText);

    // toolbar
    _toolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    _toolBar->setToolBarSpacing(5);
    _toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    _toolBar->setIconSize(QSize(25,25));

    // suggest and add
    _searchSuggest->setMinimumWidth(200);
    _searchSuggest->setMaximumWidth(this->maximumWidth());
    _searchSuggest->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    _searchSuggest->setContentsMargins(0,0,0,0);

    _addButton->setFixedSize(60,40);
    _addButton->setBorderRadius(8);

    _toolBar->addWidget(_searchSuggest);
    _toolBar->addWidget(_addButton);

    addToolBar(Qt::TopToolBarArea, _toolBar);
}

void ArchPage::initContent() {
    // message page
    addPageNode("Message", g_pMessagePage,  _msgNoticeNum, ElaIconType::Comment);

    // contact page
    addPageNode("Contact", g_pContactPage, _contactNoticeNum, ElaIconType::User);

    // file manager
    addFooterNode("FileManager", g_pFileManagerPage, _fileManagerKey, 0, ElaIconType::Folders);

    // setting
    addFooterNode("Settings", g_pSettingsPage, _settingsKey, 0, ElaIconType::GearComplex);

    // about
    addFooterNode("About", nullptr, _aboutKey, 0, ElaIconType::CircleInfo);

    _addButton->setElaIcon(ElaIconType::CirclePlus);
    _addButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    _addButton->setIsTransparent(false);
    auto * addMenu      =   new ElaMenu(_addButton);
    _createAction       =   addMenu->addElaIconAction(ElaIconType::CommentPlus, "创建群聊");
    _addAction          =   addMenu->addElaIconAction(ElaIconType::UserPlus, "添加好友/群聊");
    _addButton->setMenu(addMenu);
    _searchSuggest->setPlaceholderText("Search...");

    setStatusBar(_statusBar);

    _maskWidget->setVisible(false);
    _maskWidget->move(0,0);
    _maskWidget->setFixedSize(this->size());
}

void ArchPage::initConnectFunc() {
    connect(this, &ElaWindow::navigationNodeClicked, this, [=](ElaNavigationType::NavigationNodeType nodeType, QString nodeKey) {
        if (_aboutKey == nodeKey)
        {
            g_pAboutPage->setFixedSize(400, 700);
            g_pAboutPage->moveToCenter();
            g_pAboutPage->show();
        }
    });
    connect(this, &ElaWindow::userInfoCardClicked, this, [=](){
        UserPage * wid = g_pUserPage(UserType::Myself,{},{});

        QPoint globalPos = QCursor::pos();
        wid->showAt(globalPos + QPoint{10,10});
    });

    // user edit page require mask effect
    connect(g_pUserPage(UserType::Myself,{},{}),&UserPage::sigShowArchPageMaskEffect,this,[=]() {
        sltShowMaskEffect();
        g_pUserPage(UserType::Myself,{},{})->moveUserEditPageToCenter(pos() + QPoint(width()/2,height()/2) - QPoint(200,300));
    });
    connect(g_pUserPage(UserType::Myself,{},{}),&UserPage::sigHideArchPageMaskEffect,this,&ArchPage::sltHideMaskEffect);
}

void ArchPage::resizeEvent(QResizeEvent *event) {
    ElaWindow::resizeEvent(event);
    _maskWidget->setFixedSize(event->size());
}
