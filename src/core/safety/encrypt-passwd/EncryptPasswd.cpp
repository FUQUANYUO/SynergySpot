//
// Created by FU-QAQ on 2024/5/4.
//

#include "EncryptPasswd.h"

#include "encrypt/EncryptString.h"

static std::string pubKey = "*(&(${%synergy_spot_pub%}))";

std::string EncryptPasswd::encrypt(const std::string &doEncryptObj) {
    std::string str = EncryptString::getEncryptObj()->encryptString(doEncryptObj);
    LOG(str)
    return str;
}

bool EncryptPasswd::verifyPasswd(const std::string& inputPasswd,const std::string& encryptPasswd){
    bool res = encrypt(inputPasswd) == encryptPasswd;
    return res;
}

