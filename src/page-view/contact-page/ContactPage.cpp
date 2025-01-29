//
// Created by FU-QAQ on 2024/12/11.
//

#include "ContactPage.h"
#include "contact-delegate/ContactDelegate.h"
#include "contact-model/ContactModel.h"

#include "ela-widget-tools/ElaTreeView.h"
#include "ela-widget-tools/ElaToolButton.h"
#include "ela-widget-tools/ElaPivot.h"

#include <QVBoxLayout>
#include <QStandardItem>
#include <mutex>

ContactPage * ContactPage::_contactPage = nullptr;
static std::mutex m;

ContactPage *ContactPage::getInstance() {
    if(_contactPage == nullptr){
        m.lock();
        if(_contactPage == nullptr){
            _contactPage = new ContactPage();
        }
        m.unlock();
    }
    return _contactPage;
}

void ContactPage::destroyContactPage() {
    if(_contactPage == nullptr)
        return ;
    else{
        m.lock();
        delete _contactPage;
        _contactPage = nullptr;
        m.unlock();
    }
}

void ContactPage::addFriendGrouping(const QString &name) {
    if (_friendModel != nullptr)
        _friendModel->addGrouping(name);
    _groupingInfos["friend"].insert(name,{});
}

QList<QString> ContactPage::getFriendGrouping() {
    return _groupingInfos["friend"].keys().toList();
}

void ContactPage::addContactInfo(const QString& groupingName,const MsgCardInfo &info) {
    if (!info.isGroup) {
        if (_groupingInfos["friend"].contains(groupingName)) {
            _friendModel->addGroupingItem(groupingName,{
                info.ssid,info.name,info.mark,"","离线",QPixmap(info.avatar)
            });
            _groupingInfos["friend"][groupingName].append(info);
            _ssidToCardInfoHash[info.ssid] = info;
        }
    }else {
        _groupModel->addGroupingItem(groupingName,{
                        info.ssid,info.name,info.mark,"","离线",QPixmap(info.avatar)
                    });
        _groupingInfos["group"][groupingName].append(info);
        _ssidToCardInfoHash[info.ssid] = info;
    }
}

ContactPage::ContactPage(QWidget *parent) : ElaScrollPage(parent) {
    initWindow();

    initEdgeLayout();

    initContent();

    initConnectFunc();
}
ContactPage::~ContactPage() {
}

void ContactPage::initWindow() {
    _centralWidget         = new QWidget(this);
    _centralWidLayout      = new QVBoxLayout;
    _friendMangerContainer = new ElaToolButton(this);
    _friendNoticeButton    = new ElaToolButton(this);
    _groupNoticeButton     = new ElaToolButton(this);
    _friendOrGroupPivot    = new ElaPivot(this);
    _friendTree            = new ElaTreeView(this);
    _groupTree             = new ElaTreeView(this);
    _scrollTreeLayout      = new QVBoxLayout;
}

void ContactPage::initEdgeLayout() {
    _centralWidLayout->addWidget(_friendMangerContainer);
    _centralWidLayout->addWidget(_friendNoticeButton);
    _centralWidLayout->addWidget(_groupNoticeButton);
    _centralWidLayout->addWidget(_friendOrGroupPivot);
    _centralWidLayout->setContentsMargins(0,0,0,0);

    _scrollTreeLayout->addWidget(_friendTree);
    _scrollTreeLayout->addWidget(_groupTree);
    _scrollTreeLayout->setContentsMargins(0,0,0,0);

    _centralWidLayout->addItem(_scrollTreeLayout);

    _centralWidget->setLayout(_centralWidLayout);
    _centralWidget->setContentsMargins(0,0,20,0);
    addCentralWidget(_centralWidget,true,true,0);
}

void ContactPage::initContent() {
    setTitleVisible(false);

    _friendMangerContainer->setIsTransparent(false);
    _friendMangerContainer->setElaIcon(ElaIconType::UserGear);
    _friendMangerContainer->setText("好友管理器");
    _friendMangerContainer->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    _friendMangerContainer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    _friendMangerContainer->setFixedHeight(40);

    _friendNoticeButton->setElaIcon(ElaIconType::AngleRight);
    _friendNoticeButton->setText("好友通知");
    _friendNoticeButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    _friendNoticeButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    _friendNoticeButton->setStyleSheet("ElaToolButton");
    _friendNoticeButton->setFixedHeight(40);

    _groupNoticeButton->setElaIcon(ElaIconType::AngleRight);
    _groupNoticeButton->setText("群通知");
    _groupNoticeButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    _groupNoticeButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    _groupNoticeButton->setStyleSheet("ElaToolButton");
    _groupNoticeButton->setFixedHeight(40);

    _friendOrGroupPivot->setTextPixelSize(18);
    _friendOrGroupPivot->setPivotSpacing(10);
    _friendOrGroupPivot->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    _friendOrGroupPivot->appendPivot("好友");
    _friendOrGroupPivot->appendPivot("群组");
    _friendOrGroupPivot->setPivotSpacing(150);
    _friendOrGroupPivot->setFixedHeight(40);
    _friendOrGroupPivot->setCurrentIndex(0);

    // init model
    // grouping
    _friendModel = new ContactModel(this);
    _groupModel = new ContactModel(this);
    _groupingInfos["friend"] = {};
    _groupingInfos["group"] = {};
    ContactDelegate * cDelegate = new ContactDelegate(this);

    _groupModel->addGrouping("未命名群聊");
    _groupModel->addGrouping("我创建的群聊");
    _groupModel->addGrouping("我管理的群聊");
    _groupModel->addGrouping("我加入的群聊");

    _friendTree->setModel(_friendModel);
    _friendTree->setItemDelegate(cDelegate);
    _friendTree->setEditTriggers(QTreeView::NoEditTriggers);

    _groupTree->hide();
    _groupTree->setModel(_groupModel);
    _groupTree->setItemDelegate(cDelegate);
    _groupTree->setEditTriggers(QTreeView::NoEditTriggers);
}

void ContactPage::initConnectFunc() {
    // Pivot change
    connect(_friendOrGroupPivot,&ElaPivot::pivotClicked,[=](int index){
        // friend or group
        if(index == 0){
            _groupTree->hide();
            _friendTree->show();
        }else{
            _friendTree->hide();
            _groupTree->show();
        }
    });

    // double-clicked add msg card and change to msg page
    connect(_friendTree,&QTreeView::doubleClicked,[=](const QModelIndex &index) {
        if (index.parent().isValid()) {
            QString clickedSSID = index.data(Qt::UserRole + 1).toString();
            emit sigTriggerAddMsgCard(_ssidToCardInfoHash.value(clickedSSID));
        }
    });
    connect(_groupTree,&QTreeView::doubleClicked,[=](const QModelIndex &index) {
        if (index.parent().isValid()) {
            QString clickedSSID = index.data(Qt::UserRole + 1).toString();
            emit sigTriggerAddMsgCard(_ssidToCardInfoHash.value(clickedSSID));
        }
    });
}
