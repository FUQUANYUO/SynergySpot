//
// Created by FU-QAQ on 2024/2/9.
//

// You may need to build the project (run Qt uic code generator) to get "ui_FilePage.h" resolved

#include "FilePage.h"
#include "ui_FilePage.h"


FilePage::FilePage(QWidget *parent) : QMainWindow(parent), ui(new Ui::FilePage) {
    ui->setupUi(this);
}

FilePage::~FilePage() {
    delete ui;
}
