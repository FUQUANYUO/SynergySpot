//
// Created by FU-QAQ on 2025/3/23.
//

#include "NoticePage.h"

#include "ela-widget-tools/ElaInteractiveCard.h"
#include "ela-widget-tools/ElaScrollPage.h"
#include "ela-widget-tools/ElaTheme.h"
#include "ela-widget-tools/ElaToolButton.h"
#include "ela-widget-tools/ElaMenu.h"

#include "common-data/CommonData.h"

#include "../effect-component/SS-mask-widget/SSMaskWidget.h"
#include "../effect-component/loading-dialog/LoadingDialog.h"

#include <QPainter>

NoticePage::NoticePage() {

    initWindow();

    initEdgeLayout();

    initContent();

    initConnectFunc();
}

NoticePage::~NoticePage() {
}

void NoticePage::sltMakeFriendRecord(const UserBaseInfoDTO &dto, NoticeStatus status) {
    if (_noticeCardMap.find(dto.ssid) != _noticeCardMap.end()) {
        _noticeCardMap.value(dto.ssid)->hide();
        _noticeCardMap.value(dto.ssid)->deleteLater();
    }
    ElaInteractiveCard * card = new ElaInteractiveCard(this);
    ElaToolButton * _switchStatusBtn = new ElaToolButton(card);

    QString avatar = (dto.avatarPath=="-1"||dto.avatarPath.isEmpty())?":/contact-page/rc-page/img/SS-default-icon.jpg":dto.avatarPath;
    card->setCardPixmap(avatar);
    card->setSubTitle(QString::fromStdString("请求语句：" + g_pCommonData->getYamlNode()["request-friend-msg"]["default"].as<std::string>()));
    card->setTitle(dto.username + " (" + dto.ssid + ")");
    card->setFixedSize({width(),80});
    card->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    _switchStatusBtn->setIsTransparent(false);
    _switchStatusBtn->setBorderRadius(10);
    _switchStatusBtn->move(card->pos().x() + card->width() - 70,card->pos().y() + card->height() / 2 - 20);

    ElaMenu * addMenu = nullptr;
    QAction * _accept = nullptr;
    QAction * _reject = nullptr;

    switch (status) {
        case Request:
            card->setSubTitle(QString::fromStdString(g_pCommonData->getYamlNode()["request-friend-msg"]["default"].as<std::string>()));
            addMenu = new ElaMenu(this);
            _accept = addMenu->addElaIconAction(ElaIconType::Check, "同意");
            _reject = addMenu->addElaIconAction(ElaIconType::Xmark, "拒绝");
            _switchStatusBtn->setMenu(addMenu);
            _switchStatusBtn->setToolButtonStyle(Qt::ToolButtonTextOnly);

            connect(_accept,&QAction::triggered,this,[=]() {
                emit g_pCommonData->sigReplyFriendOrGroup(dto.ssid,true,false);
            });

            connect(_reject,&QAction::triggered,this,[=]() {
                emit g_pCommonData->sigReplyFriendOrGroup(dto.ssid,false,false);
            });

            break;
        case Waiting:
            _switchStatusBtn->setEnabled(false);
            _switchStatusBtn->setText("等待对方响应");
            break;
        case Accepted:
            _switchStatusBtn->setEnabled(false);
            _switchStatusBtn->setText("已同意");
            break;
        case Rejected:
            _switchStatusBtn->setEnabled(false);
            _switchStatusBtn->setText("已拒绝");
            break;
    }
    _noticeLayout->insertWidget(0,card);
    _noticeCardMap.insert(dto.ssid,card);
}

void NoticePage::sltJoinGroupRecord(const GroupBaseInfoDTO &dto, NoticeStatus status) {
    if (_noticeCardMap.find(dto.ssidGroup) != _noticeCardMap.end()) {
        _noticeCardMap.value(dto.ssidGroup)->hide();
        _noticeCardMap.value(dto.ssidGroup)->deleteLater();
    }
    ElaInteractiveCard * card = new ElaInteractiveCard(this);
    ElaToolButton * _switchStatusBtn = new ElaToolButton(card);

    QString avatar = (dto.avatarPath=="-1"||dto.avatarPath.isEmpty())?":/contact-page/rc-page/img/SS-default-icon.jpg":dto.avatarPath;
    card->setCardPixmap(avatar);
    card->setSubTitle("请求语句：邀请您加入群聊~");
    card->setTitle(dto.groupName + " (" + dto.ssidGroup + ")");
    card->setFixedSize({width(),80});
    card->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    _switchStatusBtn->setIsTransparent(false);
    _switchStatusBtn->setBorderRadius(10);
    _switchStatusBtn->move(card->pos().x() + card->width() - 100,card->pos().y() + card->height() / 2 - 20);

    ElaMenu * addMenu = nullptr;
    QAction * _accept = nullptr;
    QAction * _reject = nullptr;

    switch (status) {
        case Request:
            card->setSubTitle("邀请您加入群聊~");
            addMenu = new ElaMenu(this);
            _accept = addMenu->addElaIconAction(ElaIconType::Check, "同意");
            _reject = addMenu->addElaIconAction(ElaIconType::Xmark, "拒绝");
            _switchStatusBtn->setMenu(addMenu);
            _switchStatusBtn->setToolButtonStyle(Qt::ToolButtonTextOnly);

            connect(_accept,&QAction::triggered,this,[=]() {
                emit g_pCommonData->sigReplyFriendOrGroup(dto.ssidGroup,true,false);
            });

            connect(_reject,&QAction::triggered,this,[=]() {
                emit g_pCommonData->sigReplyFriendOrGroup(dto.ssidGroup,false,false);
            });

            break;
        case Waiting:
            _switchStatusBtn->setEnabled(false);
            _switchStatusBtn->setText("等待对方响应");
            break;
        case Accepted:
            _switchStatusBtn->setEnabled(false);
            _switchStatusBtn->setText("已同意");
            break;
        case Rejected:
            _switchStatusBtn->setEnabled(false);
            _switchStatusBtn->setText("已拒绝");
            break;
    }

    _noticeLayout->insertWidget(0,card);
    _noticeCardMap.insert(dto.ssidGroup,card);
}

void NoticePage::sltShowMaskEffect() {
    _maskWidget->setVisible(true);
    _maskWidget->raise();
    _maskWidget->setFixedSize(this->size());
    _maskWidget->startMaskAnimation(90);
}

void NoticePage::sltHideMaskEffect() {
    _maskWidget->startMaskAnimation(0);
}

void NoticePage::sltShowLoading() {
    sltShowMaskEffect();
    _loadingDialog->setVisible(true);
}

void NoticePage::sltHideLoading() {
    _loadingDialog->setVisible(false);
    sltHideMaskEffect();
}

void NoticePage::initWindow() {
    resize(500,600);
    setFixedSize(500,600);
    setWindowModality(Qt::ApplicationModal);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags((window()->windowFlags()) | Qt::WindowMinimizeButtonHint | Qt::FramelessWindowHint);

    _closeBtn        =   new ElaToolButton(this);
    _noticeScroll    =   new ElaScrollPage(this);
    _noticeWidget    =   new QWidget(_noticeScroll);
    _noticeLayout    =   new QVBoxLayout(_noticeWidget);

    _maskWidget      =   new SSMaskWidget(this);
    _loadingDialog   =   new LoadingDialog(this);

    _mainLayout      =   new QVBoxLayout(this);

    _maskWidget->setParent(this);
    _maskWidget->setVisible(false);
    _loadingDialog->setParent(this);
    _loadingDialog->setVisible(false);
}

void NoticePage::initEdgeLayout() {
    _noticeWidget->setMinimumHeight(700);
    _noticeWidget->setFixedWidth(width());
    _noticeWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    _noticeScroll->setFixedWidth(490);
    _noticeScroll->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    _noticeLayout->setContentsMargins(10,10,10,10);
    _noticeLayout->addStretch();

    QHBoxLayout * closeLayout = new QHBoxLayout;
    closeLayout->addStretch();
    closeLayout->addWidget(_closeBtn);
    closeLayout->setContentsMargins(0,0,0,0);

    _mainLayout->addItem(closeLayout);
    _mainLayout->addWidget(_noticeScroll);
}

void NoticePage::initContent() {
    _closeBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);
    _closeBtn->setElaIcon(ElaIconType::Xmark);
    _closeBtn->setBorderRadius(10);
    _closeBtn->setFixedSize(30,30);

    _noticeScroll->setContentsMargins(0,0,0,0);
    _noticeScroll->setTitleVisible(false);
    _noticeScroll->addCentralWidget(_noticeWidget, true, true, 0);

    _maskWidget->setVisible(false);
    _maskWidget->move(0,0);
    _maskWidget->setFixedSize(this->size());
}

void NoticePage::initConnectFunc() {
    // 关闭
    connect(_closeBtn,&ElaToolButton::clicked,[=]() {
        emit sigHideArchPageMaskEffect();
        this->hide();
    });

    connect(g_pCommonData,&CommonData::sigUpdateAvatarData,this,[=]() {
        _sigUpdateCount++;
        if (_sigUpdateCount >= _waitToReloadCount) {
            _sigUpdateCount = 0;
            _waitToReloadCount = 0;

            for (auto it = _noticeCardMap.begin(); it != _noticeCardMap.end(); it++) {
                auto res = g_pCommonData->getUserInfoBySSID(it.key());
                if ((!res.ssid.isEmpty() || res.ssid != "-1") && (!res.avatarPath.isEmpty() || res.avatarPath != "-1")) {
                    it.value()->setCardPixmap(res.avatarPath);
                }
            }
        }
    });
}

void NoticePage::paintEvent(QPaintEvent *event) {
    // 启用抗锯齿
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    // 清空背景
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::transparent);
    painter.drawRect(rect());

    // 绘制主背景
    painter.setBrush(ElaThemeColor(eTheme->getThemeMode(), DialogBase));
    painter.drawRoundedRect(rect(), 20, 20);

    // 绘制按钮栏背景
    // painter.setBrush(ElaThemeColor(eTheme->getThemeMode(), DialogLayoutArea));
    // painter.drawRoundedRect(QRectF(0, height() - 60, width(), 60), 8, 8);

    QWidget::paintEvent(event);
}