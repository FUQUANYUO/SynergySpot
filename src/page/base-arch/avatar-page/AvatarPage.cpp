//
// Created by FU-QAQ on 2024/2/10.
//

// You may need to build the project (run Qt uic code generator) to get "ui_AvatarPage.h" resolved

#include "AvatarPage.h"
#include "ui_AvatarPage.h"


AvatarPage::AvatarPage(QWidget *parent) : QWidget(parent), ui(new Ui::AvatarPage) {
    ui->setupUi(this);
}

AvatarPage::~AvatarPage() {
    delete ui;
}
