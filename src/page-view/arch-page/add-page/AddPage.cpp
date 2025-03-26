//
// Created by FU-QAQ on 2025/3/16.
//
#include "AddPage.h"

#include "ela-widget-tools/ElaInteractiveCard.h"
#include "ela-widget-tools/ElaPivot.h"
#include "ela-widget-tools/ElaScrollPage.h"
#include "ela-widget-tools/ElaSuggestBox.h"
#include "ela-widget-tools/ElaTheme.h"
#include "ela-widget-tools/ElaToolButton.h"

#include "common-data/CommonData.h"

#include "../effect-component/SS-mask-widget/SSMaskWidget.h"
#include "../effect-component/loading-dialog/LoadingDialog.h"

#include <QPainter>

AddPage::AddPage() {

    initWindow();

    initEdgeLayout();

    initContent();

    initConnectFunc();
}

AddPage::~AddPage() {
}

void AddPage::sltShowMaskEffect() {
    _maskWidget->setVisible(true);
    _maskWidget->raise();
    _maskWidget->setFixedSize(this->size());
    _maskWidget->startMaskAnimation(90);
}

void AddPage::sltHideMaskEffect() {
    _maskWidget->startMaskAnimation(0);
}

void AddPage::sltShowLoading() {
    sltShowMaskEffect();
    _loadingDialog->setVisible(true);
}

void AddPage::sltHideLoading() {
    _loadingDialog->setVisible(false);
    sltHideMaskEffect();
}

void AddPage::sltAddUserRes(const UserBaseInfoDTO &dto) {
    ElaInteractiveCard * card = new ElaInteractiveCard(this);
    ElaToolButton * addBtn = new ElaToolButton(card);

    QString avatar = (dto.avatarPath=="-1"||dto.avatarPath.isEmpty())?":/arch-page/rc-page/img/SS-default-icon.jpg":dto.avatarPath;
    card->setCardPixmap(avatar);
    card->setTitle(dto.username + " (" + dto.ssid + ")");
    card->setSubTitle(dto.personalSign);
    card->setFixedSize({width(),80});
    card->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    addBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);
    addBtn->setElaIcon(ElaIconType::Plus);
    addBtn->setBorderRadius(10);
    addBtn->setFixedSize(30,30);
    addBtn->move(card->pos().x() + card->width() - 70,card->pos().y() + card->height() / 2 - 15);

    _userResLayout->insertWidget(_userResLayout->count()-1,card);
    _userResMap.insert(dto.ssid,card);

    // 绑定按钮消息
    connect(addBtn,&ElaToolButton::clicked,[=]() {
        emit sigAddBtnClicked(dto.ssid,false);
    });
}

void AddPage::sltAddGroupRes(const GroupBaseInfoDTO &dto) {
    ElaInteractiveCard * card = new ElaInteractiveCard(this);
    ElaToolButton * addBtn = new ElaToolButton(card);
    addBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);
    addBtn->setElaIcon(ElaIconType::Plus);
    addBtn->setBorderRadius(10);
    addBtn->setFixedSize(40,40);
    addBtn->move(card->pos().x() + card->width() - 70,card->pos().y() + card->height() / 2);

    QString avatar = (dto.avatarPath=="-1"||dto.avatarPath.isEmpty())?":/arch-page/rc-page/img/SS-default-icon.jpg":dto.avatarPath;
    card->setCardPixmap(avatar);
    card->setTitle(dto.groupName + " (" + dto.ssidGroup + ")");
    card->setSubTitle(dto.profile);

    _groupResLayout->addWidget(card);
    _groupResMap.insert(dto.ssidGroup,card);

    // 绑定按钮消息
    connect(addBtn,&ElaToolButton::clicked,[=]() {
        emit sigAddBtnClicked(dto.ssidGroup,false);
    });
}

void AddPage::initWindow() {
    resize(500,600);
    setFixedSize(500,600);
    setWindowModality(Qt::ApplicationModal);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags((window()->windowFlags()) | Qt::WindowMinimizeButtonHint | Qt::FramelessWindowHint);

    _closeBtn        = new ElaToolButton(this);
    _search          = new ElaSuggestBox(this);
    _typeSwitch      = new ElaPivot(this);
    _userResScroll   = new ElaScrollPage(this);
    _userResWidget   = new QWidget(_userResScroll);
    _groupResScroll  = new ElaScrollPage(this);
    _groupResWidget  = new QWidget(_groupResScroll);

    _userResLayout   = new QVBoxLayout(_userResWidget);
    _groupResLayout  = new QVBoxLayout(_groupResWidget);
    _mainLayout      = new QVBoxLayout(this);

    _maskWidget     =   new SSMaskWidget(this);
    _loadingDialog  =   new LoadingDialog(this);

    _maskWidget->setParent(this);
    _maskWidget->setVisible(false);
    _loadingDialog->setParent(this);
    _loadingDialog->setVisible(false);
}

void AddPage::initEdgeLayout() {
    _search->setFixedSize(480, 50);

    _userResWidget->setMinimumHeight(700);
    _userResWidget->setFixedWidth(width());
    _userResWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    _groupResWidget->setMinimumHeight(700);
    _groupResWidget->setFixedWidth(width());
    _groupResWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    _userResScroll->setFixedWidth(490);
    _userResScroll->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    _groupResScroll->setFixedWidth(490);
    _groupResScroll->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QHBoxLayout * closeLayout = new QHBoxLayout;
    closeLayout->addStretch();
    closeLayout->addWidget(_closeBtn);
    closeLayout->setContentsMargins(0,0,0,0);

    _userResLayout->addStretch();
    _groupResLayout->addStretch();

    _mainLayout->addItem(closeLayout);
    _mainLayout->addWidget(_search);
    _mainLayout->addWidget(_typeSwitch);
    _mainLayout->addWidget(_userResScroll);
    _mainLayout->addWidget(_groupResScroll);
}

void AddPage::initContent() {
    _search->setPlaceholderText("键入关键词回车全网搜索...");

    _typeSwitch->appendPivot("好友");
    _typeSwitch->appendPivot("群组");
    _typeSwitch->setPivotSpacing(15);
    _typeSwitch->setFixedHeight(30);
    _typeSwitch->setCurrentIndex(0);

    _closeBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);
    _closeBtn->setElaIcon(ElaIconType::Xmark);
    _closeBtn->setBorderRadius(10);
    _closeBtn->setFixedSize(30,30);

    _userResScroll->setContentsMargins(0,0,0,0);
    _userResScroll->setTitleVisible(false);
    _userResScroll->addCentralWidget(_userResWidget, true, true, 0);
    _groupResScroll->setContentsMargins(0,0,0,0);
    _groupResScroll->setTitleVisible(false);
    _groupResScroll->addCentralWidget(_groupResWidget, true, true, 0);
    _groupResScroll->hide();

    _maskWidget->setVisible(false);
    _maskWidget->move(0,0);
    _maskWidget->setFixedSize(this->size());
}

void AddPage::initConnectFunc() {
    connect(_typeSwitch, &ElaPivot::pivotClicked, [=](int index) {
        // friend or group
        if (index == 0) {
            _groupResScroll->hide();
            _userResScroll->show();
        } else {
            _userResScroll->hide();
            _groupResScroll->show();
        }
    });

    // 关闭
    connect(_closeBtn,&ElaToolButton::clicked,[=]() {
        emit sigHideArchPageMaskEffect();
        this->hide();
    });

    // 搜索
    connect(_search,&ElaSuggestBox::sigEnterPressed,[=](const QString& content) {
        sltShowLoading();
        _closeBtn->raise();
        emit g_pCommonData->sigFuzzySearchRequest(content,_typeSwitch->getCurrentIndex()!=0);
    });

    connect(g_pCommonData,&CommonData::sigFuzzySearchFriendResponse,this,[=](QList<UserBaseInfoDTO> dto,int waitCount) {
        sltHideLoading();
        _waitToReloadCount += waitCount;
        for (const auto& it : dto) {
            sltAddUserRes(it);
        }
    });

    connect(g_pCommonData,&CommonData::sigFuzzySearchGroupResponse,this,[=](QList<GroupBaseInfoDTO> dto,int waitCount) {
        sltHideLoading();
        _waitToReloadCount += waitCount;
        for (const auto& it : dto) {
            sltAddGroupRes(it);
        }
    });

    connect(g_pCommonData,&CommonData::sigUpdateAvatarData,this,[=]() {
        _sigUpdateCount++;
        if (_sigUpdateCount >= _waitToReloadCount) {
            _sigUpdateCount = 0;
            _waitToReloadCount = 0;

            for (auto it = _userResMap.begin(); it != _userResMap.end(); it++) {
                it.value()->setCardPixmap(g_pCommonData->getUserInfoBySSID(it.key()).avatarPath);
            }

            for (auto it = _groupResMap.begin(); it != _groupResMap.end(); it++) {
                it.value()->setCardPixmap(g_pCommonData->getUserInfoBySSID(it.key()).avatarPath);
            }
        }
    });

    connect(this,&AddPage::sigAddBtnClicked,g_pCommonData,&CommonData::sigAddFriendOrGroup);
}

void AddPage::paintEvent(QPaintEvent *event) {
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