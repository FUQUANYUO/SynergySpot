//
// Created by FU-QAQ on 2024/2/28.
//

#include "DoLogin.h"

#include "../../src/utils/db-pool/ConnectionPool.h"
#include "../../src/protofile/login/LoginDTO.pb.h"


bool DoLogin::execVerifyLogin(const std::string& dto) {
    SSDTO::Login_DTO ldto;
    ldto.ParseFromString(dto);
    char sqlStr[100];
    sprintf(sqlStr,"SELECT `password` from user_private_info WHERE `ssid`='%s'",
            ldto.ssid().c_str());
    MYSQL_RES * mRes = ConnectionPool::getConnectPool()->getConnection()->query(sqlStr);
    if(mRes == nullptr) {
        LOG("ssid is not exist")
        return false;
    }else{
        MYSQL_ROW row;
        bool res = false;
        while((row = mysql_fetch_row(mRes))){
            string comS(row[0]);
            if(comS == ldto.password()){
                LOG("verify success")
                res = true;
            }else{
                LOG("the password is wrong then verify error")
            }
        }
        mysql_free_result(mRes);
        return res;
    }
}

std::string DoLogin::sendVerifyRes(bool isPass) {
    std::string resDto;
    SSDTO::Login_DTO ldto;
    ldto.set_type(SSDTO::Business_Type::LOGIN);
    ldto.set_ssid("");
    ldto.set_password("");
    ldto.set_ip("");
    ldto.set_email("");
    if(isPass){
        ldto.set_is_pass(true);
    }else{
        ldto.set_is_pass(false);
    }
    ldto.SerializeToString(&resDto);
    return resDto;
}
