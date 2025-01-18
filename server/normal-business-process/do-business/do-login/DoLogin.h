//
// Created by FU-QAQ on 2024/2/28.
//

#ifndef SERVER_DOLOGIN_H
#define SERVER_DOLOGIN_H

#include "../../src/utils/help.h"

class DoLogin {
public:
    DoLogin() = default;
    ~DoLogin() = default;

    // 执行SQL
    bool execVerifyLogin(const std::string &dto,std::string &ssid);

    // 查询结果包装为dto 返回值为protobuf序列化值
    std::string sendVerifyRes(bool isPass);
private:
    std::string execQueryUsrSSname(const std::string& ssid);
};


#endif//SERVER_DOLOGIN_H
