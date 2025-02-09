//
// Created by FU-QAQ on 2024/5/4.
//

#include "EncryptPasswd.h"

#include "encrypt/EncryptString.h"


std::string EncryptPasswd::encrypt(const std::string &beEncryptPassword,const std::string & passwordSalt) {
    std::string str = EncryptString::getEncryptObj()->encryptStringBySHA256(beEncryptPassword,passwordSalt);
    LOG_INFO("EncryptPasswd:: " << str);
    return str;
}

bool EncryptPasswd::verifyPasswd(const std::string& inputPasswd,const std::string& encryptPasswd,const std::string& passwordSalt){
    return (encrypt(inputPasswd,passwordSalt) == encryptPasswd);
}

std::string EncryptPasswd::generatePasswdSalt() {
    return EncryptString::getEncryptObj()->generatePasswordSalt();
}

