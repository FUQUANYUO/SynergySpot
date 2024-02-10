//
// Created by FU-QAQ on 2024/2/10.
//

// You may need to build the project (run Qt uic code generator) to get "ui_LandPage.h" resolved

#include "LandPage.h"
#include "ui_LandPage.h"


LandPage::LandPage(QWidget *parent) : QMainWindow(parent), ui(new Ui::LandPage) {
    ui->setupUi(this);
}

LandPage::~LandPage() {
    delete ui;
}
