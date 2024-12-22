//
// Created by FU-QAQ on 2024/12/11.
//

#include "MessagePage.h"
#include "conversation-page/ConversationPage.h"

#include "ela-widget-tools/ElaScrollPage.h"
#include "ela-widget-tools/ElaTabWidget.h"
#include "ela-widget-tools/ElaInteractiveCard.h"

#include <QHBoxLayout>
#include <QLabel>
#include <mutex>

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
    _tempMsgList->setMinimumWidth(250);
    _tempMsgList->setMaximumWidth(250);
    _tempMsgList->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    QHBoxLayout * layout = new QHBoxLayout;
    layout->addWidget(_tempMsgList);
    layout->addWidget(_conversionWid);
    layout->setContentsMargins(0,0,0,0);

    _tempMsgWidLayout->setContentsMargins(0,0,0,0);
    _tempMsgWid->setLayout(_tempMsgWidLayout);
    _tempMsgWid->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Expanding);
    _tempMsgWid->setMaximumWidth(250);
    _tempMsgWid->setMinimumHeight(700);
    _tempMsgWid->setMaximumHeight(2000);

    setLayout(layout);
}

void MessagePage::initContent() {
    // TODO: load in local cache or cloud data
    QList<ElaInteractiveCard*> tmpUserMsg;
    ElaInteractiveCard * user_1 = new ElaInteractiveCard(this);
    ElaInteractiveCard * user_2 = new ElaInteractiveCard(this);
    ElaInteractiveCard * user_3 = new ElaInteractiveCard(this);
    tmpUserMsg.append(user_1);tmpUserMsg.append(user_2);tmpUserMsg.append(user_3);

    tmpUserMsg[0]->setTitle("小柴");
    tmpUserMsg[1]->setTitle("五花");
    tmpUserMsg[2]->setTitle("大黄");
    tmpUserMsg[0]->setSubTitle("现在时间是？");
    tmpUserMsg[1]->setSubTitle("😀");
    tmpUserMsg[2]->setSubTitle("666");
    int i = 1;
    foreach(auto user,tmpUserMsg){
        user->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        user->setFixedWidth(250);
        user->setFixedHeight(60);
        user->setCardPixmap(QPixmap(QString(":/message-page/rc-page/img/default-avatar-%1.jpg").arg(i++)));
        user->setCardPixmapSize(50,50);

        _tempMsgWidLayout->addWidget(user);
    }
    _tempMsgWidLayout->addStretch();

    // TODO: connect with tmp msg list
    auto * conversationFriend = new ConversationPage(Friend,_conversionWid);
    auto * conversationGroup = new ConversationPage(Group,_conversionWid);
    _conversionWid->setStyleSheet("ElaTabWidget::pane { border: 0px;}");
    _conversionWid->addTab(conversationFriend, QIcon(":/message-page/rc-page/img/SS-default-icon.jpg"),"Test");
    _conversionWid->addTab(conversationGroup, QIcon(":/message-page/rc-page/img/SS-default-icon.jpg"),"Test");

    _tempMsgList->addCentralWidget(_tempMsgWid, true, true, 0);
    _tempMsgList->setTitleVisible(false);
}

void MessagePage::initConnectFunc() {
}
