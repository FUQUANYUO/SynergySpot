//
// Created by FU-QAQ on 2024/5/10.
//

#ifndef SERVER_DOBUSINESSBASE_H
#define SERVER_DOBUSINESSBASE_H

#include "help.h"

class DoBusinessBase {
public:
    DoBusinessBase() = default;
    ~DoBusinessBase() = default;

    static void execBusinessForwardByMap(const std::string& curSSID);
};

#endif//SERVER_DOBUSINESSBASE_H
