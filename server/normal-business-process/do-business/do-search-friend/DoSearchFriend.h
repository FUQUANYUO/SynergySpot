//
// Created by FU-QAQ on 2024/5/9.
//

#ifndef SERVER_DOSEARCHFRIEND_H
#define SERVER_DOSEARCHFRIEND_H

#include "help.h"

class DoSearchFriend {
public:
    DoSearchFriend() = default;
    ~DoSearchFriend() = default;

    // 模糊查询
    std::string execSearchFriendByKeyword(const std::string& dto);
};


#endif//SERVER_DOSEARCHFRIEND_H
