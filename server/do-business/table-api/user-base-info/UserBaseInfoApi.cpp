//
// Created by FU-QAQ on 2024/5/10.
//

#include "UserBaseInfoApi.h"

#include "db-pool/ConnectionPool.h"

#include <sstream>

extern std::mutex m;

bool UserBaseInfoApi::execReviseBaseInfo(const UserBaseInfo &uinfo) {
    // 提取成员
    std::string ssid = uinfo.ssid;
    std::string ssname = uinfo.ssname;
    std::string sex = uinfo.sex;
    std::string personal_sign = uinfo.personal_sign;
    std::string birthday = uinfo.birthday;
    std::string region = std::to_string(uinfo.region);

    std::stringstream sqlForUserBaseInfo;
    sqlForUserBaseInfo << "UPDATE `user_base_info` "
                       << "SET ";
    IF_MEMBER_EXIST(sqlForUserBaseInfo,ssid,"s")
    IF_MEMBER_EXIST(sqlForUserBaseInfo,ssname,"s")
    IF_MEMBER_EXIST(sqlForUserBaseInfo,sex,"s")
    IF_MEMBER_EXIST(sqlForUserBaseInfo,personal_sign,"s")
    IF_MEMBER_EXIST(sqlForUserBaseInfo,birthday,"s")
    IF_MEMBER_EXIST(sqlForUserBaseInfo,region,"ie")

    sqlForUserBaseInfo << "WHERE `ssid`='" << ssid << "'";

    std::lock_guard<std::mutex> lg(m);
    ConnectionPool::getConnectPool()->getConnection()->update(sqlForUserBaseInfo.str());
    return true;
}

std::vector<UserBaseInfo> UserBaseInfoApi::execQueryBaseInfo(const std::string &keyword) {
    std::stringstream sqlForQueryBaseInfo;
    std::vector<UserBaseInfo> arr;
    sqlForQueryBaseInfo << "SELECT `ssid`,`ssname` FROM `user_base_info` "
                        << "WHERE `ssid` LIKE '%" << keyword << "%' OR `ssname` LIKE '%" << keyword << "%'";

    std::lock_guard<std::mutex> lg(m);
    MYSQL_RES *res;
    res = ConnectionPool::getConnectPool()->getConnection()->query(sqlForQueryBaseInfo.str());
    MYSQL_ROW row;
    while((row = mysql_fetch_row(res))){
        UserBaseInfo ubinfo;
        ubinfo.ssid = row[0];
        ubinfo.ssname = row[1];
        arr.push_back(ubinfo);
    }
    return arr;
}
