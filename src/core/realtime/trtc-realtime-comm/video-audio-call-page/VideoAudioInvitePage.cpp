//
// Created by FU-QAQ on 2025/4/5.
//

#include "VideoAudioInvitePage.h"

#include "VideoAudioCallPage.h"

#include <QTimer>

#include "../page-view/CommonFunc.hpp"

#include "../page-view/effect-component/cv-process-video-strategy/CVProVideoStrategy.h"
#include "../page-view/plugin-manager/StrategyManager.h"

#include <CommonData.h>
#include <QApplication>
#include <QPainter>
#include <QPainterPath>
#include <QToolButton>


VideoAudioInvitePage::VideoAudioInvitePage(QString curSSID, QString targetSSID,bool isSender) {
    _curUserSSID = curSSID;
    _targetUserSSID = targetSSID;
    _isSender = isSender;

    // init camera data src
    initCameraSrc();

    initWindow();

    initEdgeLayout();

    initContent();

    initConnectFunc();

    ComFunc::moveToCenter(this);
}

VideoAudioInvitePage::~VideoAudioInvitePage() {
    _timer->stop();
    g_pPluginManager->releasePlugin(BK_PLUGIN_NAME);
}

void VideoAudioInvitePage::setHangUpBtnEnable(bool isEnable) {
    hangUpCallBtn->setEnabled(isEnable);
}

void VideoAudioInvitePage::initWindow() {
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(640,480);
    pickUpCallBtn   = new QToolButton(this);
    hangUpCallBtn   = new QToolButton(this);
    avatar          = new QLabel(this);
    userName        = new QLabel(this);
    userInfoDisplay = new QGridLayout;
    btnLayout       = new QHBoxLayout;
    mainLayout      = new QVBoxLayout;
    _timer          = new QTimer(this);
}

void VideoAudioInvitePage::initEdgeLayout() {
    inviteInfo = new QLabel(this);
    inviteInfo->setText("邀请您加入视频通话");

    QFont font;
    font.setPointSize(8);
    inviteInfo->setFont(font);
    font.setPointSize(15);
    userName->setFont(font);

    hangUpCallBtn->setFixedSize(QSize(65,70));
    pickUpCallBtn->setFixedSize(QSize(65,70));

    userInfoDisplay->addWidget(avatar,0,0,2,2);
    userInfoDisplay->addWidget(userName,0,2,1,12);
    userInfoDisplay->addWidget(inviteInfo,1,2,1,12);

    btnLayout->addStretch();
    btnLayout->addWidget(pickUpCallBtn);
    btnLayout->addWidget(hangUpCallBtn);
    btnLayout->addStretch();
    btnLayout->setSpacing(70);
    btnLayout->setContentsMargins(20,20,20,20);

    mainLayout->addLayout(userInfoDisplay);
    mainLayout->addStretch();
    mainLayout->addLayout(btnLayout);
    mainLayout->setContentsMargins(50,60,20,20);
    setLayout(mainLayout);
}

void VideoAudioInvitePage::initContent() {
    auto res = g_pCommonData->getUserInfoBySSID(_targetUserSSID);
    if (res.ssid.isEmpty() || res.ssid == "-1") {
        // 填充信息
        res.ssid = _targetUserSSID;
        res.username = _targetUserSSID;
    }
    if (res.avatarPath.isEmpty() || res.avatarPath == "-1") {
        res.avatarPath = ":/realtime/rc-page/img/SS-default-icon.jpg";
    }
    hangUpCallBtn->setStyleSheet("color: rgb(255, 255, 255);border-radius: 20px;");
    pickUpCallBtn->setStyleSheet("color: rgb(255, 255, 255);border-radius: 20px;");
    hangUpCallBtn->setIcon(QPixmap(":/realtime/rc-page/img/hang-up-rect.png"));
    hangUpCallBtn->setIconSize(QSize(40,40));
    pickUpCallBtn->setIcon(QPixmap(":/realtime/rc-page/img/pick-up.png"));
    pickUpCallBtn->setIconSize(QSize(40,40));
    hangUpCallBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    pickUpCallBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    hangUpCallBtn->setText("挂断");
    pickUpCallBtn->setText("接听");

    userName->setText(res.username);
    avatar->setFixedSize(70,70);
    QPixmap pix(res.avatarPath);
    pix = pix.scaled(avatar->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation); // 高质量缩放
    avatar->setPixmap(pix);
    avatar->setStyleSheet("border-radius: 20px;");

    inviteInfo->setStyleSheet("color: rgb(255, 255, 255);");
    userName->setStyleSheet("color: rgb(255, 255, 255);");

    if (_isSender) {
        avatar->hide();
        userName->hide();
        inviteInfo->hide();
        pickUpCallBtn->hide();
    }else {
        avatar->show();
        userName->show();
        inviteInfo->show();
        pickUpCallBtn->show();
    }
}

void VideoAudioInvitePage::initConnectFunc() {
    connect(_timer, &QTimer::timeout, this, &VideoAudioInvitePage::sltUpdateFrame);
    connect(pickUpCallBtn,&QToolButton::clicked,this,[=]() {
        _timer->stop();
        g_pPluginManager->destroyInstance();
        emit sigUserProcessResult(true);
    });
    connect(hangUpCallBtn,&QToolButton::clicked,this,[=]() {
        _timer->stop();
        g_pPluginManager->destroyInstance();
        emit sigUserProcessResult(false);
    });

    _timer->start(16);
}

void VideoAudioInvitePage::initCameraSrc() {
    QString execPath = QApplication::applicationDirPath();

    QString BKStrategyPluginName;
#ifdef QT_DEBUG
    BKStrategyPluginName = "/EffectComponentPlugind.dll";
#else
    BKStrategyPluginName = "/EffectComponentPlugin.dll";
#endif
    // init Background strategy plugin
    if (!g_pPluginManager->loadPlugin(BK_PLUGIN_NAME, execPath + BKStrategyPluginName)) {
        LOG_ERROR(BK_PLUGIN_NAME << "dont find!!!")
        return;
    }
    // init effect bk source
    dynamic_cast<CVProVideoStrategy *>(g_pPluginManager->getCurrentPlugin(BK_PLUGIN_NAME))->initialize("0");
}

void VideoAudioInvitePage::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addRoundedRect(rect(), 10, 10);
    painter.setClipPath(path);

    auto * curPlugin = g_pPluginManager->getCurrentPlugin(BK_PLUGIN_NAME);
    if (curPlugin && std::string(curPlugin->getPluginName()) == BK_PLUGIN_NAME) {
        QImage frame = dynamic_cast<CVProVideoStrategy*>(curPlugin)->getCurrentFrame();
        if (!frame.isNull()) {
            painter.drawImage(rect(), frame);
        }
    }
    // 绘制亚克力效果
    QColor acrylicColor(0, 0, 0, 120);
    painter.setBrush(acrylicColor);
    painter.setPen(Qt::NoPen);
    painter.drawRect(rect());
}

void VideoAudioInvitePage::sltUpdateFrame() {
    auto * curPlugin = g_pPluginManager->getCurrentPlugin(BK_PLUGIN_NAME);
    if(curPlugin && std::string(curPlugin->getPluginName()) == BK_PLUGIN_NAME){
        dynamic_cast<CVProVideoStrategy*>(curPlugin)->updateFrame();
        update();
    }
}