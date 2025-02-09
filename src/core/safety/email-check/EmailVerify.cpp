//
// Created by FU-QAQ on 2024/4/18.
//

#include "EmailVerify.h"
#include "help.h"

#include "DTO.pb.h"

#include "base/client-request-handler/ClientRequestHandler.h"

EmailVerify * EmailVerify::_instance = nullptr;
static std::mutex m;

EmailVerify * EmailVerify::getInstance(){
    if(_instance == nullptr) {
        m.lock();
        if(_instance == nullptr) {
            _instance = new EmailVerify();
        }
        m.unlock();
    }
    return _instance;
}

void EmailVerify::destroyInstance(){
    if(_instance != nullptr) {
        m.lock();
        if(_instance != nullptr) {
            _instance->deleteLater();
            _instance = nullptr;
        }
        m.unlock();
    }
}

EmailVerify::EmailVerify(QObject * bobj) {
    this->validTime = "10";
}

void EmailVerify::sendEmailVerifyCode(const std::string& emailAddress,const std::string& queryTime) {
    startTime.clear();
    startTime = queryTime;

    std::string outEdto;
    SSDTO::EmailVerifyDTO evdto;
    evdto.set_is_request(true);
    evdto.set_email_address(emailAddress);
    evdto.set_start_time(startTime);
    evdto.set_valid_time(validTime);
    evdto.set_verify_code("");
    evdto.SerializeToString(&outEdto);

    g_pClientRequestHandler->addRequest(SSDTO::BusinessType::EMAIL_VERIFY,outEdto);
}

std::string EmailVerify::parseEmailVerifyCode(const std::string& rawdto) {
    validTime.clear();
    SSDTO::EmailVerifyDTO evdto;
    evdto.ParseFromString(rawdto);

    LOG(evdto.start_time())
    if(evdto.start_time() != this->startTime) {
        LOG("the email verify code maybe revise,client start time doesn't equal server start time!!!")
        return "-1";
    }

    this->validTime = evdto.valid_time();
    return evdto.verify_code();
}

std::string EmailVerify::getStartTime() {
    return startTime;
}

std::string EmailVerify::getValidTime() {
    return validTime;
}
