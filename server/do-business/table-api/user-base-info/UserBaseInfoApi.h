//
// Created by FU-QAQ on 2024/5/10.
//

#ifndef SERVER_USERBASEINFOAPI_H
#define SERVER_USERBASEINFOAPI_H

#include "help.h"

struct UserBaseInfo{
    std::string ssid;
    std::string ssname;
    std::string sex;
    std::string personal_sign;
    std::string birthday;
    short region;
    UserBaseInfo(){
        ssid = "";ssname = "";sex = "";personal_sign = "";birthday = "";region = 2;
    }
};

class UserBaseInfoApi {
public:
    UserBaseInfoApi() = default;
    ~UserBaseInfoApi() = default;

    // 提供给外部操作表的通用接口

    // 修改
    static bool execReviseBaseInfo(const UserBaseInfo& uinfo);

    // 查询
    static std::vector<UserBaseInfo> execQueryBaseInfo(const std::string& keyword);
};

#endif
