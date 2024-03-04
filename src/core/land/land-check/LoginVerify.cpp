//
// Created by FU-QAQ on 2024/2/25.
//
#include "help.h"
#include "LoginVerify.h"

// protobuf 登录业务
#include "login/LoginDTO.pb.h"

#include "base/business-listen/BusinessListen.h"

// TODO email verify and get ip verify

void LoginVerify::verifyInServer(std::string ssid, std::string password) {
    // 验证 ssid 对应的 password
    SSDTO::Login_DTO ldto;
    ldto.set_type(SSDTO::Business_Type::LOGIN);
    ldto.set_ssid(ssid);
    ldto.set_password(password);
    ldto.set_is_pass(false);
    ldto.set_ip("");
    ldto.set_email("");
    std::string out;
    ldto.SerializeToString(&out);
    emit dynamic_cast<BusinessListen*>(parent())->VERIFY_ACCOUNT(out);
}

LoginVerify::LoginVerify(QObject * parent) : QObject(parent){
}