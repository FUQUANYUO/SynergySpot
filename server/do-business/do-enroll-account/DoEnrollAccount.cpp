//
// Created by FU-QAQ on 2024/4/29.
//

#include "DoEnrollAccount.h"
#include "../../../src/utils/db-pool/ConnectionPool.h"
#include "table-api/user-base-info/UserBaseInfoApi.h"

#include "enroll/EnrollDTO.pb.h"

#include <sstream>

extern std::mutex m;

std::string DoEnrollAccount::generateAccount() {
    // 读取当前数据库中最大ssid
    std::stringstream sqlForMaxSSID;
    sqlForMaxSSID << "SELECT MAX(CAST(ssid AS SIGNED)) "
                  << "FROM `user_private_info`";
    // 加锁读取账号
    std::lock_guard<std::mutex> lg(m);

    MYSQL_RES *res;
    res = ConnectionPool::getConnectPool()->getConnection()->query(sqlForMaxSSID.str());

    int baseSSID = 10000000;
    int targetSSID = baseSSID;
    MYSQL_ROW row;
    while((row = mysql_fetch_row(res))){
        if(row[0] == nullptr){
            LOG("query res is empty")
            return std::to_string(targetSSID);
        }
        int maxSSID = std::stoi(row[0]);
        targetSSID = maxSSID + 1;
    }
    return std::to_string(targetSSID);
}

void DoEnrollAccount::execStoreAccountInfo(const UserPrivateInfo& uinfo) {
    std::stringstream sqlForStoreAcc;
    sqlForStoreAcc << "INSERT INTO `user_private_info` "
                   << "(ssid,status,email,password) "
                   << "VALUES ("
                   << SQL_STR(uinfo.ssid) << ","
                   << SQL_STR(uinfo.status) << ","
                   << SQL_STR(uinfo.email) << ","
                   << SQL_STR(uinfo.password) << ")";

    // 写入加锁
    std::lock_guard<std::mutex> lg(m);
    bool res = ConnectionPool::getConnectPool()->getConnection()->update(sqlForStoreAcc.str());
    if(!res){
        LOG("store account info has occur some error!")
    }
}

std::string DoEnrollAccount::execEnrollAccount(const std::string &dto) {
    SSDTO::Enroll_DTO erdto;
    erdto.ParseFromString(dto);

    erdto.set_ssid(generateAccount());
    UserPrivateInfo uPInfo{erdto.ssid(),"0",erdto.email(),erdto.password()};
    // 存储 user_private
    execStoreAccountInfo(uPInfo);

    // 添加昵称
    UserBaseInfo uBInfo;
    uBInfo.ssid = erdto.ssid();
    uBInfo.ssname = erdto.user_name();
    UserBaseInfoApi::execReviseBaseInfo(uBInfo);

    std::string out;
    erdto.SerializeToString(&out);
    return out;
}


