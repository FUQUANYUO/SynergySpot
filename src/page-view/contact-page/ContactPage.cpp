//
// Created by FU-QAQ on 2024/12/11.
//

#include "ContactPage.h"
#include "contact-delegate/ContactDelegate.h"
#include "contact-model/ContactModel.h"

#include "ela-widget-tools/ElaTreeView.h"
#include "ela-widget-tools/ElaToolButton.h"
#include "ela-widget-tools/ElaPivot.h"

#include "common-data/CommonData.h"

#include <QHeaderView>
#include <QVBoxLayout>
#include <QStandardItem>
#include <mutex>
#include <ela-widget-tools/ElaTheme.h>


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
    if (_friendModel != nullptr && !_groupingInfos["friend"].contains(name)) {
        _friendModel->addGrouping(name);
    }
    _groupingInfos["friend"].insert(name,{});
}

QList<QString> ContactPage::getFriendGrouping() {
    return _groupingInfos["friend"].keys().toList();
}

bool ContactPage::loadCacheContact(const QList<FriendshipDTO> &caches) {
    for (auto &it : caches) {
        bool isGroup = (it.friendType==2);
        if (!isGroup) {
            UserBaseInfoDTO uInfo = g_pCommonData->getUserInfoBySSID(it.friendSSID);
            addFriendGrouping(it.groupingName);
            addContactInfo(it.groupingName,{
                uInfo,{},{},
                "",-1,false
            });
        }else {
            GroupBaseInfoDTO gInfo = g_pCommonData->getGroupInfoDataBySSID(it.friendSSID);
            QList<GroupMemberInfoDTO> members = g_pCommonData->getGroupMemberInfoData(it.friendSSID,100,1);
            addContactInfo(it.groupingName,{
                {},gInfo,members,
                "",-1,true
            });
        }
    }
    return true;
}

ElaTreeView * ContactPage::getFriendTreeView() {
    auto cpObj = new ElaTreeView();
    ContactModel* copiedModel = _friendModel->deepCopy(this);
    cpObj->header()->setVisible(false);
    cpObj->setModel(copiedModel);
    cpObj->setItemDelegate(new ContactDelegate);
    cpObj->setAcceptDrops(true);
    cpObj->setDragEnabled(true);
    cpObj->setDragDropMode(QAbstractItemView::InternalMove);
    cpObj->setSelectionMode(QAbstractItemView::SingleSelection);
    cpObj->setDropIndicatorShown(true);
    cpObj->setEditTriggers(QTreeView::NoEditTriggers);

    // double-clicked add msg card and change to msg page
    connect(cpObj,&QTreeView::doubleClicked,[=](const QModelIndex &index) {
        if (index.parent().isValid()) {
            QString clickedSSID = index.data(ContactDelegate::SSIDRole).toString();
            if (clickedSSID.isEmpty()) return;
            emit sigTriggerAddToCreateGroupList(_ssidToCardInfoHash.value(clickedSSID).userBaseInfo);
        }
    });

    return cpObj;
}

void ContactPage::addContactInfo(const QString& groupingName,const MsgCombineDTO &info) {
    if (!info.isGroup) {
        if (!_groupingInfos["friend"].contains(groupingName)) {
            addFriendGrouping(groupingName);
        }
        QString avatarPath;
        if (info.userBaseInfo.avatarPath.isEmpty() || info.userBaseInfo.avatarPath == "-1") {
            avatarPath = ":/contact-page/rc-page/img/SS-default-icon.jpg";
        }else {
            avatarPath = info.userBaseInfo.avatarPath;
        }
        _friendModel->delGroupingItem(groupingName,{
            info.userBaseInfo.ssid,info.userBaseInfo.username,"","","离线",avatarPath
        });
        _friendModel->addGroupingItem(groupingName,{
            info.userBaseInfo.ssid,info.userBaseInfo.username,"","","离线",avatarPath
        });
        _groupingInfos["friend"][groupingName].append(info);
        _ssidToCardInfoHash[info.userBaseInfo.ssid] = info;
    }else {
        QString avatarPath;
        if (info.userBaseInfo.avatarPath.isEmpty() || info.userBaseInfo.avatarPath == "-1") {
            avatarPath = ":/contact-page/rc-page/img/SS-default-icon.jpg";
        }else {
            avatarPath = info.userBaseInfo.avatarPath;
        }
        _groupModel->delGroupingItem(groupingName,{
                        info.groupBaseInfo.ssidGroup,info.groupBaseInfo.groupName,"","","离线",avatarPath
                    });
        _groupModel->addGroupingItem(groupingName,{
                        info.groupBaseInfo.ssidGroup,info.groupBaseInfo.groupName,"","","离线",avatarPath
                    });
        _groupingInfos["group"][groupingName].append(info);
        _ssidToCardInfoHash[info.groupBaseInfo.ssidGroup] = info;
    }
}

ContactPage::ContactPage(QWidget *parent) : ElaScrollPage(parent) {
    initWindow();

    initEdgeLayout();

    initContent();

    initConnectFunc();
}
ContactPage::~ContactPage() {
    _noticePage->deleteLater();
}

void ContactPage::initWindow() {
    _centralWidget         = new QWidget(this);
    _centralWidLayout      = new QVBoxLayout;
    _friendNoticeButton    = new ElaToolButton(this);
    _groupNoticeButton     = new ElaToolButton(this);
    _friendOrGroupPivot    = new ElaPivot(this);
    _friendTree            = new ElaTreeView(this);
    _groupTree             = new ElaTreeView(this);
    _noticePage            = new NoticePage();
    _scrollTreeLayout      = new QVBoxLayout;
}

void ContactPage::initEdgeLayout() {
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
    ContactDelegate * fDelegate = new ContactDelegate(this);
    ContactDelegate * gDelegate = new ContactDelegate(this);
    gDelegate->setGroupFlag();

    _groupModel->addGrouping("未命名群聊");
    _groupModel->addGrouping("我创建的群聊");
    _groupModel->addGrouping("我管理的群聊");
    _groupModel->addGrouping("我加入的群聊");

    _friendTree->setModel(_friendModel);
    _friendTree->setAcceptDrops(true);
    _friendTree->setDragEnabled(true);
    _friendTree->setDragDropMode(QAbstractItemView::InternalMove);
    _friendTree->setSelectionMode(QAbstractItemView::SingleSelection);
    _friendTree->setDropIndicatorShown(true);
    _friendTree->setItemDelegate(fDelegate);
    _friendTree->setEditTriggers(QTreeView::NoEditTriggers);

    _groupTree->hide();
    _groupTree->setModel(_groupModel);
    _groupTree->setAcceptDrops(true);
    _groupTree->setDragEnabled(true);
    _groupTree->setDragDropMode(QAbstractItemView::InternalMove);
    _groupTree->setSelectionMode(QAbstractItemView::SingleSelection);
    _groupTree->setDropIndicatorShown(true);
    _groupTree->setItemDelegate(gDelegate);
    _groupTree->setEditTriggers(QTreeView::NoEditTriggers);

    _groupTree->setAutoFillBackground(false);
    _groupTree->viewport()->setAutoFillBackground(false);
    _friendTree->setAutoFillBackground(false);
    _friendTree->viewport()->setAutoFillBackground(false);

    connect(fDelegate, &ContactDelegate::sigAddGrouping,this,[=](const QString& groupingName) {
        _friendModel->addGrouping(groupingName);
    });
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
            QString clickedSSID = index.data(ContactDelegate::SSIDRole).toString();
            if (clickedSSID.isEmpty()) return;
            emit sigTriggerAddMsgCard(_ssidToCardInfoHash.value(clickedSSID));
        }
    });
    connect(_groupTree,&QTreeView::doubleClicked,[=](const QModelIndex &index) {
        if (index.parent().isValid()) {
            QString clickedSSID = index.data(ContactDelegate::SSIDRole).toString();
            if (clickedSSID.isEmpty()) return;
            emit sigTriggerAddMsgCard(_ssidToCardInfoHash.value(clickedSSID));
        }
    });

    connect(_noticePage,&NoticePage::sigHideArchPageMaskEffect,this,&ContactPage::sigHideArchPageMaskEffect);

    connect(_friendNoticeButton,&ElaToolButton::clicked,[=]() {
        emit sigShowArchPageMaskEffect();
        _noticePage->show();
    });

    connect(this, &ContactPage::sigCommunicateRequestBySSID,this,[=](const QString& ssid) {
        emit sigTriggerAddMsgCard(_ssidToCardInfoHash.value(ssid));
    });

    connect(_friendModel, &ContactModel::sigUpdateFriendshipGrouping, this, &ContactPage::sigUpdateFriendshipGrouping);

    connect(this,&ContactPage::sigAddMakeFriendRecord,_noticePage,&NoticePage::sltMakeFriendRecord);
    connect(this,&ContactPage::sigAddJoinGroupRecord,_noticePage,&NoticePage::sltJoinGroupRecord);
}
