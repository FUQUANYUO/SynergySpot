//
// Created by FU-QAQ on 2024/5/9.
//

#ifndef SERVER_DOADDFRIEND_H
#define SERVER_DOADDFRIEND_H

#include "help.h"

// 好友表对应结构
struct Friendship{
    int id;
    std::string ssid;
    std::string friend_ssid;
    int ship_status;
    std::string create_time;
    Friendship(){
        id = -1;ssid = "";friend_ssid = "";ship_status = 0;create_time = "";
    }
};

// 群组对应表结构
struct GroupMemberInfo{
    int id;
    std::string ssid_group;
    std::string ssid_member;
    std::string create_time;
    GroupMemberInfo(){
        id = -1;ssid_group = "";ssid_member = "";create_time = "";
    }
};


class DoAddFriend {
public:
    DoAddFriend();
    ~DoAddFriend() = default;

    // 执行添加好友
    std::string execAddFriend(const std::string& dto);
private:
    // 持久化用户好友数据
    void execStoreContactInfo(const Friendship& fship);

    // 持久化群组成员数据
    void execStoreContactInfo(const GroupMemberInfo& gminfo);
};


#endif//SERVER_DOADDFRIEND_H
