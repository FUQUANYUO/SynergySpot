//
// Created by FU-QAQ on 2024/5/4.
//

#ifndef SYNERGYSPOT_ENCRYPTPASSWD_H
#define SYNERGYSPOT_ENCRYPTPASSWD_H

#include "help.h"

class EncryptPasswd {
public:
    EncryptPasswd() = default;
    ~EncryptPasswd() = default;

    static std::string encrypt(const std::string& doEncryptObj);
    static bool verifyPasswd(const std::string& inputPasswd,const std::string& encryptPasswd);
};


#endif//SYNERGYSPOT_ENCRYPTPASSWD_H
