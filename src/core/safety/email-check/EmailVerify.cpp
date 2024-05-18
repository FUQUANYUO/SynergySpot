//
// Created by FU-QAQ on 2024/4/18.
//

#include "EmailVerify.h"

#include "help.h"

// protobuf 登录业务
#include "email/EmailVerifyCode.pb.h"

#include "base/business-listen/BusinessListen.h"

BusinessListen *bl = nullptr;

EmailVerify::EmailVerify(QObject * bobj,const std::string& queryTime) {
    bl = dynamic_cast<BusinessListen*>(bobj);
    this->validTime = "10";
    this->verifyCode = "-1";
    this->startTime = queryTime;
}

void EmailVerify::sendEmailVerifyCode(const std::string& emailAddress) {
    std::string outEdto;
    SSDTO::EmailVerifyCode_DTO evdto;
    evdto.set_type(SSDTO::Business_Type::GET_EMAILCODE);
    evdto.set_is_request(true);
    evdto.set_email_address(emailAddress);
    evdto.set_start_time(startTime);
    evdto.set_valid_time("");
    evdto.set_verify_code("");
    evdto.SerializeToString(&outEdto);

    emit bl->REQUEST_EMAILCODE(outEdto);
}

void EmailVerify::parseEmailVerifyCode(const std::string& rawdto) {
    SSDTO::EmailVerifyCode_DTO evdto;
    evdto.ParseFromString(rawdto);

    LOG(evdto.start_time())
    if(evdto.start_time() != this->startTime)
        LOG("the email verify code maybe revise,client start time doesn't equal server start time!!!")

    this->verifyCode = evdto.verify_code();
    this->validTime = evdto.valid_time();
}
std::string EmailVerify::getVerifyCode() {
    return verifyCode;
}
std::string EmailVerify::getStartTime() {
    return startTime;
}
std::string EmailVerify::getValidTime() {
    return validTime;
}
