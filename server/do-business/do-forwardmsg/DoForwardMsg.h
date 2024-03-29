//
// Created by FU-QAQ on 2024/3/1.
//

#ifndef SERVER_DOFORWARDMSG_H
#define SERVER_DOFORWARDMSG_H

#include <iostream>

class DoForwardMsg {
public:
    DoForwardMsg() = default;
    ~DoForwardMsg() = default;

    // 执行转发
    void execForward(const std::string &dto);

    // 离线转发
    static void execForwardByMap(std::string curSSID);
};
#endif//SERVER_DOFORWARDMSG_H
