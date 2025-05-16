//
// Created by FU-QAQ on 2025/3/16.
//

#include "CreateGroupPage.h"

#include "ela-widget-tools/ElaTreeView.h"
#include "ela-widget-tools/ElaListView.h"
#include "ela-widget-tools/ElaScrollBar.h"
#include "ela-widget-tools/ElaToolButton.h"
#include "ela-widget-tools/ElaTheme.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "../effect-component/SS-mask-widget/SSMaskWidget.h"
#include "../effect-component/loading-dialog/LoadingDialog.h"

#include "../contact-page/ContactPage.h"

#include <QListWidget>
#include <QStandardItemModel>
#include <QPainter>
#include <common-data/CommonData.h>
#include <common-data/common-dto/CommonDatabaseDTO.h>

CreateGroupPage::CreateGroupPage() {

    initWindow();

    initEdgeLayout();

    initContent();

    initConnectFunc();
}

CreateGroupPage::~CreateGroupPage() {
}

void CreateGroupPage::sltShowMaskEffect() {
    _maskWidget->setVisible(true);
    _maskWidget->raise();
    _maskWidget->setFixedSize(this->size());
    _maskWidget->startMaskAnimation(90);
}

void CreateGroupPage::sltHideMaskEffect() {
    _maskWidget->startMaskAnimation(0);
}

void CreateGroupPage::sltShowLoading() {
    sltShowMaskEffect();
    _loadingDialog->setVisible(true);
}

void CreateGroupPage::sltHideLoading() {
    _loadingDialog->setVisible(false);
    sltHideMaskEffect();
}

void CreateGroupPage::sltAddToCreateGroupList(const UserBaseInfoDTO &dto) {
    if (!_selectedList->isVisible()) _selectedList->show();
    if (!_toAddUserHash.contains(dto.ssid)) {
        QString userName = dto.username + " (" + dto.ssid + ")";
        QStandardItem * newItem = new QStandardItem(userName);

        QString userAvatar;
        if (dto.avatarPath.isEmpty() || dto.avatarPath == "-1")
            userAvatar = ":/arch-page/rc-page/img/SS-default-icon.jpg";
        else
            userAvatar = dto.avatarPath;
        QPixmap pixmap(userAvatar);
        pixmap = pixmap.scaled(32, 32, Qt::KeepAspectRatio);

        newItem->setIcon(QIcon(pixmap));

        _dataModel->appendRow(newItem);
        _toAddUserHash.insert(dto.ssid,dto);
    }
}

void CreateGroupPage::initWindow() {
    resize(500,600);
    setFixedSize(500,600);
    setWindowModality(Qt::ApplicationModal);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags((window()->windowFlags()) | Qt::WindowMinimizeButtonHint | Qt::FramelessWindowHint);

    _closeBtn        = new ElaToolButton(this);
    _confirmBtn      = new ElaToolButton(this);

    _mainLayout      = new QVBoxLayout(this);
    _selectedLayout  = new QHBoxLayout();
    _buttonLayout    = new QHBoxLayout();

    _selectedList    = new ElaListView(this);
    _listBar         = new ElaScrollBar(_selectedList->verticalScrollBar(),_selectedList);
    _dataModel       = new QStandardItemModel(_selectedList);

    _maskWidget     =   new SSMaskWidget(this);
    _loadingDialog  =   new LoadingDialog(this);

    _maskWidget->setParent(this);
    _maskWidget->setVisible(false);
    _loadingDialog->setParent(this);
    _loadingDialog->setVisible(false);
}

void CreateGroupPage::initEdgeLayout() {
    QHBoxLayout * closeLayout = new QHBoxLayout;
    closeLayout->addStretch();
    closeLayout->addWidget(_closeBtn);
    closeLayout->setContentsMargins(0,0,10,0);

    _buttonLayout->addStretch();
    _buttonLayout->addWidget(_confirmBtn);
    _buttonLayout->setContentsMargins(0,0,10,20);

    auto treeView = g_pContactPage->getFriendTreeView();
    treeView->setParent(this);
    _selectedLayout->addWidget(treeView);
    _selectedLayout->addWidget(_selectedList);
    _selectedLayout->setContentsMargins(0,0,0,10);

    _mainLayout->addItem(closeLayout);
    _mainLayout->addItem(_selectedLayout);
    _mainLayout->addLayout(_buttonLayout);
    _mainLayout->setContentsMargins(0,0,0,0);

    _selectedList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _selectedList->setAlternatingRowColors(true);
    _listBar->setIsAnimation(true);
}

void CreateGroupPage::initContent() {
    _closeBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);
    _closeBtn->setElaIcon(ElaIconType::Xmark);
    _closeBtn->setBorderRadius(10);
    _closeBtn->setFixedSize(30,30);

    _confirmBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    _confirmBtn->setElaIcon(ElaIconType::CommentCheck);
    _confirmBtn->setText("确认");
    _confirmBtn->setBorderRadius(10);
    _confirmBtn->setFixedSize(60,30);

    _maskWidget->setVisible(false);
    _maskWidget->move(0,0);
    _maskWidget->setFixedSize(this->size());

    _selectedList->setModel(_dataModel);
}

void CreateGroupPage::initConnectFunc() {
    // 关闭
    connect(_closeBtn,&ElaToolButton::clicked,[=]() {
        _dataModel->clear();
        _toAddUserHash.clear();
        emit sigHideArchPageMaskEffect();
        this->hide();
    });

    // confirm
    connect(_confirmBtn,&ElaToolButton::clicked,[=]() {
        emit sigHideArchPageMaskEffect();
        emit g_pCommonData->sigCreateGroupRequest(_toAddUserHash.keys());
        _dataModel->clear();
        _toAddUserHash.clear();
        this->hide();
    });

    // bind contact signal
    connect(g_pContactPage,&ContactPage::sigTriggerAddToCreateGroupList, this, &CreateGroupPage::sltAddToCreateGroupList);

    // 搜索
    // connect(_search,&ElaSuggestBox::sigEnterPressed,[=](const QString& content) {
    //     sltShowLoading();
    //     _closeBtn->raise();
    //     emit g_pCommonData->sigFuzzySearchRequest(content,_typeSwitch->getCurrentIndex()!=0);
    // });
}

void CreateGroupPage::paintEvent(QPaintEvent *event) {
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