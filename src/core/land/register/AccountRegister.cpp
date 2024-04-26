//
// Created by FU-QAQ on 2024/4/18.
//

#include "AccountRegister.h"

#include "base/business-listen/BusinessListen.h"
#include "land-page/LandPage.h"
#include "land-page/register-page/RegisterPage.h"

#include "get-time/GetCurTime.h"

AccountRegister::AccountRegister(const std::string& emailAddress,QObject *parent) : QObject(parent) {
    BusinessListen * bl = dynamic_cast<LandPage*>(this->parent()->parent())->getBusinessObj();

    // 当前时间点 记录格式 xxxx-xx-xx xx:xx:xx
    ev = new EmailVerify(bl,GetCurTime::getTimeObj()->getCurTime());

    // 接收验证码请求
    connect(bl,&BusinessListen::GET_EMAILCODE,this,[=](const std::string & rawEdto){
        ev->parseEmailVerifyCode(rawEdto);
        emit dynamic_cast<RegisterPage*>(this->parent())->TRANSVERIFYDATA(
                {ev->getStartTime(),ev->getValidTime(),ev->getVerifyCode()});
    });
    ev->sendEmailVerifyCode(emailAddress);
}

AccountRegister::~AccountRegister() {
    GetCurTime::getTimeObj()->destroyTimeObj();
    ev->deleteLater();
}
