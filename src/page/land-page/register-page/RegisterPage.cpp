//
// Created by FU-QAQ on 2024/2/24.
//

// You may need to build the project (run Qt uic code generator) to get "ui_RegisterPage.h" resolved

#include "RegisterPage.h"
#include "ui_RegisterPage.h"

#include "db-pool/ConnectionPool.h"


RegisterPage::RegisterPage(QWidget *parent) : QMainWindow(parent), ui(new Ui::RegisterPage) {
    ui->setupUi(this);
    // 注册
    connect(ui->registerBtn,&QPushButton::clicked,this,[&](){
        QString inputPassword = ui->password->text();
        QString inputEmail = ui->email->text();

#ifdef LOC_TEST
        // 本地注册
        char sqlStr[1024];
        std::string ssid = std::to_string(10000000 + time(nullptr) % 10000000);
        sprintf(sqlStr,"INSERT INTO user_private_info(`ssid`,`status`,`password`,`email`) VALUES('%s',0,'%s','%s')",
                ssid.c_str(),inputPassword.toStdString().c_str(),inputEmail.toStdString().c_str());
        bool res = ConnectionPool::getConnectPool()->getConnection()->update(sqlStr);
        if(res)
            cout << "register success" << endl;
        else
            cout <<  __FILE__ << " | " << __LINE__ << " | " << __TIME__ << " | " << "register error" << endl;
#else
        // 云端注册
#endif
    });
}

RegisterPage::~RegisterPage() {
    delete ui;
}


