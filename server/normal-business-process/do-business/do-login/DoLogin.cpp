//
// Created by FU-QAQ on 2024/2/28.
//

#include "DoLogin.h"

#include "../../../src/utils/db-pool/ConnectionPool.h"
#include "login/LoginDTO.pb.h"


std::string UsrSSID;
std::string UsrEmail;

bool DoLogin::execVerifyLogin(const std::string& dto,std::string &ssid) {
    SSDTO::Login_DTO ldto;
    ldto.ParseFromString(dto);
    ssid = ldto.ssid();
    char sqlStr[100];
    LOG(ldto.password())
    sprintf(sqlStr,"SELECT `password`,`email` from user_private_info WHERE `ssid`='%s'",
            ldto.ssid().c_str());
    MYSQL_RES * mRes = ConnectionPool::getConnectPool()->getConnection()->query(sqlStr);
    if(mRes == nullptr) {
        LOG("ssid is not exist")
        return false;
    }else{
        MYSQL_ROW row;
        bool res = false;
        while((row = mysql_fetch_row(mRes))){
            std::string comS(row[0]);
            if(comS == ldto.password()){
                LOG("verify success")
                UsrSSID = ssid;
                UsrEmail = row[1];
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
    if(isPass){
        ldto.set_is_pass(true);
        ldto.set_ssname(execQueryUsrSSname(UsrSSID));
        ldto.set_ssid(UsrSSID);
        ldto.set_password("");
        ldto.set_ip("");
        ldto.set_email(UsrEmail);
    }else{
        ldto.set_is_pass(false);
        ldto.set_ssname("");
        ldto.set_ssid(UsrSSID);
        ldto.set_password("");
        ldto.set_ip("");
        ldto.set_email("");
    }
    ldto.SerializeToString(&resDto);
    return resDto;
}

std::string DoLogin::execQueryUsrSSname(const std::string &ssid) {
    char sqlStr[100];
    sprintf(sqlStr,"SELECT `ssname` from user_base_info WHERE `ssid`='%s'",
            ssid.c_str());

    MYSQL_RES * mRes = ConnectionPool::getConnectPool()->getConnection()->query(sqlStr);
    std::string ssname;
    if(mRes == nullptr) {
        LOG("ssid is not exist")
        return ssname;
    }else{
        MYSQL_ROW row;
        while((row = mysql_fetch_row(mRes))){
            ssname = row[0];
        }
        mysql_free_result(mRes);
    }
    return ssname;
}
