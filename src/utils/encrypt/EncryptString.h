//
// Created by FU-QAQ on 2024/5/3.
//

#ifndef SYNERGYSPOT_ENCRYPTSTRING_H
#define SYNERGYSPOT_ENCRYPTSTRING_H

#include "help.h"

class EncryptString {
public:
    static EncryptString *getEncryptObj();
    std::string encryptStringByMD5(const std::string& str);
    std::string encryptStringBySHA256(const std::string& str, const std::string& passwordSalt);
    std::string generatePasswordSalt(size_t length = 16);

    std::string encryptStringByAES(const std::string& str,const std::string encryptKey);
    std::string decryptStringByAES(const std::string& str,const std::string encryptKey);
    void destroyObj();
private:
    static EncryptString * obj;
    EncryptString() = default;
    ~EncryptString() = default;
};


#endif//SYNERGYSPOT_ENCRYPTSTRING_H
