//
// Created by FU-QAQ on 2025/4/4.
//

#include "UserVideoGroupPage.h"

#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QScrollArea>

UserVideoGroupPage::UserVideoGroupPage(QWidget *parent) {
   // 主垂直布局
    QVBoxLayout *verticalLayout2 = new QVBoxLayout(this);
    verticalLayout2->setContentsMargins(0, 0, 0, 0);

    // 顶部框架
    QFrame *frame = new QFrame();
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setFrameShadow(QFrame::Raised);
    frame->setLayoutDirection(Qt::LeftToRight);

    QHBoxLayout *horizontalLayout = new QHBoxLayout(frame);
    horizontalLayout->setContentsMargins(0, 0, 0, 0);

    // 水平布局内容
    QSpacerItem *horizontalSpacer2 = new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Minimum);
    horizontalLayout->addItem(horizontalSpacer2);

    QLabel *label37 = new QLabel("网络模式");
    horizontalLayout->addWidget(label37);

    networkModeCb = new QComboBox();
    networkModeCb->addItem("优先清晰");
    networkModeCb->addItem("优先流畅");
    horizontalLayout->addWidget(networkModeCb);

    muteAllRemoteAudioCb = new QCheckBox("远端全部静音");
    horizontalLayout->addWidget(muteAllRemoteAudioCb);

    muteAllRemoteVideoCb = new QCheckBox("远端全部静画");
    horizontalLayout->addWidget(muteAllRemoteVideoCb);

    openDashBoardCb = new QCheckBox("仪表盘");
    horizontalLayout->addWidget(openDashBoardCb);

    checkBoxVolumeEvaluation = new QCheckBox("开启/关闭音量柱");
    checkBoxVolumeEvaluation->setChecked(true);
    horizontalLayout->addWidget(checkBoxVolumeEvaluation);

    pushButtonShowRemoteScreenShare = new QPushButton("显示屏幕分享");
    pushButtonShowRemoteScreenShare->setEnabled(false);
    horizontalLayout->addWidget(pushButtonShowRemoteScreenShare);

    QSpacerItem *horizontalSpacer = new QSpacerItem(70, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout->addItem(horizontalSpacer);

    // 滚动区域
    QScrollArea *scrollView = new QScrollArea();
    scrollView->setWidgetResizable(false);
    scrollView->setFrameShape(QFrame::NoFrame);
    scrollView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    mainVideoPlaceHolder = new QWidget();
    mainVideoPlaceHolder->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrollView->setWidget(mainVideoPlaceHolder);

    // 添加主布局
    verticalLayout2->addWidget(frame);
    verticalLayout2->addWidget(scrollView);

    // 窗口属性
    setWindowTitle("Form");
    setLayoutDirection(Qt::RightToLeft);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}