//
// Created by FU-QAQ on 2024/12/11.
//

#include "MessagePage.h"
#include "conversation-page/ConversationPage.h"

#include "help.h"

#include "ela-widget-tools/ElaScrollPage.h"
#include "ela-widget-tools/ElaTabWidget.h"
#include "ela-widget-tools/ElaInteractiveCard.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QTabBar>
#include <QHash>
#include <QPointer>
#include <mutex>
#include <common-data/CommonData.h>

MessagePage * MessagePage::_messagePage = nullptr;
static std::mutex m;

MessagePage *MessagePage::getInstance() {
    if(_messagePage == nullptr){
        m.lock();
        if(_messagePage == nullptr)
            _messagePage = new MessagePage();
        m.unlock();
    }
    return _messagePage;
}

void MessagePage::destroyMessagePage() {
    if(_messagePage == nullptr)
        return;
    else {
        m.lock();
        delete _messagePage;
        _messagePage = nullptr;
        m.unlock();
    }
}

void MessagePage::addMsgCard(const MsgCardInfo &info) {
    QFont font;
    font.setPixelSize(9);
    if (!_ssidLinkCardHash.contains(info.ssid)) { // 没有添加
        ElaInteractiveCard * user = new ElaInteractiveCard(this);
        user->setTitle(info.name);
        user->setSubTitle(info.content);
        user->setCardPixmap(QPixmap(info.avatar));
        user->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        user->setFixedWidth(250);
        user->setFixedHeight(60);
        user->setCardPixmapSize(50,50);

        _tempMsgWidLayout->insertWidget(_tempMsgWidLayout->count()-1,user);
        _tmpUserMsgList[user] = info;
        _ssidLinkCardHash[info.ssid] = user;

        if (info.isGroup) {
            auto cP = new ConversationPage(Group, info, _conversionWid);
            cP->hide();
            _cardLinkPageHash[user] = cP;
            connect(user,&ElaInteractiveCard::clicked,this,[=]() {
               _unreadMsgCount[info.ssid] = 0;
               user->changeStatus(true);
               user->show();
               _conversionWid->addTab(_cardLinkPageHash[user],QPixmap(info.avatar),info.name);
           });
        }else {
            auto cP = new ConversationPage(Friend, info, _conversionWid);
            cP->hide();
            _cardLinkPageHash[user] = cP;
            connect(user,&ElaInteractiveCard::clicked,this,[=]() {
               _unreadMsgCount[info.ssid] = 0;
               user->changeStatus(true);
               user->show();
               _conversionWid->addTab(_cardLinkPageHash[user],QPixmap(info.avatar),info.name);
           });
        }
    }
    if (info.timestamp != 0) { // new msg come in
        int unreadCount = ++_unreadMsgCount[info.ssid];
        _ssidLinkCardHash[info.ssid]->setStatusContent((unreadCount>99?"99+":QString::number(unreadCount)),font,40);
        if (info.isGroup) { // 群组带用户名称
            _ssidLinkCardHash[info.ssid]->setSubTitle(info.name + "：" + info.content);
        }else {
            _ssidLinkCardHash[info.ssid]->setSubTitle(info.content);
        }
        _ssidLinkCardHash[info.ssid]->setTimeContent(QString::fromStdString(
        GetCurTime::getTimeObj()->getMsgTypeTime(static_cast<std::time_t>(info.timestamp))),Qt::gray,font);
    }
}


MessagePage::MessagePage(QWidget *parent) : QWidget(parent) {
    initWindow();

    initEdgeLayout();

    initContent();

    initConnectFunc();
}
MessagePage::~MessagePage() {

}

void MessagePage::initWindow(){
    setWindowFlags(Qt::FramelessWindowHint);
    _tempMsgList      = new ElaScrollPage(this);
    _conversionWid    = new ElaTabWidget(this);
    _tempMsgWid       = new QWidget(_tempMsgList);
    _tempMsgWidLayout = new QVBoxLayout(_tempMsgWid);
}

void MessagePage::initEdgeLayout() {
    _conversionWid->setContentsMargins(0,0,0,0);
    _conversionWid->setMaximumHeight(1000);
    _tempMsgList->setFixedWidth(250);
    _tempMsgList->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    _tempMsgList->setContentsMargins(0,0,0,0);

    QHBoxLayout * layout = new QHBoxLayout;
    layout->addWidget(_tempMsgList);
    layout->addWidget(_conversionWid);
    layout->setContentsMargins(0,0,0,0);

    _tempMsgWidLayout->setContentsMargins(0,0,0,0);
    _tempMsgWid->setLayout(_tempMsgWidLayout);
    _tempMsgWid->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    _tempMsgWid->setFixedWidth(250);
    _tempMsgWid->setMinimumHeight(700);
    _tempMsgWid->setMaximumHeight(2000);
    _tempMsgWid->setContentsMargins(0,20,0,0);

    setLayout(layout);
}

void MessagePage::initContent() {
    // TODO: load in local cache or cloud data
    _tempMsgWidLayout->addStretch();
    _conversionWid->setStyleSheet("ElaTabWidget::pane { border: 0px;}");
    _conversionWid->setIsTabTransparent(true);

    _tempMsgList->addCentralWidget(_tempMsgWid, true, true, 0);
    _tempMsgList->setTitleVisible(false);
}

void MessagePage::initConnectFunc() {
    for (auto it = _tmpUserMsgList.begin(); it != _tmpUserMsgList.end(); it++) {
        if (!it.value().isGroup) {
            auto cP = new ConversationPage(Friend, it.value(), _conversionWid);
            cP->hide();
            _cardLinkPageHash[it.key()] = cP;
            connect(it.key(),&ElaInteractiveCard::clicked,this,[=]() {
                _unreadMsgCount[it.value().ssid] = 0;
                it.key()->changeStatus(true);
                it.key()->show();
                _conversionWid->addTab(_cardLinkPageHash[it.key()],it.key()->getCardPixmap(),it.key()->getTitle());
            });
        }else {
            auto cP = new ConversationPage(Group, it.value(), _conversionWid);
            cP->hide();
            _cardLinkPageHash[it.key()] = cP;
            connect(it.key(),&ElaInteractiveCard::clicked,this,[=]() {
                _unreadMsgCount[it.value().ssid] = 0;
                it.key()->changeStatus(true);
                it.key()->show();
                _conversionWid->addTab(_cardLinkPageHash[it.key()],it.key()->getCardPixmap(),it.key()->getTitle());
            });
        }
    }
}
