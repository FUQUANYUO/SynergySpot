//
// Created by FU-QAQ on 2024/4/29.
//

#ifndef SERVER_DOENROLLACCOUNT_H
#define SERVER_DOENROLLACCOUNT_H

#include "help.h"
#include <mutex>

// 对应表结构
struct UserPrivateInfo{
    std::string ssid;
    std::string status; // 状态
    std::string email; // 邮箱
    std::string password; // 密码
};


class DoEnrollAccount {
public:
    DoEnrollAccount() = default;
    ~DoEnrollAccount() = default;

    // 执行账号注册
    std::string execEnrollAccount(const std::string& dto);

private:
    // 生成账号
    std::string generateAccount();

    // 执行持久化账号信息
    void execStoreAccountInfo(const UserPrivateInfo& uinfo);
};


#endif//SERVER_DOENROLLACCOUNT_H
