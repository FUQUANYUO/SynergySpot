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

    static std::string encrypt(const std::string &beEncryptPassword,const std::string & passwordSalt);
    static bool verifyPasswd(const std::string& inputPasswd,const std::string& encryptPasswd,const std::string& passwordSalt);
    static std::string generatePasswdSalt();
};


#endif//SYNERGYSPOT_ENCRYPTPASSWD_H
