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
    ui->statusBar->setStyleSheet("QStatusBar { font-size: 18pt; font-weight: bold; color: red; }");

    _data = {"","",""};

    auto * timer = new QTimer(this);
    // 发送获取验证码请求
    connect(ui->sendVerifyEmailCode,&QPushButton::clicked,this,[=](){
        // 验证邮箱格式
        QString emailAddress = ui->email->text();
        if(emailAddress.contains("@") && emailAddress.count("@") == 1){
            // 一分钟禁用发送按钮
            ui->sendVerifyEmailCode->setDisabled(true);
            connect(timer,&QTimer::timeout,this,[=](){
                ui->sendVerifyEmailCode->setDisabled(false);
            });
            timer->start(60 * 1000);
            auto *ar = new AccountRegister(emailAddress.toStdString(),this);
        }
    });

    // 收到验证信息
    connect(this,&RegisterPage::TRANSVERIFYDATA,this,[=](const std::array<std::string,3>& data){
        _data = data;
    });

    // 用户注册并验证验证码
    connect(ui->registerBtn,&QPushButton::clicked,this,[=](){
        if(_data[0].empty() && _data[1].empty() && _data[2].empty()){
            ui->statusBar->showMessage(tr("客户端出现错误！") , 5000);
        }else{
            std::string inputEmailCode = ui->emailCode->text().toStdString();
            if(!inputEmailCode.empty() && inputEmailCode.size() == 6){
                std::time_t requestTimeStamp = GetCurTime::getTimeObj()->transformStrToTimeStamp(_data[0]);
                std::time_t curTimeStamp = GetCurTime::getTimeObj()->getCurTimeStamp();
                // 验证验证码是否在有效期
                LOG("[email code] : request time " << _data[0] << " " << requestTimeStamp)
                LOG("[email code] : cur time " << GetCurTime::getTimeObj()->getCurTime() << " " << curTimeStamp)
                if(requestTimeStamp + std::stoi(_data[1]) * 60 > curTimeStamp){
                    // 比对输入与服务端生成是否一致
                    if(inputEmailCode == _data[2]){
                        LOG("[email code] : verify success!")
                        ui->statusBar->setStyleSheet("QStatusBar { font-size: 20pt; font-weight: bold; color: green; }");
                        ui->statusBar->showMessage(tr("验证通过，注册成功！请返回登录！"), -1);
                        ui->registerBtn->setDisabled(true);
                    }else{
                        LOG("[email code] : input code doesn't equal server generate code!!!")
                        ui->statusBar->showMessage(tr("验证码不正确，请检查邮件验证码！！！") , 5000);
                    }
                }else{
                    ui->statusBar->showMessage(tr("验证码已过期，请再次获取验证码！！！"), 5000);
                }
            }else{
                ui->statusBar->showMessage(tr("验证码格式不正确，请再次检查！！！"), 5000);
            }
        }
    });
}

RegisterPage::~RegisterPage() {
    GetCurTime::getTimeObj()->destroyTimeObj();
    delete ui;
}


