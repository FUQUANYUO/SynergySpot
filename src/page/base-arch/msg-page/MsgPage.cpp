//
// Created by FU-QAQ on 2024/2/10.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MsgPage.h" resolved

#include "MsgPage.h"
#include "ui_MsgPage.h"


MsgPage::MsgPage(QWidget *parent) : QWidget(parent), ui(new Ui::MsgPage) {
    ui->setupUi(this);
}

MsgPage::~MsgPage() {
    delete ui;
}
