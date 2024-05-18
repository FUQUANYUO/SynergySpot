//
// Created by FU-QAQ on 2024/4/20.
//

#ifndef SERVER_DOEMAILCODE_H
#define SERVER_DOEMAILCODE_H

#include "help.h"
#include <vector>

class DoEmailCode {
public:
    DoEmailCode() = default;
    ~DoEmailCode() = default;

    // 发送验证码给指定的邮箱
    std::string sendEmailCode(std::string &rawdto);
private:
    // 生成验证码
    void genEmailCode(int len);

    std::string emailCode;
};


#endif//SERVER_DOEMAILCODE_H
