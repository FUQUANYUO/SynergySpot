//
// Created by FU-QAQ on 2025/4/4.
//

#include "UserScreenShareViewPage.h"
#include <QVBoxLayout>
#include <QLabel>

UserScreenShareViewPage::UserScreenShareViewPage(QWidget *parent) {
    // 基本窗口设置
    setWindowTitle("屏幕分享");
    resize(600, 400);

    // 可添加主布局（根据实际需求扩展）
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // 可在此添加内容控件（根据实际需求扩展）
    QLabel *placeholder = new QLabel("屏幕分享内容区域", this);
    mainLayout->addWidget(placeholder);
}

void UserScreenShareViewPage::retranslateUi() {

}