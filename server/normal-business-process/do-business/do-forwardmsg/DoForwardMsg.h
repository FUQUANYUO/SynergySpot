//
// Created by FU-QAQ on 2024/3/1.
//

#ifndef SERVER_DOFORWARDMSG_H
#define SERVER_DOFORWARDMSG_H

#include "help.h"

class DoForwardMsg {
public:
    DoForwardMsg() = default;
    ~DoForwardMsg() = default;

    // 执行转发
    void execForward(const std::string &dto);

    // 离线转发
    static void execForwardByMap(const std::string& curSSID);
private:
    bool isGroup(const std::string& ssid);
    std::string makeJsonData(const std::string& ssid,const std::string& content);
    std::vector<std::string> getGroupMember(const std::string& ssid);
};
#endif//SERVER_DOFORWARDMSG_H
