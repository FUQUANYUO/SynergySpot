//
// Created by FU-QAQ on 2024/3/10.
//

#ifndef SERVER_DOGETFRIENDLIST_H
#define SERVER_DOGETFRIENDLIST_H

#include "help.h"

class DoGetFriendList {
public:
    DoGetFriendList() = default;
    ~DoGetFriendList() = default;

    // 查询请求request_ssid的好友列表
    std::string execQueryFriendList(const std::string &dto);

    std::string execQueryFriendListBySSID(std::string ssid);
};


#endif//SERVER_DOGETFRIENDLIST_H
