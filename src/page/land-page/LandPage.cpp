//
// Created by FU-QAQ on 2024/2/10.
//

// You may need to build the project (run Qt uic code generator) to get "ui_LandPage.h" resolved

#include "LandPage.h"
#include "ui_LandPage.h"

// enroll_type protobuf
#include "enroll/EnrollDTO.pb.h"

LandPage::LandPage(QWidget *parent) : QMainWindow(parent), ui(new Ui::LandPage) {
    ui->setupUi(this);

    // 注册账号
    connect(ui->registerBtn,&QPushButton::clicked,this,[&](){


    });


}

LandPage::~LandPage() {
    delete ui;
}
