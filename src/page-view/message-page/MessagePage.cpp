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
#include <QRegularExpression>
#include <QTabBar>
#include <QHash>
#include <QPointer>
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

bool MessagePage::loadCacheMsg(const QList<MessageContentDTO> &caches) {
    for (const auto& it : caches) {
        bool isGroup = (it.recipient.recipientType==2);
        if (!isGroup) {
            // get user ssid base info
            UserBaseInfoDTO userBaseInfo;
            if (it.senderSSID.toStdString() == g_pCommonData->getCurUserInfo().CurSSID) {
                userBaseInfo = g_pCommonData->getUserInfoBySSID(it.recipient.recipientSSID);
            }else {
                // find recipient base info
                userBaseInfo = g_pCommonData->getUserInfoBySSID(it.senderSSID);
            }
            if (!userBaseInfo.ssid.isEmpty()) {
                addMsgCard({
                    userBaseInfo,{},{},
                    it.content,it.createTime.toMSecsSinceEpoch(),isGroup
                });
            }else {
                LOG_WARNING("not find the ssid : " << it.recipient.recipientSSID.toStdString() << " , then ask for the data to the server database!");
                addMsgCard({
                    {it.recipient.recipientSSID,"NULL",":/message-page/rc-page/img/SS-default-icon.jpg",
                        "男生","UNKNOW",QDateTime::currentDateTime(),0,2,it.createTime},{},{},
                    it.content,it.createTime.toMSecsSinceEpoch(),isGroup
                });
            }
        }else {
            // get group ssid base info
            GroupBaseInfoDTO   gBaseInfo;
            QList<GroupMemberInfoDTO> gMemberInfos;
            if (it.senderSSID.toStdString() == g_pCommonData->getCurUserInfo().CurSSID) {
                gBaseInfo    = g_pCommonData->getGroupInfoDataBySSID(it.recipient.recipientSSID);
                gMemberInfos = g_pCommonData->getGroupMemberInfoData(it.recipient.recipientSSID, 100, 1);
            }else {
                // find recipient base info
                gBaseInfo = g_pCommonData->getGroupInfoDataBySSID(it.senderSSID);
                gMemberInfos = g_pCommonData->getGroupMemberInfoData(it.senderSSID, 100, 1);
            }
            if (!gBaseInfo.ssidGroup.isEmpty()) {
                addMsgCard({
                    {},gBaseInfo,gMemberInfos,
                    it.content,it.createTime.toMSecsSinceEpoch(),isGroup
                });
            }else {
                LOG_WARNING("not find the ssid : " << it.recipient.recipientSSID.toStdString() << " , then ask for the data to the server database!");
                addMsgCard({
                    {},{"","NULL",":/message-page/rc-page/img/SS-default-icon-flat.jpg","","",
                        QDateTime::currentDateTime()},{},
                    it.content,it.createTime.toMSecsSinceEpoch(),isGroup
                });
            }
        }
        addMsgContent(it);
    }
    return true;
}

void MessagePage::addMsgContent(const MessageContentDTO &content) {
    QFont font;
    font.setPointSize(8);
    QRegularExpression imgRegex("<img[^>]*>", QRegularExpression::CaseInsensitiveOption);
    QString html = content.content;
    html.replace(imgRegex,"[图片]");
    QTextDocument docu;
    docu.setHtml(html);

    QString _curSSID = QString::fromStdString(g_pCommonData->getCurUserInfo().CurSSID);
    QString _curName = QString::fromStdString(g_pCommonData->getCurUserInfo().CurSSname);
    QString _avatar = QString::fromStdString(g_pCommonData->getCurUserInfo().CurUserAvatarPath);
    bool isUserSend = (content.senderSSID == _curSSID);
    bool isGroup    = (content.recipient.recipientType==2);// 2 is group type
    if(isUserSend) {
        _ssidLinkCardHash[content.recipient.recipientSSID]->setTimeContent(QString::fromStdString(
        GetCurTime::getTimeObj()->getMsgTypeTime(content.createTime.toMSecsSinceEpoch())),Qt::gray,font);

        // update conversation data
        if (isGroup) {
            _ssidLinkCardHash[content.recipient.recipientSSID]->setSubTitle(_curName + "：" + docu.toPlainText());
            auto typeWid = dynamic_cast<ConversationGroupPage*>(_cardLinkPageHash[_ssidLinkCardHash[content.recipient.recipientSSID]]->getConversationTypePage());
            typeWid->insertMsgBubble({_curSSID,_curName,content.content,_avatar,true});
        }else {
            auto typeWid = dynamic_cast<ConversationFriendPage*>(_cardLinkPageHash[_ssidLinkCardHash[content.recipient.recipientSSID]]->getConversationTypePage());
            _ssidLinkCardHash[content.recipient.recipientSSID]->setSubTitle(docu.toPlainText());
            typeWid->insertMsgBubble({_curSSID,_curName,content.content,_avatar,true});
        }
        _ssidLinkCardHash[content.recipient.recipientSSID]->changeStatus(true);

    }else { // other user send to cur user
        int unreadCount = 0;
        if (!content.recipient.readStatus)
           unreadCount = ++_unreadMsgCount[content.senderSSID];
        _ssidLinkCardHash[content.senderSSID]->setStatusContent((unreadCount>99?"99+":QString::number(unreadCount)),font,40);

        _ssidLinkCardHash[content.senderSSID]->setTimeContent(QString::fromStdString(
        GetCurTime::getTimeObj()->getMsgTypeTime(content.createTime.toMSecsSinceEpoch())),Qt::gray,font);
        _ssidLinkCardHash[content.senderSSID]->setSubTitle(docu.toPlainText());

        // update conversation data
        if (isGroup) {
            auto typeWid = dynamic_cast<ConversationGroupPage*>(_cardLinkPageHash[_ssidLinkCardHash[content.senderSSID]]->getConversationTypePage());
            auto userInfo = g_pCommonData->getUserInfoBySSID(content.senderSSID);
            _ssidLinkCardHash[content.senderSSID]->setSubTitle(userInfo.username + "：" + docu.toPlainText());
            typeWid->insertMsgBubble({content.senderSSID,userInfo.username,content.content,userInfo.avatarPath,true});
        }else {
            auto typeWid = dynamic_cast<ConversationFriendPage*>(_cardLinkPageHash[_ssidLinkCardHash[content.senderSSID]]->getConversationTypePage());
            auto userInfo = g_pCommonData->getUserInfoBySSID(content.senderSSID);
            _ssidLinkCardHash[content.senderSSID]->setSubTitle(docu.toPlainText());
            typeWid->insertMsgBubble({content.senderSSID,userInfo.username,content.content,userInfo.avatarPath,true});
        }
    }
}

void MessagePage::addMsgCard(const MsgCombineDTO &info) {
    QFont font;
    font.setPixelSize(8);

    if (!info.isGroup) { // not group
        if (!_ssidLinkCardHash.contains(info.userBaseInfo.ssid)) { // 没有添加
            ElaInteractiveCard * user = new ElaInteractiveCard(this);
            user->setTitle(info.userBaseInfo.username);
            user->setSubTitle(info.content);
            user->setCardPixmap(QPixmap(info.userBaseInfo.avatarPath));
            user->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
            user->setFixedWidth(245);
            user->setFixedHeight(60);
            user->setCardPixmapSize(50,50);

            _tempMsgWidLayout->insertWidget(_tempMsgWidLayout->count()-1,user);
            _tmpUserMsgList[user] = info;
            _ssidLinkCardHash[info.userBaseInfo.ssid] = user;

            auto cP = new ConversationPage(Friend, info, _conversionWid);
            cP->hide();
            _cardLinkPageHash[user] = cP;

            connect(user,&ElaInteractiveCard::clicked,this,[=]() {
               _unreadMsgCount[info.userBaseInfo.ssid] = 0;
               user->changeStatus(true);
               user->show();
               _conversionWid->addTab(_cardLinkPageHash[user],QPixmap(info.userBaseInfo.avatarPath),info.userBaseInfo.username);
           });
        }
        if (info.timestamp > 0) { // new msg come in
            int unreadCount = ++_unreadMsgCount[info.userBaseInfo.ssid];
            _ssidLinkCardHash[info.userBaseInfo.ssid]->setStatusContent((unreadCount>99?"99+":QString::number(unreadCount)),font,40);
            _ssidLinkCardHash[info.userBaseInfo.ssid]->setSubTitle(info.content);
            _ssidLinkCardHash[info.userBaseInfo.ssid]->setTimeContent(QString::fromStdString(
            GetCurTime::getTimeObj()->getMsgTypeTime(static_cast<std::time_t>(info.timestamp))),Qt::gray,font);
        }
    }else {
        if (!_ssidLinkCardHash.contains(info.groupBaseInfo.ssidGroup)) { // 没有添加
            ElaInteractiveCard * user = new ElaInteractiveCard(this);
            user->setTitle(info.groupBaseInfo.groupName);
            user->setSubTitle(info.content);
            user->setCardPixmap(QPixmap(info.groupBaseInfo.avatarPath));
            user->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
            user->setFixedWidth(245);
            user->setFixedHeight(60);
            user->setCardPixmapSize(50,50);

            _tempMsgWidLayout->insertWidget(_tempMsgWidLayout->count()-1,user);
            _tmpUserMsgList[user] = info;
            _ssidLinkCardHash[info.groupBaseInfo.ssidGroup] = user;

            auto cP = new ConversationPage(Group, info, _conversionWid);
            cP->hide();
            _cardLinkPageHash[user] = cP;

            connect(user,&ElaInteractiveCard::clicked,this,[=]() {
               _unreadMsgCount[info.groupBaseInfo.ssidGroup] = 0;
               user->changeStatus(true);
               user->show();
               _conversionWid->addTab(_cardLinkPageHash[user],QPixmap(info.groupBaseInfo.avatarPath),info.groupBaseInfo.groupName);
           });
        }
        if (info.timestamp > 0) { // new msg come in
            int unreadCount = ++_unreadMsgCount[info.groupBaseInfo.ssidGroup];
            _ssidLinkCardHash[info.groupBaseInfo.ssidGroup]->setStatusContent((unreadCount>99?"99+":QString::number(unreadCount)),font,40);
            _ssidLinkCardHash[info.groupBaseInfo.ssidGroup]->setSubTitle(info.groupBaseInfo.groupName + "：" + info.content);
            _ssidLinkCardHash[info.groupBaseInfo.ssidGroup]->setTimeContent(QString::fromStdString(
                GetCurTime::getTimeObj()->getMsgTypeTime(static_cast<std::time_t>(info.timestamp))),Qt::gray,font
            );
        }
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
    _tempMsgWid->setContentsMargins(0,20,5,0);

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
                _unreadMsgCount[it.value().userBaseInfo.ssid] = 0;
                it.key()->changeStatus(true);
                it.key()->show();
                _conversionWid->addTab(_cardLinkPageHash[it.key()],it.key()->getCardPixmap(),it.key()->getTitle());
            });
        }else {
            auto cP = new ConversationPage(Group, it.value(), _conversionWid);
            cP->hide();
            _cardLinkPageHash[it.key()] = cP;
            connect(it.key(),&ElaInteractiveCard::clicked,this,[=]() {
                _unreadMsgCount[it.value().groupBaseInfo.ssidGroup] = 0;
                it.key()->changeStatus(true);
                it.key()->show();
                _conversionWid->addTab(_cardLinkPageHash[it.key()],it.key()->getCardPixmap(),it.key()->getTitle());
            });
        }
    }
}
