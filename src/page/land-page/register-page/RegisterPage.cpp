//
// Created by FU-QAQ on 2024/2/24.
//

// You may need to build the project (run Qt uic code generator) to get "ui_RegisterPage.h" resolved

#include <QTimer>

#include "RegisterPage.h"
#include "ui_RegisterPage.h"

//-----------      core      -----------//
#include "land/register/AccountRegister.h"
#include "safety/encrypt-passwd/EncryptPasswd.h"

// enroll_type protobuf
#include "enroll/EnrollDTO.pb.h"

#include "db-pool/ConnectionPool.h"
#include "get-time/GetCurTime.h"

#include "land-page/LandPage.h"

RegisterPage::RegisterPage(QWidget *parent) : QMainWindow(parent), ui(new Ui::RegisterPage) {
    ui->setupUi(this);
    ui->statusBar->setSizeGripEnabled(false);
    ui->statusBar->setStyleSheet("QStatusBar { font-size: 18pt; font-weight: bold; color: red; }");

    BusinessListen * bl = dynamic_cast<LandPage*>(this->parent())->getBusinessObj();
    _data = {"","",""};
    bool verifyPass = false;


    auto * timer = new QTimer(this);
    // 发送获取验证码请求
    connect(ui->sendVerifyEmailCode,&QPushButton::clicked,this,[=,&verifyPass](){
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
        // 检查昵称、密码、确认密码、邮箱、验证码是否为空
        if(ui->nickname->text().isEmpty() ||
            ui->password->text().isEmpty() ||
            ui->confirmPassword->text().isEmpty() ||
            ui->email->text().isEmpty() ||
            ui->emailCode->text().isEmpty())
        {
            LOG("some info has not completed")
            ui->statusBar->showMessage(tr("请检查相关信息是否填写完整！！！") , 5000);
            return ;
        }

        // 检查密码与确认密码是否相同
        if(ui->password->text() != ui->confirmPassword->text()){
            LOG("please check confirm-password again")
            ui->statusBar->showMessage(tr("请确保两次密码是否一致！！！") , 5000);
            return ;
        }
        std::string inputEmailCode = ui->emailCode->text().toStdString();
        if(_data[0].empty() && _data[1].empty() && _data[2].empty()){
            ui->statusBar->showMessage(tr("客户端出现错误！") , 5000);
        }else{
            std::time_t requestTimeStamp = GetCurTime::getTimeObj()->transformStrToTimeStamp(_data[0]);
            std::time_t curTimeStamp = GetCurTime::getTimeObj()->getCurTimeStamp();
            // 验证验证码是否在有效期
            LOG("[email code] : request time " << _data[0] << " " << requestTimeStamp)
            LOG("[email code] : cur time " << GetCurTime::getTimeObj()->getCurTime() << " " << curTimeStamp)
            if(requestTimeStamp + std::stoi(_data[1]) * 60 > curTimeStamp){
                // 比对输入与服务端生成是否一致
                if(inputEmailCode == _data[2]){
                    LOG("[email code] : verify success!")

                    std::string nickname = ui->nickname->text().toStdString();
                    std::string password = ui->password->text().toStdString();
                    std::string email = ui->email->text().toStdString();

                    ui->statusBar->setStyleSheet("QStatusBar { font-size: 20pt; font-weight: bold; color: green; }");
                    ui->statusBar->showMessage(tr("注册中..."), -1);
                    ui->registerBtn->setDisabled(true);

                    EncryptPasswd ep;
                    std::string out;
                    SSDTO::Enroll_DTO erdto;
                    erdto.set_type(SSDTO::Business_Type::ENROLL);
                    erdto.set_ssid("");
                    erdto.set_user_name(nickname);
                    erdto.set_password(ep.encrypt(password));
                    erdto.set_email(email);
                    erdto.SerializeToString(&out);

                    emit bl->REQUEST_ENROLLACC(out);
                }else{
                    LOG("[email code] : input code doesn't equal server generate code!!!")
                    ui->statusBar->showMessage(tr("验证码不正确，请检查邮件验证码！！！") , 5000);
                }
            }else{
                ui->statusBar->showMessage(tr("验证码已过期，请再次获取验证码！！！"), 5000);
            }
        }
    });

    // 注册成功
    connect(bl,&BusinessListen::GET_ENROLLRES,this,[=](const std::string& rawErdto){
        SSDTO::Enroll_DTO erdto;
        erdto.ParseFromString(rawErdto);

        if(erdto.ssid().empty()){
            LOG("register failed! please check log!")
            ui->statusBar->showMessage(tr("注册失败，请联系客服人员！！！") , 5000);
        }else{
            LOG("register success!")
            ui->statusBar->setStyleSheet("QStatusBar { font-size: 20pt; font-weight: bold; color: green; }");
            ui->statusBar->showMessage(tr("您的账号是 [ ") + QString::fromStdString(erdto.ssid()) + tr(" ], 请牢记并返回登录！"), -1);
        }
    });
}

RegisterPage::~RegisterPage() {
    GetCurTime::getTimeObj()->destroyTimeObj();
    delete ui;
}


