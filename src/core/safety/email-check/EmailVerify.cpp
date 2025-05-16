//
// Created by FU-QAQ on 2024/4/18.
//

#include "EmailVerify.h"

#include <utility>
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

void EmailVerify::sendEmailVerifyCode(const std::string& emailAddress,const std::string& queryTime, std::string ssid) {
    startTime.clear();
    startTime = queryTime;

    std::string outEdto;
    SSDTO::EmailVerifyDTO evdto;
    evdto.set_is_request(true);
    evdto.set_email_address(emailAddress);
    evdto.set_start_time(startTime);
    evdto.set_valid_time(validTime);
    evdto.set_verify_code("");
    evdto.set_request_ssid(ssid);
    evdto.SerializeToString(&outEdto);

    emit g_pClientRequestHandler->sigEmailCodeRequest(outEdto);
}

std::string EmailVerify::getStartTime() {
    return startTime;
}

std::string EmailVerify::getValidTime() {
    return validTime;
}

void EmailVerify::setStartTime(std::string time) {
    startTime = std::move(time);
}

void EmailVerify::setValidTime(std::string time) {
    validTime = std::move(time);
}
