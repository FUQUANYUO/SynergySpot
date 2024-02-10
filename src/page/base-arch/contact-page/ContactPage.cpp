//
// Created by FU-QAQ on 2024/2/9.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ContactPage.h" resolved

#include "ContactPage.h"
#include "ui_ContactPage.h"


ContactPage::ContactPage(QWidget *parent) : QWidget(parent), ui(new Ui::ContactPage) {
    ui->setupUi(this);
}

ContactPage::~ContactPage() {
    delete ui;
}
