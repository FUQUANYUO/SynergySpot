//
// Created by FU-QAQ on 2024/5/18.
//

// You may need to build the project (run Qt uic code generator) to get "ui_CreateGroupPage.h" resolved

#include "CreateGroupPage.h"
#include "ui_CreateGroupPage.h"


CreateGroupPage::CreateGroupPage(QWidget *parent) : QWidget(parent), ui(new Ui::CreateGroupPage) {
    ui->setupUi(this);
}

CreateGroupPage::~CreateGroupPage() {
    delete ui;
}
