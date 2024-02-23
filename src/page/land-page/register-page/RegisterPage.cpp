//
// Created by FU-QAQ on 2024/2/24.
//

// You may need to build the project (run Qt uic code generator) to get "ui_RegisterPage.h" resolved

#include "RegisterPage.h"
#include "ui_RegisterPage.h"


RegisterPage::RegisterPage(QWidget *parent) : QMainWindow(parent), ui(new Ui::RegisterPage) {
    ui->setupUi(this);
}

RegisterPage::~RegisterPage() {
    delete ui;
}
