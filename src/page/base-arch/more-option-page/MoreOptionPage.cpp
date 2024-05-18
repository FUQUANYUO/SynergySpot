//
// Created by FU-QAQ on 2024/2/9.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MoreOptionPage.h" resolved

#include "MoreOptionPage.h"
#include "ui_MoreOptionPage.h"

#include <QMouseEvent>



MoreOptionPage::MoreOptionPage(QWidget *parent) : QWidget(parent), ui(new Ui::MoreOptionPage) {
    ui->setupUi(this);
}

MoreOptionPage::~MoreOptionPage() {
    delete ui;
}