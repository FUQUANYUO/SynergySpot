//
// Created by FU-QAQ on 2024/2/24.
//

// You may need to build the project (run Qt uic code generator) to get "ui_RegisterPage.h" resolved

#include <QTimer>

#include "RegisterPage.h"
#include "ui_RegisterPage.h"

//-----------      core      -----------//
#include "land/register/AccountRegister.h"

#include "db-pool/ConnectionPool.h"
#include "get-time/GetCurTime.h"

#include "land-page/LandPage.h"

RegisterPage::RegisterPage(QWidget *parent) : QMainWindow(parent), ui(new Ui::RegisterPage) {
    ui->setupUi(this);
    ui->statusBar->setSizeGripEnabled(false);
    // 发送获取验证码请求
    connect(ui->sendVerifyEmailCode,&QPushButton::clicked,this,[=](){
        // 验证邮箱格式
        QString emailAddress = ui->email->text();
        if(emailAddress.contains("@") && emailAddress.count("@") == 1){
            // 一分钟禁用发送按钮
            ui->sendVerifyEmailCode->setDisabled(true);

            QTimer timer;
            connect(&timer,&QTimer::timeout,this,[=](){
                ui->sendVerifyEmailCode->setDisabled(false);
            });
            timer.start(60 * 1000);

            AccountRegister ar(emailAddress.toStdString(),this);
        }
    });

    // 收到验证信息
    connect(this,&RegisterPage::TRANSVERIFYDATA,this,[=](const std::array<std::string,3>& data){
        // 用户注册并验证验证码
        connect(ui->registerBtn,&QPushButton::clicked,this,[=](){
            std::string inputEmailCode = ui->emailCode->text().toStdString();
            if(!inputEmailCode.empty() && inputEmailCode.size() == 6){
                // 验证验证码是否在有效期
                LOG("[email code] : request time " + data[0]);
                LOG("[email code] : cur time " + GetCurTime::getTimeObj()->getCurTime())
                std::time_t requestTimeStamp = GetCurTime::getTimeObj()->transformStrToTimeStamp(data[0]);
                std::time_t curTimeStamp = GetCurTime::getTimeObj()->getCurTimeStamp();
                if(requestTimeStamp + atoi(data[1].c_str()) * 60 > curTimeStamp){
                    // 比对输入与服务端生成是否一致
                    if(inputEmailCode == data[2]){
                        LOG("[email code] : verify success!")
                    }else{
                        LOG("[email code] : input code doesn't equal server generate code!!!")
                    }
                }else{
                    ui->statusBar->showMessage("<font color='red' size=4><b>" + tr("邮箱验证码超过时效期！") + "</b></font>",5000);
                }
            }else{
                ui->statusBar->showMessage("<font color='red' size=4><b>" + tr("输入的验证码不符合格式要求！") + "</b></font>",5000);
            }
        });
    });

    // 注册
    connect(ui->registerBtn,&QPushButton::clicked,this,[=](){
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
    GetCurTime::getTimeObj()->destroyTimeObj();
    delete ui;
}


