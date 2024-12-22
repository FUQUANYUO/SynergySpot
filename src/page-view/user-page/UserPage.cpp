//
// Created by FU-QAQ on 2024/12/18.
//

#include "UserPage.h"
#include "../effect-component/material-effect/MaterialEffect.h"

#include "ela-widget-tools/ElaInteractiveCard.h"
#include "ela-widget-tools/ElaToolButton.h"
#include "ela-widget-tools/ElaPushButton.h"
#include "ela-widget-tools/ElaText.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPainter>
#include <mutex>


QMap<UserType,UserPage*> UserPage::_userObjMap;
static std::mutex m;

UserPage *UserPage::getInstance(UserType type,UserInfo & info,QWidget * parent) {
    if(_userObjMap.isEmpty()){
        m.lock();
        if(_userObjMap.isEmpty()){
            UserInfo i;
            _userObjMap[UserType::Myself] = new UserPage(UserType::Myself,i,parent);
            _userObjMap[UserType::Strangers] = new UserPage(UserType::Strangers,i,parent);
            _userObjMap[UserType::Friends] = new UserPage(UserType::Friends,i,parent);

            _userObjMap[type]->setInfo(info);
        }
        m.unlock();
    }
    UserPage *p = _userObjMap[type];
    p->setInfo(info);
    return p;
}

void UserPage::destroyUserPage() {
    if(_userObjMap.isEmpty())
        return ;
    else{
        m.lock();
        delete _userObjMap[UserType::Myself];
        delete _userObjMap[UserType::Strangers];
        delete _userObjMap[UserType::Friends];
        _userObjMap.clear();
        m.unlock();
    }
}

UserPage::UserPage(UserType type,UserInfo &info,QWidget *parent) : ElaWidget(parent) {

    initWindow();

    initEdgeLayout();

    initContent();

    initConnectFunc();

    switch(type){
        case Myself:
            _editUserButton->show();
            _addFriendButton->hide();
            _callButton->hide();
            break;
        case Strangers:
            _addFriendButton->show();
            _editUserButton->hide();
            _callButton->hide();
            break;
        case Friends:
            _callButton->show();
            _addFriendButton->hide();
            _editUserButton->hide();
            break;
    }
    setInfo(info);
}

UserPage::~UserPage() {
}

void UserPage::initWindow() {
    setWindowButtonFlag(ElaAppBarType::StayTopButtonHint,false);
    setWindowButtonFlag(ElaAppBarType::ThemeChangeButtonHint,false);
    setWindowButtonFlag(ElaAppBarType::MinimizeButtonHint,false);
    setWindowButtonFlag(ElaAppBarType::MaximizeButtonHint,false);
    setWindowButtonFlag(ElaAppBarType::CloseButtonHint,false);
    setWindowFlag(Qt::Popup);

    setGraphicsEffect(g_pCommonEffect->setShadowForWidgetBorder(10));
    setWindowTitle("");
    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    this->setIsFixedSize(true);
    this->setFixedSize(300,400);
    _avatarInfo         = new ElaInteractiveCard(this);
    _likeButton         = new ElaToolButton(this);
    _addFriendButton    = new ElaPushButton(this);
    _sendMsgButton      = new QPushButton(this);
    _callButton         = new ElaPushButton(this);
    _editUserButton     = new ElaPushButton(this);
    _remarkText         = new ElaText(this);
    _signContentText    = new ElaText(this);
    _joinDayText        = new ElaText(this);
    _localInfoText      = new ElaText(this);

    _mainLayout         = new QVBoxLayout;
    _buttonLayout       = new QHBoxLayout;
    _textLayout         = new QGridLayout;
}

void UserPage::initEdgeLayout() {
    _textLayout->addWidget(_avatarInfo,0,0,3,2);
    _textLayout->addWidget(_likeButton,1,2,1,1);
    auto *day = new ElaText("加入天数",this);
    auto *remark = new ElaText("备注",this);
    auto *sign = new ElaText("个性签名",this);
    auto *local = new ElaText("所在地",this);
    day->setTextPixelSize(12);
    remark->setTextPixelSize(12);
    sign->setTextPixelSize(12);
    local->setTextPixelSize(12);
    sign->setTextStyle(ElaTextType::BodyStrong);
    remark->setTextStyle(ElaTextType::BodyStrong);
    day->setTextStyle(ElaTextType::BodyStrong);
    local->setTextStyle(ElaTextType::BodyStrong);
    _remarkText->setTextPixelSize(12);
    _signContentText->setTextPixelSize(12);
    _joinDayText->setTextPixelSize(12);
    _localInfoText->setTextPixelSize(12);
    _remarkText->setTextStyle(ElaTextType::Body);
    _signContentText->setTextStyle(ElaTextType::Body);
    _joinDayText->setTextStyle(ElaTextType::Body);
    _localInfoText->setTextStyle(ElaTextType::Body);
    _textLayout->addWidget(day,3,0,1,1);
    _textLayout->addWidget(remark,4,0,1,1);
    _textLayout->addWidget(sign,5,0,1,1);
    _textLayout->addWidget(local,6,0,1,1);
    _textLayout->addWidget(_joinDayText,3,1,1,1);
    _textLayout->addWidget(_remarkText,4,1,1,1);
    _textLayout->addWidget(_signContentText,5,1,1,1);
    _textLayout->addWidget(_localInfoText,6,1,1,1);
    _textLayout->setContentsMargins(0,0,0,0);

    // add | call | edit | send
    _buttonLayout->setSpacing(20);
    _buttonLayout->insertWidget(0,_addFriendButton);
    _buttonLayout->insertWidget(1,_callButton);
    _buttonLayout->insertWidget(2,_editUserButton);
    _buttonLayout->insertWidget(3,_sendMsgButton);
    _buttonLayout->setContentsMargins(0,0,0,0);

    _mainLayout->addItem(_textLayout);
    _mainLayout->addItem(_buttonLayout);
    _mainLayout->setContentsMargins(20,0,20,20);

    this->setLayout(_mainLayout);
    this->setContentsMargins(0,0,0,0);
}

void UserPage::initContent() {
    _avatarInfo->setCardPixmap(QPixmap(":/user-page/rc-page/img/SS-default-icon.jpg"));
    _avatarInfo->setTitle("{test-name}");
    _avatarInfo->setSubTitle("SSID 1129935824");

    _likeButton->setElaIcon(ElaIconType::ThumbsUp);
    _likeButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    _likeButton->setText("9999+");
    _addFriendButton->setBorderRadius(10);
    _addFriendButton->setText("加好友");
    _addFriendButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    _addFriendButton->setFixedHeight(40);

    _sendMsgButton->setText("发消息");
    _sendMsgButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    _sendMsgButton->setFixedHeight(35);
    _sendMsgButton->setStyleSheet(R"(
        QPushButton {
            background-color : rgb(0,153,255);
            color : white;
            border-radius : 10px;
        }
        QPushButton:hover {
            background-color : rgb(0,141,235);
        }
        QPushButton:pressed {
            background-color : rgb(0,141,235);
        }
    )");

    _callButton->setBorderRadius(10);
    _callButton->setText("音视频通话");
    _callButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    _callButton->setFixedHeight(40);

    _editUserButton->setBorderRadius(10);
    _editUserButton->setText("编辑资料");
    _editUserButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    _editUserButton->setFixedHeight(40);

    _remarkText->setText("{Remark}");
    _signContentText->setText("{Sign Text}");
    _localInfoText->setText("{Country/Province}");
    _joinDayText->setText("316");
}

void UserPage::initConnectFunc() {
}

void UserPage::setInfo(const UserInfo &info) {
    if(!info.isEmpty()){
        _avatarInfo->setTitle(info._name);
        _avatarInfo->setSubTitle(info._ssid);
        _avatarInfo->setCardPixmap(QPixmap(info._picPath));

        _likeButton->setText(info._likeCount > 9999 ? QString("9999+"):QString::number(info._likeCount));
        _joinDayText->setText(QString::number(info._joinDay));
        _remarkText->setText(info._remark);
        _signContentText->setText(info._signContent);

        QString setLocalInfo;
        if(!info._localInfo.province.isEmpty() && !info._localInfo.city.isEmpty()){
            setLocalInfo = info._localInfo.province + "·" + info._localInfo.city;
        }
        else{
            setLocalInfo = info._localInfo.country;
        }
        _localInfoText->setText(setLocalInfo);
    }
}
void UserPage::showAt(const QPoint &pos) {
    move(pos);
    show();
}
void UserPage::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addRoundedRect(rect(), 10, 10);
    painter.setClipPath(path);
    painter.fillPath(path, Qt::white);
}
void UserPage::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);

    QPainterPath path;
    path.addRoundedRect(rect(), 10, 10);
    setMask(QRegion(path.toFillPolygon().toPolygon()));
}
